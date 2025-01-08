#ifndef __LGC_SYS_H__
#define __LGC_SYS_H__

#include <pthread.h>
#include "jpsdk.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef struct{
    pthread_t m_CheckBrightnessThread;      //亮度检测线程，控制用来控制白光灯
    volatile int m_bExitCheckBrightness;    
    int m_AutoLightMode;                    // 1 手动 ，2 通过光敏自动控制
    pthread_mutex_t m_CheckBrightnessLock;

    pthread_t m_CheckTemperatureThread;      //温度检测线程，用来控制自动加热开关
    volatile int m_bExitTemperature;    
    int m_AutoTemperatureMode;              // 1手动 ， 2 自动加热
    pthread_mutex_t m_CheckTemperatureLock;
    TemperatureCfg m_autoTemCtrlCfg;
    int m_TemCtrlFlag;              //加热开关标志

}SystemInfo_t;

/**
 * @brief 系统信息管理初始化
 *
 * @return int
 */
int LGC_SYS_Init();

/**
 * @brief 系统信息管理反初始化
 *
 */
void LGC_SYS_DeInit();

/**
 * @brief 获取系统信息句柄
 *
 * @param pInfo
 * @return int
 */
int LGC_SYS_GetSystemInfo(SystemInfo_t** pInfo);

int LGC_SYS_SetLightMode(int mode);

int LGC_SYS_SetheatMode(int mode);


int LGC_SYS_SetLightEnable(int enable);

int LGC_SYS_SetheatEnable(int ch, int enable);

int LGC_SYS_SetTemCtrlCfg(TemperatureCfg cfg);


#ifdef __cplusplus
} /* extern "C" */
#endif

#endif