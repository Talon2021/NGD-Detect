#include "mpu_ciu_dev.h"
#include "network.h"
#include "common.h"
#include "sdk_log.h"
#include "mpu_ciu_config.h"
#include <string.h>
#include <sys/time.h>
#include "CConfig.h"
#include <unistd.h>
#include "mpu_avl_api.h"
static int MPU_CIU_GET_DevAblity(NetworkAbilitySupportFunction *pcfg)
{
    int i = 0;
    if(pcfg == NULL)
    {
        ERROR(" parma is err NULL\n");
        return -1;
    }
    /* IR*/
    memset(pcfg, 0, sizeof(NetworkAbilitySupportFunction));
    pcfg->camera_ir_funcs.num = 1;
    for(i = 0; i < pcfg->camera_ir_funcs.num; i++)
    {
        pcfg->camera_ir_funcs.camera_ir[i].support_state = 1;
        pcfg->camera_ir_funcs.camera_ir[i].camera_ir_image_info.brightness_support = 10;
        pcfg->camera_ir_funcs.camera_ir[i].camera_ir_image_info.contrast_support = 10;
        pcfg->camera_ir_funcs.camera_ir[i].camera_ir_image_info.pseudo_color_support_mask = (1 << NETWORK_PSEUDO_COLOR_WHITE_HOT)\
                                                                                           |(1 << NETWORK_PSEUDO_COLOR_BLACK_HOT)\
                                                                                           |(1 << NETWORK_PSEUDO_COLOR_IRON_RED)\
                                                                                           |(1 << NETWORK_PSEUDO_COLOR_IRONBOW_REVERSE)\
                                                                                           |(1 << NETWORK_PSEUDO_COLOR_LAVA_FORWARD)\
                                                                                           |(1 << NETWORK_PSEUDO_COLOR_LAVA_REVERSE)\
                                                                                           |(1 << NETWORK_PSEUDO_COLOR_RAINBOW)\
                                                                                           |(1 << NETWORK_PSEUDO_COLOR_RAINBOW_REVERSE)\
                                                                                           |(1 << NETWORK_PSEUDO_COLOR_RAINBOWHC_FORWARD)\
                                                                                           |(1 << NETWORK_PSEUDO_COLOR_RAINBOWHC_REVERSE);
        pcfg->camera_ir_funcs.camera_ir[i].camera_ir_image_info.sharpening_support = 10;
        pcfg->camera_ir_funcs.camera_ir[i].camera_ir_image_info.saturation_support = 10;
        pcfg->camera_ir_funcs.camera_ir[i].camera_ir_image_enhance.central_enhance_support = 0;
        pcfg->camera_ir_funcs.camera_ir[i].camera_ir_image_enhance.detail_enhance_support = 0;
        pcfg->camera_ir_funcs.camera_ir[i].camera_ir_calibration.background_calibration_support = 0;
        pcfg->camera_ir_funcs.camera_ir[i].camera_ir_calibration.shutter_calibration_support = 0;
        pcfg->camera_ir_funcs.camera_ir[i].camera_ir_calibration.bad_pix_threshold_support.support_state = 0;
        pcfg->camera_ir_funcs.camera_ir[i].camera_ir_image_zoom.electronic_zoom_support = 4;
        pcfg->camera_ir_funcs.camera_ir[i].camera_ir_focusing.focusing_mode_support_mask = (1 << NETWORK_FOCUSING_MODE_AUTO)\
                                                                                          |(1 << NETWORK_FOCUSING_MODE_ELECTRIC);
    }
    /*VIS*/
    pcfg->camera_vis_funcs.num = 1;
    for(i = 0; i < pcfg->camera_vis_funcs.num; i++)
    {
        pcfg->camera_vis_funcs.camera_vis[i].camera_vis_image_info.brightness_support = 10;
        pcfg->camera_vis_funcs.camera_vis[i].camera_vis_image_info.contrast_support = 10;
        pcfg->camera_vis_funcs.camera_vis[i].camera_vis_image_info.sharpening_support = 10;
        pcfg->camera_vis_funcs.camera_vis[i].camera_vis_image_info.saturation_support = 10;
        pcfg->camera_vis_funcs.camera_vis[i].camera_vis_image_zoom.electronic_zoom_support = 4;
        pcfg->camera_vis_funcs.camera_vis[i].camera_vis_focusing.focusing_mode_support_mask = (1 << NETWORK_FOCUSING_MODE_AUTO);
    }
    pcfg->ptz_func.preset_support = 1;
    pcfg->algorithm_func.algorithm_detection.gas_detection_support = 1;
    pcfg->display_screen_func.display_info.language_support_mask = (1 << NETWORK_LANGUAGE_CHINESE);
    pcfg->other_func.preview.cvbs_support = 1;
    pcfg->other_func.other_info.wipers_support = 1;
    pcfg->other_func.other_info.fill_light_support = 1;
   
    return 0;

}

static int MPU_CIU_GET_DevSystem(NetworkSystem *pcfg)
{
    int ret = 0;
    if(pcfg == NULL)
    {
        ERROR(" parma is err NULL\n");
        return -1;
    }
   
    struct timeval curTime = {0};
    char addr[32] = {0};
    DevInfo_st info;
    AVL_Ext_GetDevVersionInfo(0, &info);
    switch (pcfg->type)
    {
    case NETWORK_SYSTEM_REBOOT:
    case NETWORK_SYSTEM_RESET:
    case NETWORK_SYSTEM_FORMAT:
        ret = -1;
        break;
    case NETWORK_SYSTEM_DEVICE_INFO:
        memcpy(pcfg->out.device_info.soft_version, info.sort_version, sizeof(pcfg->out.device_info.soft_version));
        memcpy(pcfg->out.device_info.hard_version, info.hart_version, sizeof(pcfg->out.device_info.hard_version));
        memcpy(pcfg->out.device_info.serial_number, info.serial_number, sizeof(pcfg->out.device_info.serial_number));
        break;
    case NETWORK_SYSTEM_SET_TIME:
        ret = -1;
        break;
    case NETWORK_SYSTEM_GET_RTSP_URL:
        if(pcfg->cam == 0)
        {
            memcpy(pcfg->out.rtsp_url.main_stream, info.ir_rtsp_url[0], sizeof(pcfg->out.rtsp_url.main_stream));
            memcpy(pcfg->out.rtsp_url.sub_stream, info.ir_rtsp_url[1], sizeof(pcfg->out.rtsp_url.sub_stream));
            memcpy(pcfg->out.rtsp_url.third_stream, info.ir_rtsp_url[2], sizeof(pcfg->out.rtsp_url.third_stream));
        }
        else if(pcfg->cam == 1)
        {
            memcpy(pcfg->out.rtsp_url.main_stream, info.vis_rtsp_url[0], sizeof(pcfg->out.rtsp_url.main_stream));
            memcpy(pcfg->out.rtsp_url.sub_stream, info.vis_rtsp_url[1], sizeof(pcfg->out.rtsp_url.sub_stream));
            memcpy(pcfg->out.rtsp_url.third_stream, info.vis_rtsp_url[2], sizeof(pcfg->out.rtsp_url.third_stream));
        }
        break;
    default:
        ERROR("the type is not support \n");
        ret = -1;
        break;
    }
    return ret;
}

static int MPU_CIU_SET_DevSystem(NetworkSystem *pcfg)
{
    int ret = 0;
    if(pcfg == NULL)
    {
        ERROR(" parma is err NULL\n");
        return -1;
    }
    switch (pcfg->type)
    {
    case NETWORK_SYSTEM_REBOOT:
        mysystem("sync");
        mysystem("reboot");
        break;
    case NETWORK_SYSTEM_RESET:
        mysystem("rm /etc/config.ini");
        mysystem("sync");
        mysystem("reboot");
        break;
    case NETWORK_SYSTEM_FORMAT:
        mysystem("rm -rf /userdata/");
        mysystem("sync");
        break;
    case NETWORK_SYSTEM_DEVICE_INFO:
        ret = -1;
        break;
    case NETWORK_SYSTEM_SET_TIME:
        AVL_Coder_SetTime(0, pcfg->in.time);
        break;
    case NETWORK_SYSTEM_GET_RTSP_URL:
        ret = -1; 
        break;
    default:
        ERROR("the type is not subport\n");
        break;
    }
    return ret;
}


int MPU_CIU_GET_DevConfig(unsigned int type, void *buff, int bufflen)
{
    int ret = 0;
    switch (type)
    {
    case DEVICE_ALL_ABLITY:
        {
            if(bufflen < sizeof(NetworkAbilitySupportFunction))
            {
                ret = -1;
                ERROR("buffer is too small\n");
                break;
            }
            NetworkAbilitySupportFunction *pcfg = (NetworkAbilitySupportFunction *)buff;
            ret = MPU_CIU_GET_DevAblity(pcfg);
            break;
        }
    case DEVICE_SYSTEM_INFO:
        {
            if(bufflen < sizeof(NetworkSystem))
            {
                ret = -1;
                ERROR("buffer is too small\n");
                break;
            }
            NetworkSystem *pcfg = (NetworkSystem *)buff;
            ret = MPU_CIU_GET_DevSystem(pcfg);
            break;
        }
    default:
        break;
    }
    return ret;
}

int MPU_CIU_SET_DevConfig(unsigned int type, void *buff, int bufflen)
{
    int ret = 0;
    switch (type)
    {
    case DEVICE_ALL_ABLITY:
        {
            ret = -1;
            break;
        }
    case DEVICE_SYSTEM_INFO:
        {
            if(bufflen < sizeof(NetworkSystem))
            {
                ret = -1;
                ERROR("buffer is too small\n");
                break;
            }
            NetworkSystem *pcfg = (NetworkSystem *)buff;
            ret = MPU_CIU_SET_DevSystem(pcfg);
            break;
        }
    default:
        break;
    }
    return ret;
}


static int NETWORK_Ability_cb(NetworkAbilitySupportFunction *param)
{
    int ret = 0;
    
    ret = MPU_CIU_Get_ALL_ConfigGure(JP_NVR_GET_DEVICEINFO, DEVICE_ALL_ABLITY, param, sizeof(NetworkAbilitySupportFunction), NULL, 0);
    return ret;
}

static int NETWORK_System_cb(NetworkSystem *param)
{
    int ret = 0;
    switch (param->type)
    {
    case NETWORK_SYSTEM_DEVICE_INFO:
    case NETWORK_SYSTEM_GET_RTSP_URL:
    {
        ret = MPU_CIU_Get_ALL_ConfigGure(JP_NVR_GET_DEVICEINFO, DEVICE_SYSTEM_INFO, param, sizeof(NetworkSystem), NULL, 0);
        break;
    }
    case NETWORK_SYSTEM_REBOOT:
    case NETWORK_SYSTEM_RESET:
    case NETWORK_SYSTEM_FORMAT:
    case NETWORK_SYSTEM_SET_TIME:
    {
        ret = MPU_CIU_Set_ALL_ConfigGure(JP_NVR_SET_DEVICEINFO, DEVICE_SYSTEM_INFO, param, sizeof(NetworkSystem), NULL, 0);
        break;
    }
    
    default:
        break;
    }
    
    return ret;
}

void CIU_RegisterDevInfo()
{
    NetworkOperationRegister(NETWORK_OPERATION_GET_ABILITY, (void *)NETWORK_Ability_cb);
    
    NetworkOperationRegister(NETWORK_OPERATION_SYSTEM_REQUEST, (void *)NETWORK_System_cb);
}

void CIU_UnRegisterDevInfo()
{
   
    NetworkOperationRegister(NETWORK_OPERATION_GET_ABILITY, NULL);
   
    NetworkOperationRegister(NETWORK_OPERATION_SYSTEM_REQUEST, NULL);
}
