#include "jpsdk.h"
#include <sys/time.h>
#include "ipc_timeout_task.h"
#include "meida_common.h"
#include "lgc.h"
#include "gpio_hal.h"
int JPSys_Init(int mode)
{
    int nRet;
    LGC_Init();
    ipc_timeout_task_init();
    //ipc_timeout_task_add(200, check_misc_status, NULL);
    return nRet;
}


int JPSys_DeInit()
{
    LGC_DeInit();
    ipc_timeout_task_deinit();
    return 0;
}

int JPSys_SetWiperEnable(int enbale)
{
    int ret = 0;
    ret = wiper_pin_status_set(enbale);
    return ret;
}

int JPSys_SetheatEnable(int enbale)
{
    int ret = 0;
    ret = LGC_SYS_SetheatEnable(enbale);
    return ret;
}

int JPSys_SetLightEnable(int enbale)
{
    int ret = 0;
    ret = LGC_SYS_SetLightEnable(enbale);
    return ret;
}

int JPSys_SetLightMode(int mode)
{
    int ret = 0;
    ret = LGC_SYS_SetLightMode(mode);
    return ret;
}

int JPSys_SetHeatMode(int mode)
{
    int ret = 0;
    ret = LGC_SYS_SetheatMode(mode);
    return ret;
}

int JPSys_RegisterGasDetectResultCb(GasDetectResult_CALLBACK cb, void *userdata)
{
    int ret = 0;
    ret = LGC_ALGO_RegisterGasResultCb(cb, userdata);
    return ret;
}
