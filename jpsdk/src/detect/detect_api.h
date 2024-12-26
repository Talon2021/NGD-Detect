#ifndef DETECT_API_H

#define DETECT_API_H
#include "image_common.h"
#include <stdint.h>
#ifndef __ASYNC_ALG_H
#define __ASYNC_ALG_H


typedef enum DETECT_IMAGE_TYPE_ENUM 
{
    DETECT_IMAGE_NV21 = 0,
    DETECT_IMAGE_RGB888,
    DETECT_IMAGE_BGR888,
} DETECT_IMAGE_TYPE_E;

typedef enum TRACK_STATUS_ENUM
{
    MGVL1_TRACK_STATUS_NEW = 0x00,  	    //TRACK new
    MGVL1_TRACK_STATUS_UPDATE,  	        //TRACK need to be updated
    MGVL1_TRACK_STATUS_DIE,			   	    //TRACK died
	MGVL1_TRACK_STATUS_MAX,
}TRACK_STATUS_E;

typedef enum DETECT_HANDLE_TYPE_ENUM
{
    MGVL1_HANDLE_TYPE_VIDEO = 1, //@ video detection
    MGVL1_HANDLE_TYPE_IMAGE,     //@ image detection
    MGVL1_HANDLE_TYPE_MAX
}DETECT_HANDLE_TYPE_E;

typedef struct DETECT_HANDLE_OPTPARAM_ST
{
    const char *model_path;
    float confidence;
    int input_width;
    int input_height;
}DETECT_HANDLE_OPTPARAM_S;

typedef struct DETECT_RECT_ST
{
    int left;
    int top;
    int right;
    int bottom;
}DETECT_RECT;

typedef struct DETECT_FRAME_ST
{
    unsigned char *frame_date;
    int width;
    int height;
    int format;
    DETECT_IMAGE_TYPE_E TYPE;
    uint64_t frame_id;
    uint64_t track_id;

    void *user_data;

} DETECT_FRAME;

typedef struct DETECT_TASK_COMMON_ST
{
    uint64_t frame_id;
    uint64_t track_id;
    DETECT_RECT rect;
    TRACK_STATUS_E status;
    double confidence;
} DETECT_TASK_COMMON;

typedef struct DETECT_TASK_SETMASK_ST
{
   uint8_t *seg_mask;
} DETECT_TASK_SETMASK;

typedef struct DETECT_TASK_RESULT_ST
{
    uint64_t frame_id;
    DETECT_TASK_COMMON *gas_list;
    DETECT_TASK_SETMASK *gas_mask_list;
    int gas_list_size;

    void *user_data;
} DETECT_TASK_RESULT;




void *detect_init_handle_v2(DETECT_HANDLE_OPTPARAM_S param);

int detect_uninit_handle_v2(void **handle);

int detect_send_frame_task(void *handle, DETECT_FRAME *frame);

int detect_recv_task_result(void *handle, DETECT_TASK_RESULT *result);

int detect_reuslt_delete(void *p);


#endif


#endif