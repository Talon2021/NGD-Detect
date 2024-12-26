#ifndef _MPP_VENC_H
#define _MPP_VENC_H

#include <stdint.h>
#include "image_common.h"


typedef struct
{
    uint64_t frame_id;
    uint8_t *img;
    uint32_t size;
    int format;
    int bpp;
    uint32_t widht;
    uint32_t height;
}__attribute__((packed)) img_data;



typedef int (*mpp_recv_raw_fnx)(rgb_data_t* data);

int mpp_venc_init(void);

int mpp_send_frame_callback(mpp_recv_raw_fnx fnx);

int mpp_vi_get_buffer(int ch);

int mpp_thread_exit(void);

int mpp_venc_deinit(void);

int mpp_venc_push_stream(void *stream);
#define MAX_THREAD_NUM (64)

#endif