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

#include "lgc_algo.h"
#include "detect_api.h"
#include "image_common.h"
#include "mpp_venc.h"
#include "meida_common.h"

static int GetFraneOfshareMem(void *frame)
{
    return 0;
}


static CPlatformChannelContext g_LgcAlgoHandleContext = { 0 };

static int clamp(float val, int min, int max)
{
    return val > min ? (val < max ? val : max) : min;
}
static unsigned char class_colors[][3] = {
        {255, 56, 56},   // 'FF3838'
        {255, 157, 151}, // 'FF9D97'
        {255, 112, 31},  // 'FF701F'
        {255, 178, 29},  // 'FFB21D'
        {207, 210, 49},  // 'CFD231'
        {72, 249, 10},   // '48F90A'
        {146, 204, 23},  // '92CC17'
        {61, 219, 134},  // '3DDB86'
        {26, 147, 52},   // '1A9334'
        {0, 212, 187},   // '00D4BB'
        {44, 153, 168},  // '2C99A8'
        {0, 194, 255},   // '00C2FF'
        {52, 69, 147},   // '344593'
        {100, 115, 255}, // '6473FF'
        {0, 24, 236},    // '0018EC'
        {132, 56, 255},  // '8438FF'
        {82, 0, 133},    // '520085'
        {203, 56, 255},  // 'CB38FF'
        {255, 149, 200}, // 'FF95C8'
        {255, 55, 199}   // 'FF37C7'
    };
#define N_CLASS_COLORS 20

int write_data_to_file(const char *path, const char *data, unsigned int size)
{
    FILE *fp;

    fp = fopen(path, "w+");
    if(fp == NULL) {
        printf("open error: %s\n", path);
        return -1;
    }

    fwrite(data, 1, size, fp);
    fflush(fp);

    fclose(fp);
    return 0;
}
static int GasResultPacket(DETECT_TASK_RESULT *result)
{
    int i = 0;
    CPlatformChannelContext* ctx = &g_LgcAlgoHandleContext;
    GAS_DETECT_RESULT packet = {0};

    SDK_DBG("result =========================count = %d frame id = %d \n", result->gas_list_size, result->frame_id);
    
    packet.frame_id =  result->frame_id;
    for(i = 0; i < result->gas_list_size; i++)
    {
        if(i >= MAX_GAS_NUM)
        {
            break;
        }
        memcpy(&(packet.stGasAttr[packet.u8GasNum].stGasRecth), &(result->gas_list[i].rect), sizeof(DETECT_RECT));
        packet.gas_mask = result->gas_mask_list[0].seg_mask;
        packet.u8GasNum++;
        
    }
    // if(packet.u8GasNum >= 1)
    // {
    //     int width = 640;
    //     int height = 512;
    //     char *ori_img = (char *)result->user_data;
    //     float alpha = 0.5f;
    //     uint8_t *seg_mask  = result->gas_mask_list[0].seg_mask;
    //     for (int j = 0; j < height; j++)
    //     {
    //         for (int k = 0; k < width; k++)
    //         {
    //             int pixel_offset = 3 * (j * width + k);
    //             if (seg_mask[j * width + k] != 0)
    //             {
    //                 ori_img[pixel_offset + 0] = (unsigned char)clamp(class_colors[seg_mask[j * width + k] % N_CLASS_COLORS][0] * (1 - alpha) + ori_img[pixel_offset + 0] * alpha, 0, 255); // r
    //                 ori_img[pixel_offset + 1] = (unsigned char)clamp(class_colors[seg_mask[j * width + k] % N_CLASS_COLORS][1] * (1 - alpha) + ori_img[pixel_offset + 1] * alpha, 0, 255); // g
    //                 ori_img[pixel_offset + 2] = (unsigned char)clamp(class_colors[seg_mask[j * width + k] % N_CLASS_COLORS][2] * (1 - alpha) + ori_img[pixel_offset + 2] * alpha, 0, 255); // b
    //             }
    //         }
    //     }
    //     write_data_to_file("./test_msak.yuv", (char *)result->user_data, 640 * 512 *3);
    // }
    if(ctx->m_GasResultCallBack)
    {
        ctx->m_GasResultCallBack((void *)(&packet), sizeof(GAS_DETECT_RESULT), ctx->m_GasResultUserData);
    }
    return 0;

}

static void* GasFrameSendThread(void* lpParameter)
{
    CPlatformChannelContext* ctx = &g_LgcAlgoHandleContext;
    DETECT_FRAME frame;
    ThreadInfo_t        *pParent = (ThreadInfo_t *)lpParameter;
    size_t bytes_read;
    unsigned char *frame_buff = calloc(1, 640 * 512 * 3);
    static uint64_t frame_id = 0;
    FILE *file = fopen("./output.test2.yuv", "rb");
    while (1)
    {
        if (pParent->m_Exit == 1)
		{
			break;
		}
        bytes_read = fread(frame_buff, 1, 640 * 512 *3 , file);
        if (bytes_read != 640 *512 *3)
        {
            if (feof(file)) 
            {
                fseek(file, 0, SEEK_SET);
            }
        }
        frame.width = 640;
        frame.height = 512;
        frame.frame_date = frame_buff;
        frame.format = IMAGE_FORMAT_RGB888;
        frame.frame_id = frame_id++;
        frame.user_data = frame_buff;

        detect_send_frame_task(ctx->m_Detect_handle, &frame);

        usleep(20 * 1000);
        break;
    }
    fclose(file);
    return NULL;
}

static int GasSendFrame(rgb_data_t *data)
{
    int ret ;
    DETECT_FRAME frame;
    CPlatformChannelContext* ctx = &g_LgcAlgoHandleContext;

    frame.width = data->width;
    frame.height = data->height;
    frame.frame_date = data->data;
    frame.format = IMAGE_FORMAT_RGB888;
    frame.frame_id = data->frame_id;
    frame.user_data = data->data;
    ret = detect_send_frame_task(ctx->m_Detect_handle, &frame);
    if(ret < 0)
    {
        SDK_ERR("send algo frame is err \n");
        return -1;
    }

    return 0;
}

static void* GasResulRecvThread(void* lpParameter)
{
    CPlatformChannelContext* ctx = &g_LgcAlgoHandleContext;
    DETECT_TASK_RESULT result;
    ThreadInfo_t        *pParent = (ThreadInfo_t *)lpParameter;
    while (1)
    {
        if (pParent->m_Exit == 1)
		{
			break;
		}
        memset(&result, 0, sizeof(DETECT_TASK_RESULT));
        if(detect_recv_task_result(ctx->m_Detect_handle, &result))
        {
            SDK_ERR("recv algo result is err\n");
            usleep(20 * 1000);
            continue;
        }
        GasResultPacket(&result);

        if(result.user_data)
        {
            free(result.user_data);
        }
        detect_reuslt_delete((void *)(&result));
        usleep(20 * 1000);
    }
    
    return NULL;
}

int LGC_ALGO_Init(void)
{
    CPlatformChannelContext* ctx = &g_LgcAlgoHandleContext;
    memset(ctx, 0, sizeof(CPlatformChannelContext));
    DETECT_HANDLE_OPTPARAM_S param;

    param.confidence = 0.1;
    param.model_path = "/root/app/ai/yolov8_seg.rknn";
    param.input_width = 1024;
    param.input_height = 1024;

    ctx->m_Detect_handle = detect_init_handle_v2(param);
    if(ctx->m_Detect_handle == NULL)
    {
        SDK_ERR(" detect algo init is err \n");
        return -1;
    }

    //Comm_CreateThread(&ctx->m_GasFrameSendThread, 0, GasFrameSendThread);

    Comm_CreateThread(&ctx->m_GasResultRecvThread, 0, GasResulRecvThread);

    mpp_send_frame_callback(GasSendFrame);

    mpp_vi_get_buffer(0);

    return 0;
}

int LGC_ALGO_DeInit(void)
{
    CPlatformChannelContext* ctx = &g_LgcAlgoHandleContext;
    Comm_DestroyThread(&ctx->m_GasFrameSendThread);

    Comm_DestroyThread(&ctx->m_GasResultRecvThread);

    detect_uninit_handle_v2(&ctx->m_Detect_handle);

    return 0;
}

int LGC_ALGO_RegisterGasResultCb(GasDetectResult_CALLBACK callback, void *userdata)
{
    CPlatformChannelContext* ctx = &g_LgcAlgoHandleContext;

    if (HY_Res_SDK_Lock() != 0)
    {
        SDK_ERR("SDK Not Init\n");
        return ERR_SDK_NOINIT;
    }
    ctx->m_GasResultCallBack = callback;
    ctx->m_GasResultUserData = userdata;
    HY_Res_SDK_UnLock();
    return 0;
}