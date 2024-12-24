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
static int GetFraneOfshareMem(void *frame)
{
    return 0;
}


static CPlatformChannelContext g_LgcAlgoHandleContext = { 0 };


static int GasResultPacket(DETECT_TASK_RESULT *result)
{
    int i = 0;
    CPlatformChannelContext* ctx = &g_LgcAlgoHandleContext;
    GAS_DETECT_RESULT packet = {0};

    fprintf(stderr, "result =========================count = %d \n", result->gas_list_size);
    // for(i = 0; i < result->count; i++)
    // {
    //     if(i >= MAX_GAS_NUM)
    //     {
    //         break;
    //     }
    //     memcpy(&(packet.stGasAttr[packet.u8GasNum].stGasRecth), &(result->gas_list[i].rect), sizeof(DETECT_RECT));
    //     packet.u8GasNum++;
    // }
    // if(ctx->m_GasResultCallBack)
    // {
    //     ctx->m_GasResultCallBack((void *)(&packet), sizeof(GAS_DETECT_RESULT), ctx->m_GasResultUserData);
    // }
    return 0;

}

static void* GasFrameSendThread(void* lpParameter)
{
    CPlatformChannelContext* ctx = &g_LgcAlgoHandleContext;
    DETECT_FRAME frame;
    ThreadInfo_t        *pParent = (ThreadInfo_t *)lpParameter;
    size_t bytes_read;
    unsigned char frame_buff[640 *512 *3];
    static uint64_t frame_id = 0;
    FILE *file = fopen("./output.test2.rgb", "rb");
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
    }
    fclose(file);
    return NULL;
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

    param.confidence = 0.3;
    param.model_path = "./yolov8_seg.rknn";

    ctx->m_Detect_handle = detect_init_handle_v2(param);
    if(ctx->m_Detect_handle == NULL)
    {
        SDK_ERR(" detect algo init is err \n");
        return -1;
    }

    Comm_CreateThread(&ctx->m_GasFrameSendThread, 0, GasFrameSendThread);

    Comm_CreateThread(&ctx->m_GasResultRecvThread, 0, GasResulRecvThread);

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
