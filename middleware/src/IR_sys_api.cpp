#include "IR_common.h"
#include "IR_GasApi.h"
#include "common.h"
#include "mpu_init.h"
#include "mpu_avl_api.h"
#include "CChannelManager.h"
#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include "sdk_log.h"
#include "CConfig.h"
#include "jpsdk.h"

int IR_Sys_Init()
{
    my_spd_log_init();
    int ret;
    ret = mpu_init();
    return ret;
}


int IR_Sys_UnInit()
{
    int ret;
    ret = mpu_uninit();
    return ret;
}

int IR_Sys_AlgoSendFrame(img_data *data)
{
    int ret ;
    ret = JPSys_PushStream(data);
    return ret;
}

int IR_Sys_RegisterResultCAllBack(DetectResult_CALLBACK cb)
{
    int ret ;
    ret = AVL_Alg_RegisterGasResultCb(0, cb);
    return ret;
}

int IR_Sys_SetIrCtrlCb(IRControlFunctions ir_cb)
{
    int ret ;
    ret = AVL_InfraredImage_RegisterIrCtrlCb(0, ir_cb);
    return ret;
}

int IR_Sys_SetVisCtrlCb(VisControlFunctions vis_cb)
{
    int ret ;
    ret = AVL_Vis_RegisterVisCtrlCb(0, vis_cb);
    return ret;
}
