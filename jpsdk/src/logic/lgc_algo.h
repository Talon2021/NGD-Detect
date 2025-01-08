#ifndef _LGC_ALGO_H
#define _LGC_ALGO_H


#include <pthread.h>

#include "meida_common.h"
#include "jpsdk.h"
#include "mpp_venc.h"
#ifdef __cplusplus
extern "C" {
#endif

typedef enum
{
	ALARM_EVENT,
}event_type;

typedef enum 
{
    ALARM_TYPE_OVER_BOUNDARY,            // 越界  
    ALARM_TYPE_AREA_INTRUDE,             // 区域入侵
    ALARM_TYPE_ABNORMAL_ACTION,          // 异常行为
    ALARM_TYPE_FIRE_POINT_DETECTION,     // 火点检测
    ALARM_TYPE_TEMPERATURE_ABNORMAL,     // 温度异常
    ALARM_TYPE_VOLTAGE_ABNORMAL,         // 电压异常
    ALARM_TYPE_AMPERE_ABNORMAL,          // 电流异常
    ALARM_TYPE_GAS_LEAKAGE,          // 气体泄漏
}alarm_type;
typedef struct _alarm_data_st
{
	int vaild;
	int type;
	unsigned int timeTamp;
}alarm_data;


typedef struct PlatformChannelContext_S{
    
    ThreadInfo_t m_GasFrameSendThread;
    ThreadInfo_t m_GasResultRecvThread;
    void *m_Detect_handle;
    GasDetectResult_CALLBACK m_GasResultCallBack;
    void *m_GasResultUserData;
    int m_AlgoPicWidth;
    int m_AlgoPicHeight;

    AlarmEventCfg m_GasAlarmCfg;

    AlarmEvent_CALLBACK m_AlarmEventCb;
    void *m_AlarmUserData;
}CPlatformChannelContext;

int LGC_ALGO_Init(void);

int LGC_ALGO_DeInit(void);

int LGC_ALGO_RegisterGasResultCb(GasDetectResult_CALLBACK callback, void *userdata);

int LGC_ALGO_SetGasEventCfg(AlarmEventCfg cfg);

int LGC_ALGO_RegisterAlarmCb(AlarmEvent_CALLBACK callback, void *userdata);

#ifdef __cplusplus
} /* extern "C" */
#endif

#endif