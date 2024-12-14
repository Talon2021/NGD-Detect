#include "mpu_ciu_update.h"
#include "network.h"
#include "sdk_log.h"
#include "mpu_ciu_config.h"
#include "mpu_avl_api.h"
#include <string.h>
#include "common.h"
static int NETWORK_Update_cb(const char *param)
{
    DEBUG(" update is start \n");
    mysystem("sync");
    mysystem("reboot");
    return 0;
}

static int NETWORK_Data_Download(NetworkTransparentTransmission *data)
{
    int ret = 0;
    switch (data->type)
    {
    case NETWORK_TRANSPARENT_TRANSMISSION_PTZ:
       {
            ret = AVL_Ptz_DataTransm(0, data->in, strlen(data->in), data->out);
            break;
       }
    
    default:
        ERROR("download type is err \n");
        break;
    }
    return ret;
}
void CIU_RegisterUpdate()
{
    NetworkOperationRegister(NETWORK_OPERATION_UPGRADE, (void *)NETWORK_Update_cb);
    NetworkOperationRegister(NETWORK_OPERATION_TRANSPARENT_TRANSMISSION , (void *)NETWORK_Data_Download);
}

void CIU_UnRegisterUpdate()
{
    NetworkOperationRegister(NETWORK_OPERATION_UPGRADE, NULL);
    NetworkOperationRegister(NETWORK_OPERATION_TRANSPARENT_TRANSMISSION , NULL);
}


