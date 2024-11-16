
#include "mpu_ciu_ptz.h"
//#include "onvif_common.h"
#include "mpu_avl_api.h"
#include "sdk_log.h"
#include "mpu_ciu_config.h"


static int MPU_PtzCtrlNormal(int type, void *buf1, void * reserve)
{
    double yaw = 0;
    double pitch = 0;;
    int ret = 0;
    int mode;
    AVL_Ptz_GetPtzAngle(0, &yaw, &pitch);
    AVL_Ptz_GetScanMode(0, &mode);
    if(mode != 2 && mode != 3)
    {
        AVL_Ptz_ZeroInit(0);
        AVL_Ptz_SetPtzEnable(0,1);
        AVL_Ptz_SetScanMode(0, 2);
    }
    printf("xh test ===============type = %d \n", type);
    switch (type)
    {
    case JP_PTZ_CTRL_UP:
        {
            pitch+=5;
            ret = AVL_Ptz_SetPtzAngle(0, yaw, pitch);
            break;
        }
     case JP_PTZ_CTRL_DOWN:
        {
            pitch-=5;
            ret = AVL_Ptz_SetPtzAngle(0, yaw, pitch);
            break;
        }
     case JP_PTZ_CTRL_LEFT:
        {
            yaw-=10;
            // if(yaw < 0)
            // {
            //     yaw = yaw + 360;
            // }
            //ret = AVL_Ptz_SetPtzAngle(0, yaw, pitch);
            target_data data = {0};
            data.mode = 0;
            data.target_angle = yaw;
            DEBUG("data.target_angle = %lf\n",data.target_angle);
            ret |= AVL_Ptz_SetTargetLocation(0, data);
            break;
        }
     case JP_PTZ_CTRL_RIGHT:
        {
            yaw+=10;
            // if(yaw > 360)
            // {
            //     yaw = yaw - 360;
            // }
            target_data data = {0};
            data.mode = 0;
            data.target_angle = yaw;
            DEBUG("data.target_angle = %lf\n",data.target_angle);
            ret |= AVL_Ptz_SetTargetLocation(0, data);
            //ret = AVL_Ptz_SetPtzAngle(0, yaw, pitch);
            break;
        }
     case JP_PTZ_CTRL_HOME:
        {
            yaw = 0;
            target_data data = {0};
            data.mode = 0;
            data.target_angle = yaw;
            ret = AVL_Ptz_SetTargetLocation(0, data);
            break;
        }
    case JP_PTZ_CTRL_PRESET:
        {
            char *pre_name = (char *)buf1;

            break;
        }
        
    default:
        ERROR("the type is err \n");
        break;
    }
    return ret;
}


int MPU_PtzAllAction(int majorType, int minorType, void *buf1, void *reserve)
{
    int ret = 0;
    switch (majorType)
    {
    case JP_PTZ_CONTORL:
        {
            ret = MPU_PtzCtrlNormal(minorType, buf1, reserve);
            break;
        }
    default:
        {
            ERROR("default is err\n");
            break;
        }
    }
    return ret;
}