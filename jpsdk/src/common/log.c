
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdint.h>
#include <fcntl.h>
#include <unistd.h>
#include <libgen.h>
#include <pthread.h>
#include <errno.h>
#include <stdarg.h>
#include <time.h>
#include <sys/time.h>
#include <sys/select.h>
#include <sys/types.h>
#include <sys/stat.h>

#include "colors.h"
#include "log.h"

#define LOG_ERR(format, args...) do{printf(C_LR"[LOG][%s-%d]"C_RST format, basename(__FILE__), __LINE__, ##args);}while(0)
#define LOG_DBG(format, args...) do{printf(C_LG"[LOG][%s-%d]"C_RST format, basename(__FILE__), __LINE__, ##args);}while(0)
#define LOG_INF(format, args...) do{printf(C_LM"[LOG][%s-%d]"C_RST format, basename(__FILE__), __LINE__, ##args);}while(0)
#define LOG_SYS(format, args...) do{printf(C_LR"[LOG][%s-%d](%s) "C_RST format, basename(__FILE__), __LINE__, strerror(errno), ##args);}while(0)

#define LOG_ALLOC(x, size)   do{if (NULL==x)x=calloc(1, (size));}while(0);
#define LOG_FREE(x)          do{if (x) free(x);x = NULL;}while(0);
#define LOG_FCLOSE(x)        do{if (x) fclose(x);x = NULL;}while(0);

#define IS_FILE_IN_MEM(file) ({\
    int stat_ret = 0;\
    struct stat stat_buf = {0};\
    if (0 == stat(file, &stat_buf)) {\
        if (S_ISCHR(stat_buf.st_mode) \
         || S_ISBLK(stat_buf.st_mode) \
         || S_ISFIFO(stat_buf.st_mode) \
         || S_ISSOCK(stat_buf.st_mode)){\
            stat_ret = 1;\
        }\
    }\
    stat_ret;\
})

#define MAX_PREFIX_STR_SIZE    (64)
#define MAX_COLOR_PREFIX_SIZE  (7)
#define MAX_COLOR_SUFFIX_SIZE  (4)
#define MAX_COLOR_BUF_SIZE     (MAX_PREFIX_STR_SIZE+MAX_COLOR_PREFIX_SIZE+MAX_COLOR_SUFFIX_SIZE)

typedef struct log_handler_s{
    FILE *log_fp;
    int file_changed_flag;
    int flush_file_thread_exit;
    int log_file_line_num;
    char *log_recv_buffer;
    char *log_result_buffer;
    char *log_io_buffer;
    pthread_t flush_file_thread_pid;
    pthread_mutex_t write_log_mutex;
    p_log_param_t log_param;
}log_handler_t, *p_log_handler_t;

/* select实现毫秒级别定时器 */
static inline void timer_milliseconds(int m_sec)
{
	if (m_sec < 0)
	{
		return ;
	}

    int err = -1;
    struct timeval tv = {0, 0};

    tv.tv_sec = m_sec / 1000;
    tv.tv_usec = (m_sec % 1000) * 1000;

    do
	{
       err = select(0, NULL, NULL, NULL, &tv);
    }while((err<0) && (EINTR==errno));
}

/* 异步同步写文件线程 */
static void* flush_log_file_thread(void *lparam)
{
    int time_count_sec = 0;
    p_log_handler_t p_handler = (p_log_handler_t)lparam;

    if (NULL == p_handler)
    {
        LOG_ERR("log module is not inited.\n");
        return NULL;
    }

    while (1)
    {
        if (p_handler->flush_file_thread_exit)
        {
            break;
        }

        if ((++time_count_sec) >= (p_handler->log_param->flush_file_time_sec*10))
        {
            if (p_handler->file_changed_flag)
            {
                time_count_sec = 0;
                pthread_mutex_lock(&p_handler->write_log_mutex);
                p_handler->file_changed_flag = 0;
                fflush(p_handler->log_fp);
                fsync(fileno(p_handler->log_fp));
                pthread_mutex_unlock(&p_handler->write_log_mutex);
            }
        }

        timer_milliseconds(100);
    }

    return NULL;
}

/*@ preif log_init log模块初始化
 *@ param [in] log_param log模块需要设置的参数
 *@ return 0 成功返回句柄
 *@       -1 初始化失败NULL
 */
void* log_init(const log_param_t *log_param)
{
    if (NULL == log_param)
    {
        LOG_ERR("Log param invaild!\n");
        return NULL;
    }

    p_log_handler_t p_handler = NULL;
    LOG_ALLOC(p_handler, sizeof(log_handler_t));
    if (NULL == p_handler)
    {
        LOG_SYS("Fail to calloc.\n");
        return NULL;
    }

    LOG_ALLOC(p_handler->log_param, sizeof(log_param_t));
    if (NULL == p_handler->log_param)
    {
        LOG_SYS("Fail to calloc.\n");
        LOG_FREE(p_handler);
        return NULL;
    }

    memcpy(p_handler->log_param, log_param, sizeof(log_param_t));

    if (NULL == p_handler->log_param->log_prefix)
    {
        p_handler->log_param->log_prefix = "LOG";
    }

    LOG_ALLOC(p_handler->log_recv_buffer, p_handler->log_param->max_chars_in_line);
    if (NULL == p_handler->log_recv_buffer)
    {
        LOG_SYS("Fail to calloc.\n");
        LOG_FREE(p_handler->log_param);
        LOG_FREE(p_handler);
        return NULL;
    }

    LOG_ALLOC(p_handler->log_result_buffer, p_handler->log_param->max_chars_in_line+70);
    if (NULL == p_handler->log_result_buffer)
    {
        LOG_SYS("Fail to calloc.\n");
        LOG_FREE(p_handler->log_recv_buffer);
        LOG_FREE(p_handler->log_param);
        LOG_FREE(p_handler);
        return NULL;
    }

    if (p_handler->log_param->log_file)
    {
        p_handler->log_fp = fopen(p_handler->log_param->log_file, "a+");
        if (NULL == p_handler->log_fp)
        {
            LOG_SYS("Fail to fopen %s.\n", p_handler->log_param->log_file);
            LOG_FREE(p_handler->log_result_buffer);
            LOG_FREE(p_handler->log_recv_buffer);
            LOG_FREE(p_handler->log_param);
            LOG_FREE(p_handler);
            return NULL;
        }

#ifdef ENABLE_LINE_CIRCLE
        // get log file line numbers
        fseek(p_handler->log_fp, 0L, SEEK_SET);
        p_handler->log_file_line_num = 0;
        #if 0
        while(!feof(p_handler->log_fp))
        {
            fgets(p_handler->log_result_buffer, p_handler->log_param->max_chars_in_line+70, p_handler->log_fp);
            p_handler->log_file_line_num ++;
        }

        if (p_handler->log_file_line_num >= p_handler->log_param->max_lines_into_circle)
        {
            fseek(p_handler->log_fp, 0L, SEEK_SET);
            p_handler->log_file_line_num = 0;
        }
        #endif
#endif

        if (p_handler->log_param->io_buffer_size)
        {
            LOG_ALLOC(p_handler->log_io_buffer, p_handler->log_param->io_buffer_size);
            if (NULL == p_handler->log_io_buffer)
            {
                LOG_SYS("Fail to called calloc.\n");
                LOG_FCLOSE(p_handler->log_fp);
                LOG_FREE(p_handler->log_result_buffer);
                LOG_FREE(p_handler->log_recv_buffer);
                LOG_FREE(p_handler->log_param);
                LOG_FREE(p_handler);
                return NULL;
            }

            // io stream buffer
            setvbuf(p_handler->log_fp, p_handler->log_io_buffer, _IOFBF, p_handler->log_param->io_buffer_size);
        }
        
        //if ((!IS_FILE_IN_MEM(p_handler->log_param->log_file)) && (p_handler->log_param->flush_file_time_sec))
        if ((p_handler->log_param->flush_file_time_sec))
        {
            if (pthread_create(&p_handler->flush_file_thread_pid, NULL, flush_log_file_thread, (void*)p_handler))
            {
                LOG_SYS("Fail to called pthread_create.\n");
                LOG_FCLOSE(p_handler->log_fp);
                LOG_FREE(p_handler->log_result_buffer);
                LOG_FREE(p_handler->log_recv_buffer);
                LOG_FREE(p_handler->log_param);
                LOG_FREE(p_handler);
                return NULL;
            }
        }
    }

    pthread_mutex_init(&p_handler->write_log_mutex, NULL);

    return p_handler;
}

/*@ preif log_print log打印
 *@ param [in] log_handler log句柄
 *@ param [in] show_log_level log的等级
 *@ param [in] format 可变参数显示
 *@ return void
 */
void log_print(void *log_handler, log_level_t log_level, const char* format, ...)
{
    p_log_handler_t p_handler = (p_log_handler_t)log_handler;
    if (NULL == p_handler)
    {
        //LOG_ERR("log module is not inited.\n");
        return;
    }

    pthread_mutex_lock(&p_handler->write_log_mutex);

    if (NULL == p_handler->log_param || log_level < p_handler->log_param->show_log_level)
    {
        pthread_mutex_unlock(&p_handler->write_log_mutex);
        return;
    }

    va_list list;
    struct tm tm_now = {0};
    struct timeval tv_now = {0};
    char time_tmp[24] = {0};
    char time_buf[MAX_PREFIX_STR_SIZE] = {0};
    char time_buf_color[MAX_COLOR_BUF_SIZE] = {0};

    gettimeofday(&tv_now, NULL);
    localtime_r(&tv_now.tv_sec, &tm_now);
    strftime(time_tmp, sizeof(time_tmp), "%F %T", &tm_now);

    switch(log_level)
    {
        case LOG_DEBUG:
        {
            snprintf(time_buf, sizeof(time_buf), "[%s.%03d][%s][DBG]", 
                     time_tmp, (int)(tv_now.tv_usec / 1000), p_handler->log_param->log_prefix); 
            snprintf(time_buf_color, MAX_COLOR_BUF_SIZE, C_LG"%s"C_RST, time_buf); 
            break;
        }
        case LOG_INFO:
        {
            snprintf(time_buf, sizeof(time_buf), "[%s.%03d][%s][INF]", 
                     time_tmp, (int)(tv_now.tv_usec / 1000), p_handler->log_param->log_prefix); 
            snprintf(time_buf_color, MAX_COLOR_BUF_SIZE, C_LM"%s"C_RST, time_buf); 
            break;
        }
        case LOG_WARN:
        {
            snprintf(time_buf, sizeof(time_buf), "[%s.%03d][%s][WRN]", 
                     time_tmp, (int)(tv_now.tv_usec / 1000), p_handler->log_param->log_prefix); 
            snprintf(time_buf_color, MAX_COLOR_BUF_SIZE, C_LC"%s"C_RST, time_buf); 
            break;
        }
        case LOG_ERROR:
        {
            snprintf(time_buf, sizeof(time_buf), "[%s.%03d][%s][ERR]", 
                     time_tmp, (int)(tv_now.tv_usec / 1000), p_handler->log_param->log_prefix); 
            snprintf(time_buf_color, MAX_COLOR_BUF_SIZE, C_LR"%s"C_RST, time_buf); 
            break;
        }
    }

    va_start(list, format);
    vsnprintf(p_handler->log_recv_buffer, p_handler->log_param->max_chars_in_line, format, list);
    va_end(list);

    if (ENABLE_LOG_CONSOLE == p_handler->log_param->log_console_enable)
    {
        snprintf(p_handler->log_result_buffer, (p_handler->log_param->max_chars_in_line+MAX_COLOR_BUF_SIZE), "%s %s", 
                 time_buf_color, p_handler->log_recv_buffer);
        fprintf(stderr, "%s", p_handler->log_result_buffer);
    }

    if (p_handler->log_fp)
    {
        memset(p_handler->log_result_buffer, 0, (p_handler->log_param->max_chars_in_line+MAX_COLOR_BUF_SIZE));
        snprintf(p_handler->log_result_buffer, (p_handler->log_param->max_chars_in_line+MAX_COLOR_BUF_SIZE), "%s %s", 
                 time_buf, p_handler->log_recv_buffer);
        fwrite(p_handler->log_result_buffer, strlen(p_handler->log_result_buffer), 1, p_handler->log_fp);
        p_handler->file_changed_flag = 1;

#ifdef ENABLE_LINE_CIRCLE
        if (++p_handler->log_file_line_num >= p_handler->log_param->max_lines_into_circle)
        {
            fseek(p_handler->log_fp, 0L, SEEK_SET);
            p_handler->log_file_line_num = 0;
        }
#endif
    }
    pthread_mutex_unlock(&p_handler->write_log_mutex);

    return;
}

/*@ preif log_print_console_enable log允许输出到console
 *@ param [in] log_handler log句柄
 *@ param [in] enable 0:不输出到console 1:输出到console
 *@ return void
 */
void log_print_console_enable(void *log_handler, int enable)
{
    p_log_handler_t p_handler = (p_log_handler_t)log_handler;

    if (p_handler)
    {

        pthread_mutex_lock(&p_handler->write_log_mutex);
        if (NULL == p_handler || NULL == p_handler->log_param)
        {
            LOG_ERR("log module is not inited.\n");        
            pthread_mutex_unlock(&p_handler->write_log_mutex);
            return;
        }
        p_handler->log_param->log_console_enable = enable;
        pthread_mutex_unlock(&p_handler->write_log_mutex);
    }

    return;
}

/*@ preif log_flush logs刷新到文件(日志文件存在有效)
 *@ param [in] log_handler log句柄
 *@ return 0 成功
 *@       -1 失败
 */
int log_flush(void *log_handler)
{
    p_log_handler_t p_handler = (p_log_handler_t)log_handler;

    if (NULL == p_handler)
    {
        LOG_ERR("log module is not inited.\n");
        return -1;
    }

    pthread_mutex_lock(&p_handler->write_log_mutex);
    if (NULL==p_handler->log_fp || IS_FILE_IN_MEM(p_handler->log_param->log_file))
    {
        pthread_mutex_unlock(&p_handler->write_log_mutex);
        return 0;
    }
    fflush(p_handler->log_fp);
    fsync(fileno(p_handler->log_fp));
    pthread_mutex_unlock(&p_handler->write_log_mutex);

    return 0;
}

/*@ preif log_deinit logs反初始化
 *@ param [in] log_handler log句柄
 *@ return 0 反初始化成功
 *@       -1 反初始化失败
 */
int log_deinit(void *log_handler)
{
    p_log_handler_t p_handler = (p_log_handler_t)log_handler;

    if (NULL == p_handler)
    {
        LOG_ERR("log module is not inited.\n");
        return -1;
    }

    pthread_mutex_lock(&p_handler->write_log_mutex);

    // release thread
    if (p_handler->flush_file_thread_pid)
    {
        p_handler->flush_file_thread_exit = 1;
        pthread_join(p_handler->flush_file_thread_pid, NULL);
        p_handler->flush_file_thread_pid = 0;
    }

    // sync log to file
    if (p_handler->log_fp && (!IS_FILE_IN_MEM(p_handler->log_param->log_file)))
    {
        fflush(p_handler->log_fp);
        fsync(fileno(p_handler->log_fp));
        LOG_FCLOSE(p_handler->log_fp);
        LOG_FREE(p_handler->log_io_buffer);
    }
    LOG_FREE(p_handler->log_param);
    LOG_FREE(p_handler->log_recv_buffer);
    LOG_FREE(p_handler->log_result_buffer);
    pthread_mutex_unlock(&p_handler->write_log_mutex);

    pthread_mutex_destroy(&p_handler->write_log_mutex);
    LOG_FREE(p_handler);

    return 0;
}
