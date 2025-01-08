#include "jpsdk.h"
#include <sys/time.h>
#include "ipc_timeout_task.h"
#include "meida_common.h"
#include "lgc.h"
#include "gpio_hal.h"
#include "lgc_algo.h"
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

int JPSys_SetheatEnable(int ch, int enbale)
{
    int ret; 
    ret = LGC_SYS_SetheatEnable(ch, enbale);
    return ret;
}

int JPSys_SetMcuPower(int enbale)
{
    int ret; 
    ret = mcu_pin_power_set(enbale);
    return ret;
}

int JPSys_SetIrCameraPower(int enbale)
{
    int ret; 
    ret = ir_pin_power_set(enbale);
    return ret;
}

int JPSys_SetVisCameraPower(int enbale)
{
   int ret; 
    ret = vis_pin_power_set(enbale);
    return ret;
}

int JPSys_SetPtzUartSwitchPower(int enbale)
{
    int ret; 
    ret = ptz_pin_power_set(enbale);
    return ret;
}

int JPSys_SetAlarmUartSwitchPower(int enbale)
{
    int ret; 
    ret = alarm_pin_power_set(enbale);
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

int JPSys_PushStream(void *stream)
{
    int ret = 0;
    ret = mpp_venc_push_stream(stream);
    return ret;

}

int JPSys_SetAutoTemperaCfg(TemperatureCfg tem_cfg)
{
    int ret = 0;
    ret = LGC_SYS_SetTemCtrlCfg(tem_cfg);
    return ret;
}
