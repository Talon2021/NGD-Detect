/*************************************************************************
 * Copyright (C) 2021 All rights reserved.
 *  文件名称：log.h
 *  创 建 者：樊聪
 *  邮    箱：fancong20@163.com
 *  创建日期：2021年09月17日 星期五
 *  描    述：定时刷新日志文件系统
 *           支持如下功能：
 *           1. 独立控制是否写日志文件，是否终端显示；
 *           2. 调试等级自由控制；
 *           3. 日志信息打印自动添加时间，颜色区分等；
 *           4. 加锁支持多线程同时写入文件；
 *           5. 针对写日志，如果日志文件位于磁盘(非内存上面)，为了避免频繁调用IO，
 *              采用异步线程和异步缓冲的形式，避免IO频繁调用；
 *  更新日志：
 *           2021年09月21日 星期二 
 *           1. 添加文件修改标记信息，避免系统反复进行sync，消耗io；
 *           2. 添加可重用性的句柄结构，方便多模块使用；
 *           2021年09月23日 星期四
 *           1. 修复段错误；
 *           2. 修复多线程释放段错误；
 *           3. 添加多行循环覆盖支持；
 *           4. 修复打印等级错误以及console显示的宏错误的BUG；
 *  
 *************************************************************************/

#ifndef __LOG_H__
#define __LOG_H__

#include <stdint.h>

typedef enum 
{
    LOG_DEBUG = 0,   /* debug level messages */
    LOG_INFO  = 1,   /* informational */
    LOG_WARN  = 2,   /* warning conditions */
    LOG_ERROR = 3    /* action must be taken immediately */
} log_level_t;

#define ENABLE_LINE_CIRCLE        // 支持进入行循环模式
#define ENABLE_LOG_CONSOLE    (1)   // 使能终端日志显示
#define DISABLE_LOG_CONSOLE   (0)   // 关闭终端日志显示

typedef struct log_param_s
{
    char *log_file;              // log日志文件，为空表示标准输出打印，不写入日志
    char *log_prefix;            // log信息前缀，为空表示默认"LOG"，一般为模块名
    log_level_t show_log_level;  // 需要显示的log的等级(小于该值的log信息不会显示和写入日志)
    int log_console_enable;      // 是否需要输出到终端显示(支持接口动态修改)
#ifdef ENABLE_LINE_CIRCLE
    int max_lines_into_circle;   // 进入循环的最大行号(日志超过该行号自动循环覆盖，为0表示不循环)
#endif
    int max_chars_in_line;       // 每行最大的字符个数
    int flush_file_time_sec;     // 当前刷新日志文件的时间间隔(log_file非空有效，为0表示不刷新)
    int io_buffer_size;          // IO缓冲区大小(log_file非空有效, 为0表示不设置，使用系统默认buffer)
} log_param_t, *p_log_param_t;

#ifdef __cplusplus
#if __cplusplus
extern "C"
{
#endif
#endif

/*@ preif log_init log模块初始化
 *@ param [in] log_param log模块需要设置的参数
 *@ return 0 成功返回句柄
 *@       -1 初始化失败NULL
 */
void* log_init(const log_param_t *log_param);

/*@ preif log_print log打印
 *@ param [in] log_handler log句柄
 *@ param [in] show_log_level log的等级
 *@ param [in] format 可变参数显示
 *@ return void
 */
void log_print(void *log_handler, log_level_t log_level, const char* format, ...);

/*@ preif log_print_console_enable log允许输出到console
 *@ param [in] log_handler log句柄
 *@ param [in] enable 0:不输出到console 1:输出到console
 *@ return void
 */
void log_print_console_enable(void *log_handler, int enable);

/*@ preif log_flush logs刷新到文件(日志文件存在有效)
 *@ param [in] log_handler log句柄
 *@ return 0 成功
 *@       -1 失败
 */
int log_flush(void *log_handler);

/*@ preif log_deinit logs反初始化
 *@ param [in] log_handler log句柄
 *@ return 0 反初始化成功
 *@       -1 反初始化失败
 */
int log_deinit(void *log_handler);

#ifdef __cplusplus
#if __cplusplus
}
#endif
#endif

#endif // end of __LOG_H__

