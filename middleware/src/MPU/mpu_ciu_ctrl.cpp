#include "mpu_ciu_ctrl.h"
#include "common.h"
#include "network.h"
#include "mpu_avl_api.h"
#include "mpu_ciu_config.h"
#include "sdk_log.h"
#include <string.h>
#include <map>
#include "mpu_ciu_ptz.h"
static std::map<int, int> Ptz_ctrl_http = {
    {NETWORK_CONTORL_PTZ_CTRL_UP, JP_PTZ_CTRL_UP},
    {NETWORK_CONTORL_PTZ_CTRL_DOWN, JP_PTZ_CTRL_DOWN},
    {NETWORK_CONTORL_PTZ_CTRL_LEFT, JP_PTZ_CTRL_LEFT},
	{NETWORK_CONTORL_PTZ_CTRL_RIGHT, JP_PTZ_CTRL_RIGHT},
	{NETWORK_CONTORL_PTZ_CTRL_HOME, JP_PTZ_CTRL_HOME},
	{NETWORK_CONTORL_PTZ_CTRL_PRESET, JP_PTZ_CTRL_PRESET},
};
static int MPU_CIU_CRTL_Badpix(NetworkContorlBadPix *pcfg)
{
    int ret = 0;
    if(pcfg == NULL)
    {
        ERROR(" parma is err NULL\n");
        return -1;
    }
    return ret;
}

static int MPU_CIU_CRTL_trackObj(NetworkContorlTrackingObject *pcfg)
{
    int ret = 0;
    if(pcfg == NULL)
    {
        ERROR(" parma is err NULL\n");
        return -1;
    }
    std::vector<long> track_id;
    for(int i = 0; i < pcfg->tracking_num; i++)
    {
        track_id.emplace_back(pcfg->tracking_object[i]);
    }
    ret = AVL_Alg_SetDetectTrackId(0, track_id);
    return ret;
}

static int MPU_CIU_CRTL_ptz(NetworkContorlPtzCtrl *pcfg)
{
    int ret = 0;
    auto it = Ptz_ctrl_http.find(pcfg->mode);
    if((it == Ptz_ctrl_http.end()))
    {
        ERROR("param is err minorType = %d\n",pcfg->mode);
        return -1;
    }
    ret = MPU_PtzAllAction(JP_PTZ_CONTORL, it->second, (void *)pcfg->preset_name, NULL);
    return ret;
}

static int MPU_CIU_CRTL_infrared_electric(NetworkContorlIrFocusing *pcfg) // 红外电动调焦, 0停止，1左转，2右转
{
    if(pcfg == NULL)
    {
        ERROR("the parame is err NULL \n");
        return -1;
    }
    AVL_InfraredImage_SetElectricFocu(0, pcfg->mode);
    return 0;
}

static int MPU_CIU_CRTL_SnapPic(NetworkContorlSnap *pcfg)
{
    if(pcfg == NULL)
    {
        ERROR("the parame is err NULL \n");
        return -1;
    }
    return 0;
}

static int MPU_CIU_CRTL_Recode(NetworkContorlRecord *pcfg)
{
    if(pcfg == NULL)
    {
        ERROR("the parame is err NULL \n");
        return -1;
    }
    return 0;
}

static int MPU_CIU_CRTL_ShutterCalibration(NetworkContorlShutterCalibration *pcfg)
{
    if(pcfg == NULL)
    {
        ERROR("the parame is err NULL \n");
        return -1;
    }
    return 0;
}

int MPU_CIU_CTRL_MODLES(int type, void *buffer, int bufflen, void *reserve, int reserveLen)
{
    int ret = 0;
    int cam = *(int *)reserve;
    switch (type)
    {
        case CONTORL_SNAP:
        {
            if(bufflen < sizeof(NetworkContorlSnap))
            {
                ret = -1;
                ERROR("buffer is too small\n");
                break;
            }
            NetworkContorlSnap *pcfg = (NetworkContorlSnap *)buffer;
            ret = MPU_CIU_CRTL_SnapPic(pcfg);
            break;
        }
        case CONTORL_RECORD:
        {
            if(bufflen < sizeof(NetworkContorlRecord))
            {
                ret = -1;
                ERROR("buffer is too small\n");
                break;
            }
            NetworkContorlRecord *pcfg = (NetworkContorlRecord *)buffer;
            ret = MPU_CIU_CRTL_Recode(pcfg);
                
            break;
        }
        case CONTORL_BAD_PIX:
        {
            if(bufflen < sizeof(NetworkContorlBadPix))
            {
                ret = -1;
                ERROR("buffer is too small\n");
                break;
            }
            NetworkContorlBadPix *pcfg = (NetworkContorlBadPix *)buffer;
            ret = MPU_CIU_CRTL_Badpix(pcfg);
            break;
        }
        case CONTORL_TRACK:
        {
            if(bufflen < sizeof(NetworkContorlTrackingObject))
            {
                ret = -1;
                ERROR("buffer is too small\n");
                break;
            }
            NetworkContorlTrackingObject *pcfg = (NetworkContorlTrackingObject *)buffer;
            ret = MPU_CIU_CRTL_trackObj(pcfg);
            break;
        }
        case CONTORL_SHUTTER_CALIBRATION:
        {
            if(bufflen < sizeof(NetworkContorlShutterCalibration))
            {
                ret = -1;
                ERROR("buffer is too small\n");
                break;
            }
            NetworkContorlShutterCalibration *pcfg = (NetworkContorlShutterCalibration *)buffer;
            ret = MPU_CIU_CRTL_ShutterCalibration(pcfg);
            break;
        }
        case CONTORL_PTZ:
        {
            if(bufflen < sizeof(NetworkContorlPtzCtrl))
            {
                ret = -1;
                ERROR("buffer is too small\n");
                break;
            }
            NetworkContorlPtzCtrl *pcfg = (NetworkContorlPtzCtrl *)buffer;
            ret = MPU_CIU_CRTL_ptz(pcfg);
            break;
        }
        case CONTORL_INFRARED_ELECTRIC_FOCUS:
        {
            if(bufflen < sizeof(NetworkContorlIrFocusing))
            {
                ret = -1;
                ERROR("buffer is too small\n");
                break;
            }
            NetworkContorlIrFocusing *pcfg = (NetworkContorlIrFocusing *)buffer;
            ret = MPU_CIU_CRTL_infrared_electric(pcfg);
            break;
        }
        default:
            break;
    }
    return ret;
}

static int NETWORK_Contotl_cb(int type, int cam, void *buff, int buffLen)
{
    int ret = 0;
    int *reserve = &cam;
    switch (type)
    {
    case NETWORK_CONTORL_SNAP:
        ret = MPU_CIU_Set_ALL_ConfigGure(JP_NVR_CONTORL, CONTORL_SNAP, buff, buffLen, (void *)reserve, sizeof(int));
        break;
    case NETWORK_CONTORL_RECORD:
        ret = MPU_CIU_Set_ALL_ConfigGure(JP_NVR_CONTORL, CONTORL_RECORD, buff, buffLen, (void *)reserve, sizeof(int));
        break;
    case NETWORK_CONTORL_BAD_PIX:
        ret = MPU_CIU_Set_ALL_ConfigGure(JP_NVR_CONTORL, CONTORL_BAD_PIX, buff, buffLen, (void *)reserve, sizeof(int));
        break;
    case NETWORK_CONTORL_SHUTTER_CALIBRATION:
        ret = MPU_CIU_Set_ALL_ConfigGure(JP_NVR_CONTORL, CONTORL_SHUTTER_CALIBRATION, buff, buffLen, (void *)reserve, sizeof(int));
        break;
    case NETWORK_CONTORL_LASER_RANGING:
        ret = MPU_CIU_Set_ALL_ConfigGure(JP_NVR_CONTORL, CONTORL_RANG_DISTANCE, buff, buffLen, (void *)reserve, sizeof(int));
        break;
    case NETWORK_CONTORL_PTZ:
        ret = MPU_CIU_Set_ALL_ConfigGure(JP_NVR_CONTORL, CONTORL_PTZ, buff, buffLen, (void *)reserve, sizeof(int));
        break;
    case NETWORK_CONTORL_TRACKING:
        ret = MPU_CIU_Set_ALL_ConfigGure(JP_NVR_CONTORL, CONTORL_TRACK, buff, buffLen, (void *)reserve, sizeof(int));
        break;
    case NETWORK_CONTORL_IR_FOCUSING:
        ret = MPU_CIU_Set_ALL_ConfigGure(JP_NVR_CONTORL, CONTORL_INFRARED_ELECTRIC_FOCUS, buff, buffLen, (void *)reserve, sizeof(int));
        break;
    default:
        ERROR("control type is not support\n");
        ret = -1;
        break;
    }
    
    return ret;
}


void CIU_RegisterDevCrtl()
{
    
    NetworkOperationRegister(NETWORK_OPERATION_CONTORL_REQUEST, (void *)NETWORK_Contotl_cb);
}

void CIU_UnRegisterDevCrtl()
{
    NetworkOperationRegister(NETWORK_OPERATION_CONTORL_REQUEST, NULL);
}