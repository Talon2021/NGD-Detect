#include <pthread.h>

#include "mpp_venc.h"
#include "meida_common.h"
#include "image_common.h"
#include "safe_quene.hpp"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/ipc.h>
#include <sys/shm.h>
#include <sys/sem.h>
#include <unistd.h>
#include <signal.h>
#include <time.h>
#include <stdint.h>
#include <sys/time.h>

typedef struct mpp_venc_handler_s{
    pthread_t pids[MAX_THREAD_NUM];
    int pthread_exit[MAX_THREAD_NUM];
   
    int phtread_count;
    pthread_mutex_t thread_lock;

    mpp_recv_raw_fnx m_sendFrameFnx;

    SafeQueuee<img_data*> *m_ViStream;

}mpp_venc_handler_t;

static mpp_venc_handler_t *g_mpp_venc_handler = NULL;

static void yuv420_to_rgb(const uint8_t* yuv_data, uint8_t* rgb_data, int width, int height) {
    int frame_size = width * height;
    for (int i = 0; i < frame_size; ++i) {
        uint8_t y = yuv_data[i];  

        rgb_data[3 * i] = y;     // R
        rgb_data[3 * i + 1] = y; // G
        rgb_data[3 * i + 2] = y; // B
    }
}

static void *mpp_vi_get_buffer_thread(void *arg) 
{
    int chn = (int)arg;
    int ret = 0;
    mpp_venc_handler_t* ctx = (mpp_venc_handler_t*)g_mpp_venc_handler;
    pthread_mutex_lock(&ctx->thread_lock);  
    int pthread_index = ctx->phtread_count;
    ctx->pids[ctx->phtread_count] = pthread_self();        
    ctx->phtread_count ++;
    pthread_mutex_unlock(&ctx->thread_lock);
    //static u_int32_t frame_id = 0;
    img_data *data;
    struct sembuf sem_op;
    SDK_DBG("vi get buff is start \n");
    

    while (1)
    {
        if (ctx->pthread_exit[pthread_index])
        {
            SDK_DBG("pthread_exit.\n");
            break;
        }
        sem_op.sem_num = 0;
        sem_op.sem_op = -1;
        sem_op.sem_flg = 0;
        
        g_mpp_venc_handler->m_ViStream->pop(data);
        
        u_int8_t *rbg_data = (u_int8_t *)malloc(data->widht * data->height *3);
        yuv420_to_rgb(data->img, rbg_data, data->widht, data->height);
        rgb_data_t rgb_data_s;
        rgb_data_s.data = rbg_data;
        rgb_data_s.width = data->widht;
        rgb_data_s.height = data->height;
        rgb_data_s.size = data->widht * data->height * 3;
        rgb_data_s.fmt = IMAGE_FORMAT_RGB888;
        rgb_data_s.frame_id = data->frame_id;
        if(ctx->m_sendFrameFnx)
        {
            ret = ctx->m_sendFrameFnx(&rgb_data_s);
            if(ret < 0)
            {
                free(rbg_data);
            }
        }
        
        free(data->img);
        free(data);
        data = NULL;
    }
    return NULL;
    
}

int mpp_venc_init(void)
{
     if(g_mpp_venc_handler == NULL)
    {
        SDK_ALLOC(g_mpp_venc_handler, mpp_venc_handler_t, sizeof(mpp_venc_handler_t));
        if(NULL == g_mpp_venc_handler)
        {
            SDK_ERR("Fail to calloc g_mpp_venc_handler.\n");
            return -1;
        }
        pthread_mutex_init(&g_mpp_venc_handler->thread_lock, NULL);
        g_mpp_venc_handler->m_ViStream = new SafeQueuee<img_data*>(true, 32);
    }
    return 0;
}

int mpp_send_frame_callback(mpp_recv_raw_fnx fnx)
{
    mpp_venc_handler_t* ctx = (mpp_venc_handler_t*)g_mpp_venc_handler;
    ctx->m_sendFrameFnx = fnx;
    return 0;
}

int mpp_vi_get_buffer(int ch)
{
    pthread_t th = 0;
	int idx = ch;
	
	if (pthread_create(&th, NULL, mpp_vi_get_buffer_thread, (void*)idx)) 
	{
        SDK_ERR("create mpp_vi_get_buffer_thread %d thread failed!\n", ch);
        return -1;
	}

	return 0;
}

int mpp_thread_exit(void)
{
    mpp_venc_handler_t* ctx = g_mpp_venc_handler;

    for (int i=0; i<=ctx->phtread_count; i++)
    {
        if (ctx->pids[i])
        {
            ctx->pthread_exit[i] = 1;
            pthread_join(ctx->pids[i], NULL);
            ctx->pthread_exit[i] = 0;
            ctx->pids[i] = 0;
        }
    }
    ctx->phtread_count = 0;
    return 0;
}

int mpp_venc_deinit(void)
{
    if(g_mpp_venc_handler == NULL)
    {
        SDK_ERR("NULL POINT EXCEPTION \n");
        return -1;
    }
    mpp_thread_exit();
    pthread_mutex_destroy(&g_mpp_venc_handler->thread_lock);
    //delete (g_mpp_venc_handler->frame_queue_handle);
    SDK_FREE(g_mpp_venc_handler);
    return 0;
}

int mpp_venc_push_stream(void *stream)
{
    if(g_mpp_venc_handler == NULL)
    {
        SDK_ERR("NULL POINT EXCEPTION \n");
        return -1;
    }
    if(g_mpp_venc_handler->m_ViStream->full())
    {
        SDK_ERR("stream chandle is full \n");
        return -1;
    }
    img_data *tmp_stream = (img_data *)stream;
    mpp_venc_handler_t* ctx = (mpp_venc_handler_t*)g_mpp_venc_handler;

    img_data *data = calloc(1, sizeof(img_data));
    memcpy(data, tmp_stream, sizeof(img_data));

    data->img = malloc(tmp_stream->size);

    memcpy(data->img, tmp_stream->img, tmp_stream->size);
    g_mpp_venc_handler->m_ViStream->push(data);
    return 0;
}
