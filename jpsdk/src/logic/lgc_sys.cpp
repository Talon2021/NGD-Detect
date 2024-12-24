#include <unistd.h>

#include "lgc_sys.h"
#include "meida_common.h"
#include "gpio_hal.h"

SystemInfo_t g_SystemInfo = { 0 };


static void* check_brightness_thread(void* arg)
{
    SystemInfo_t* pSysInfo = (SystemInfo_t*)arg;
    while (1)
    {
        if (0 != pSysInfo->m_bExitCheckBrightness)
        {
            break;
        }
        pthread_mutex_lock(&pSysInfo->m_CheckBrightnessLock);
        if(pSysInfo->m_AutoLightMode == 2)  //自动检测开启
        {

        }
        pthread_mutex_unlock(&pSysInfo->m_CheckBrightnessLock);
        usleep(100 *1000);
    }

    return NULL;
}

static void* check_temperature_thread(void* arg)
{
    SystemInfo_t* pSysInfo = (SystemInfo_t*)arg;
    while (1)
    {
        if (0 != pSysInfo->m_bExitTemperature)
        {
            break;
        }
        pthread_mutex_lock(&pSysInfo->m_CheckTemperatureLock);
        if(pSysInfo->m_AutoTemperatureMode == 2)  //自动检测开启
        {

        }
        pthread_mutex_unlock(&pSysInfo->m_CheckTemperatureLock);
        usleep(100 *1000);
    }

    return NULL;
}


void StartCheckBrightness()
{
    SystemInfo_t* pSysInfo = NULL;
    LGC_SYS_GetSystemInfo(&pSysInfo);
    pSysInfo->m_bExitCheckBrightness = 0;
    pthread_create(&pSysInfo->m_CheckBrightnessThread, NULL, check_brightness_thread, (void*)pSysInfo);
    return;
}


void StopCheckBrightness()
{
    SystemInfo_t* pSysInfo = NULL;
    LGC_SYS_GetSystemInfo(&pSysInfo);
    pSysInfo->m_bExitCheckBrightness = 1;
    pthread_join(pSysInfo->m_CheckBrightnessThread, NULL);
    return;
}

void StartCheckTemperature()
{
    SystemInfo_t* pSysInfo = NULL;
    LGC_SYS_GetSystemInfo(&pSysInfo);
    pSysInfo->m_bExitTemperature = 0;
    pthread_create(&pSysInfo->m_CheckTemperatureThread, NULL, check_temperature_thread, (void*)pSysInfo);
    return;
}

void StopCheckTemperature()
{
    SystemInfo_t* pSysInfo = NULL;
    LGC_SYS_GetSystemInfo(&pSysInfo);
    pSysInfo->m_bExitTemperature = 1;
    pthread_join(pSysInfo->m_CheckTemperatureThread, NULL);
    return;
}

int LGC_SYS_Init()
{
    SystemInfo_t* pSysInfo = &g_SystemInfo;
    memset(pSysInfo, 0, sizeof(SystemInfo_t));
    pthread_mutex_init(&pSysInfo->m_CheckBrightnessLock, NULL);
    pthread_mutex_init(&pSysInfo->m_CheckTemperatureLock, NULL);

    return 0;
}

void LGC_SYS_DeInit()
{
    SystemInfo_t* pSysInfo = &g_SystemInfo;
    pthread_mutex_destroy(&pSysInfo->m_CheckBrightnessLock);
    pthread_mutex_destroy(&pSysInfo->m_CheckTemperatureLock);
}

int LGC_SYS_GetSystemInfo(SystemInfo_t **pInfo)
{
    *pInfo = &g_SystemInfo;
    return 0;
}

int LGC_SYS_SetLightMode(int mode)
{
    SystemInfo_t* pSysInfo = NULL;
    if (HY_Res_SDK_Lock() != 0)
    {
        SDK_ERR("SDK Not Init\n");
        return -1;
    }
    LGC_SYS_GetSystemInfo(&pSysInfo);

    pthread_mutex_lock(&pSysInfo->m_CheckBrightnessLock);
    pSysInfo->m_AutoLightMode = mode;
    pthread_mutex_unlock(&pSysInfo->m_CheckBrightnessLock);

    HY_Res_SDK_UnLock();
    return 0;
}

int LGC_SYS_SetheatMode(int mode)
{
    SystemInfo_t* pSysInfo = NULL;
    if (HY_Res_SDK_Lock() != 0)
    {
        SDK_ERR("SDK Not Init\n");
        return -1;
    }
    LGC_SYS_GetSystemInfo(&pSysInfo);

    pthread_mutex_lock(&pSysInfo->m_CheckTemperatureLock);
    pSysInfo->m_AutoTemperatureMode = mode;
    pthread_mutex_unlock(&pSysInfo->m_CheckTemperatureLock);


    HY_Res_SDK_UnLock();
    return 0;
}

int LGC_SYS_SetLightEnable(int enable)
{
    SystemInfo_t* pSysInfo = NULL;
    if (HY_Res_SDK_Lock() != 0)
    {
        SDK_ERR("SDK Not Init\n");
        return -1;
    }
    LGC_SYS_GetSystemInfo(&pSysInfo);

    pthread_mutex_lock(&pSysInfo->m_CheckBrightnessLock);
    light_pin_status_set(enable);
    pthread_mutex_unlock(&pSysInfo->m_CheckBrightnessLock);


    HY_Res_SDK_UnLock();
    return 0;
}

int LGC_SYS_SetheatEnable(int enable)
{
     SystemInfo_t* pSysInfo = NULL;
    if (HY_Res_SDK_Lock() != 0)
    {
        SDK_ERR("SDK Not Init\n");
        return -1;
    }
    LGC_SYS_GetSystemInfo(&pSysInfo);

    pthread_mutex_lock(&pSysInfo->m_CheckTemperatureLock);
    heat_pin_status_set(enable);
    pthread_mutex_unlock(&pSysInfo->m_CheckTemperatureLock);


    HY_Res_SDK_UnLock();
    return 0;
}
