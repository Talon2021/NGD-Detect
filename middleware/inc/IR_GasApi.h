
#ifndef IR_GAS_API_H
#define IR_GAS_API_H

#include "IR_common.h"



#ifdef __cplusplus
extern "C"
{
#endif
/// @brief 系统初始化
/// @return 非0失败
int IR_Sys_Init();

/// @brief 反初始化
/// @return 非0失败
int IR_Sys_UnInit();

int IR_Sys_AlgoSendFrame(img_data *data);

int IR_Sys_RegisterResultCAllBack(DetectResult_CALLBACK cb);

int IR_Sys_SetIrCtrlCb(IRControlFunctions ir_cb);

int IR_Sys_SetVisCtrlCb(VisControlFunctions vis_cb);

#ifdef __cplusplus
}
#endif

#endif