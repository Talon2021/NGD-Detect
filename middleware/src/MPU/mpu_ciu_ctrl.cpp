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
    {NETWORK_PTZ_CTRL_UP, JP_PTZ_CTRL_UP},
    {NETWORK_PTZ_CTRL_DOWN, JP_PTZ_CTRL_DOWN},
    {NETWORK_PTZ_CTRL_LEFT, JP_PTZ_CTRL_LEFT},
	{NETWORK_PTZ_CTRL_RIGHT, JP_PTZ_CTRL_RIGHT},
	{NETWORK_PTZ_CTRL_HOME, JP_PTZ_CTRL_HOME},
	{NETWORK_PTZ_CTRL_PRESET, JP_PTZ_CTRL_PRESET},
};
static int MPU_CIU_CRTL_Badpix(int *opt, int *out)
{
    int ret = 0;
    if(opt == NULL)
    {
        ERROR(" parma is err NULL\n");
        return -1;
    }
    if(*opt == NETWORK_BAD_PIX_OPERATION_GET)
    {

    }
    else
    {
        ret = AVL_InfraredImage_ManualDefectRemoval(0, *opt, 1);
    }
    return ret;
}

static int MPU_CIU_CRTL_trackObj(NetworkTrackingObject *pcfg)
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

static int MPU_CIU_CRTL_ptz(NetworkPtzCtrl *pcfg)
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

static int MPU_CIU_CRTL_infrared_electric(int *pcfg) // 红外电动调焦, 0停止，1左转，2右转
{
    if(pcfg == NULL)
    {
        ERROR("the parame is err NULL \n");
        return -1;
    }

    return 0;
}

int MPU_CIU_CTRL_MODLES(int type, void *buffer, int bufflen)
{
    int ret = 0;
    NetworkContorl *pcfg = (NetworkContorl *)buffer;
    switch (type)
    {
        case NETWORK_CONTORL_SNAP:
        {
                
            break;
        }
        case CONTORL_RECORD:
        {
                
            break;
        }
        case CONTORL_BAD_PIX:
        {
            if(bufflen < sizeof(int))
            {
                ret = -1;
                ERROR("buffer is too small\n");
                break;
            }
            
            ret = MPU_CIU_CRTL_Badpix(&(pcfg->in.bad_pix_operation), &(pcfg->out.bad_pix_num));
            break;
        }
        case CONTORL_RANG_DISTANCE:
        {
            break;
        }
        case CONTORL_TRACK:
        {
            if(bufflen < sizeof(NetworkTrackingObject))
            {
                ret = -1;
                ERROR("buffer is too small\n");
                break;
            }
            ret = MPU_CIU_CRTL_trackObj(&(pcfg->in.tracking_object));
            break;
        }
        case CONTORL_SHUTTER_CALIBRATION:
        {
            break;
        }
        case CONTORL_SHUTTER_PTZ:
        {
            if(bufflen < sizeof(NetworkPtzCtrl))
            {
                ret = -1;
                ERROR("buffer is too small\n");
                break;
            }
            ret = MPU_CIU_CRTL_ptz(&(pcfg->in.ptz_ctrl));
            break;
        }
        case CONTORL_INFRARED_ELECTRIC_FOCUS:
        {
            if(bufflen < sizeof(NetworkPtzCtrl))
            {
                ret = -1;
                ERROR("buffer is too small\n");
                break;
            }
            ret = MPU_CIU_CRTL_infrared_electric(&(pcfg->in.infrared_focus_mode));
            break;
        }
        default:
            break;
    }
    return ret;
}

static int NETWORK_Contotl_cb(NetworkContorl *param)
{
    int ret = 0;
    switch (param->type)
    {
    case NETWORK_CONTORL_SNAP:
        ret = MPU_CIU_Set_ALL_ConfigGure(JP_NVR_CONTORL, CONTORL_SNAP, param, sizeof(NetworkContorl));
        break;
    case NETWORK_CONTORL_RECORD:
        ret = MPU_CIU_Set_ALL_ConfigGure(JP_NVR_CONTORL, CONTORL_RECORD, param, sizeof(NetworkContorl));
        break;
    case NETWORK_CONTORL_BAD_PIX:
        ret = MPU_CIU_Set_ALL_ConfigGure(JP_NVR_CONTORL, CONTORL_BAD_PIX, param, sizeof(NetworkContorl));
        break;
    case NETWORK_CONTORL_SHUTTER_CALIBRATION:
        ret = MPU_CIU_Set_ALL_ConfigGure(JP_NVR_CONTORL, CONTORL_SHUTTER_CALIBRATION, param, sizeof(NetworkContorl));
        break;
    case NETWORK_CONTORL_LASER_RANGING:
        ret = MPU_CIU_Set_ALL_ConfigGure(JP_NVR_CONTORL, CONTORL_RANG_DISTANCE, param, sizeof(NetworkContorl));
        break;
    case NETWORK_CONTORL_PTZ:
        ret = MPU_CIU_Set_ALL_ConfigGure(JP_NVR_CONTORL, CONTORL_SHUTTER_PTZ, param, sizeof(NetworkContorl));
        break;
    case NETWORK_CONTORL_TRACKING:
        ret = MPU_CIU_Set_ALL_ConfigGure(JP_NVR_CONTORL, CONTORL_TRACK, param, sizeof(NetworkContorl));
        break;
    case NETWORK_CONTORL_INFRARED_ELECTRIC_FOCUS:
        ret = MPU_CIU_Set_ALL_ConfigGure(JP_NVR_CONTORL, CONTORL_INFRARED_ELECTRIC_FOCUS, param, sizeof(NetworkContorl));
        break;
    default:
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