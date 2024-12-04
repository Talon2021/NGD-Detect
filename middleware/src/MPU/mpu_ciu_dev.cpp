#include "mpu_ciu_dev.h"
#include "network.h"
#include "common.h"
#include "sdk_log.h"
#include "mpu_ciu_config.h"
#include <string.h>
#include <sys/time.h>
#include "CConfig.h"
#include <unistd.h>
static int MPU_CIU_GET_DevAblity(NetworkSupportFunction *pcfg)
{
    if(pcfg == NULL)
    {
        ERROR(" parma is err NULL\n");
        return -1;
    }
    pcfg->system_function.language_support_mask = 0;
    pcfg->system_function.position_function_support = false;
    pcfg->system_function.ptz_function_support = true;
    pcfg->system_function.area_function_support = true;
    pcfg->system_function.onvif_protocol_support = true;
    pcfg->system_function.ipv6_protocol_support =false;
    pcfg->camera_chip_function.brightness_support = 99;
    pcfg->camera_chip_function.contrast_support = 99;
    pcfg->camera_chip_function.pseudo_color_support_mask = (1 << 0) | (1 << 1) | (1 << 2) | (1 << 3) | (1 << 4) | (1 << 5) | (1 << 6) | (1 << 7);
    pcfg->camera_chip_function.sharpening_support = 7;
    pcfg->camera_chip_function.central_enhance_support = false;
    pcfg->camera_chip_function.detail_enhance_support = 0;
    pcfg->camera_chip_function.electronic_zoom_support = 0;
    pcfg->camera_chip_function.target_recognition_support = false;
    pcfg->camera_chip_function.background_calibration_support = false;
    pcfg->camera_chip_function.hot_spot_tracking_support = true;
    pcfg->camera_chip_function.picture_in_picture_support_mask = 0;
    pcfg->camera_chip_function.outdoor_mode_support = false;
    pcfg->camera_chip_function.fusion_mode_support_mask = false;
    pcfg->camera_chip_function.shutter_calibration_support = false;
    pcfg->camera_chip_function.bad_pix_threshold_support.support_state = true;
    pcfg->camera_chip_function.bad_pix_threshold_support.min = 20;
    pcfg->camera_chip_function.bad_pix_threshold_support.max = 32;
    pcfg->camera_chip_function.inrared_auto_focus_support = true;
    pcfg->camera_chip_function.visible_light_auto_focus_support = true;
    pcfg->camera_chip_function.gas_enhanced_display_support = true;
    pcfg->calibration_gun_function.support_state = false;
    pcfg->calibration_gun_function.gun_type_support = false;
    pcfg->calibration_gun_function.graticule_style_support_mask = 0;
    pcfg->calibration_gun_function.color_support_mask = 0;
    pcfg->other_function.defog_by_heat_support = true;
    pcfg->other_function.heat_support = true;
    pcfg->other_function.fan_support = true;
    pcfg->other_function.electronic_compass_support = false;
    pcfg->other_function.red_dot_support = false;
    pcfg->other_function.hdmi_support = false;
    pcfg->other_function.wipers_support = true;
    pcfg->other_function.fill_light_support = true;
    pcfg->other_function.cvbs_support = true;
    pcfg->algorithm_function.object_detection_support = true;
    pcfg->algorithm_function.tracking_support = true;
    pcfg->algorithm_function.action_analyze_support = false;
    pcfg->algorithm_function.splicing_support = true;
    pcfg->algorithm_function.fire_point_support = false;
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
    char *softVersion = "1.0.0-00-240914";
    char *hardVersion = "1.0.0-00-240914";
    char *serialNumbae = "123456";
    struct timeval curTime = {0};
    char addr[32] = {0};
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
#if 0
        CConfig *p_config = new CConfig();

        p_config->LoadFile("/oem/app/edvr/DeviceVersion.ini");
        snprintf(pcfg->out.device_info.soft_version, sizeof(pcfg->out.device_info.soft_version),"%d.%d.%d-%02d-%d",
                            p_config->GetValue("Software", "Major_version", (long)1),
                            p_config->GetValue("Software", "Minor_version", (long)0),
                            p_config->GetValue("Software", "Reversion", (long)0),
                            p_config->GetValue("Software", "Province", (long)0),
                            p_config->GetValue("Software", "BuildDate", (long)0));
        
        snprintf(pcfg->out.device_info.hard_version, sizeof(pcfg->out.device_info.hard_version),"%d.%d.%d-%02d-%d",
                            p_config->GetValue("DSPSoftware", "Major_version", (long)1),
                            p_config->GetValue("DSPSoftware", "Minor_version", (long)0),
                            p_config->GetValue("DSPSoftware", "Reversion", (long)0),
                            p_config->GetValue("DSPSoftware", "Province", (long)0),
                            p_config->GetValue("DSPSoftware", "BuildDate", (long)0));
       
        memcpy(pcfg->out.device_info.serial_number, serialNumbae, sizeof(pcfg->out.device_info.serial_number));
        delete p_config;
#endif
        
        break;
    case NETWORK_SYSTEM_SET_TIME:
        break;
    case NETWORK_SYSTEM_GET_RTSP_URL:
    //rtsp://192.168.110.50/live/main_stream

        get_localip("eth0", addr);
        snprintf(pcfg->out.rtsp_url, sizeof(pcfg->out.rtsp_url), "rtsp://%s/live/main_stream",addr);
        //底层获取
        break;
    default:
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
    char *softVersion = "1.0.0-00-240914";
    char *hardVersion = "1.0.0-00-240914";
    char *serialNumbae = "123456";
    struct timeval curTime = {0};
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
        break;
    case NETWORK_SYSTEM_SET_TIME:
        curTime.tv_sec = pcfg->in.time;
        if(settimeofday(&curTime, NULL))
        {
            ERROR("set time is err\n");
            ret = -1;
        }
        break;
    case NETWORK_SYSTEM_GET_RTSP_URL:
        //底层获取
        break;
    default:
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
            if(bufflen < sizeof(NetworkSupportFunction))
            {
                ret = -1;
                ERROR("buffer is too small\n");
                break;
            }
            fprintf(stderr, "xh test param = %p line = %d \n", buff, __LINE__);
            NetworkSupportFunction *pcfg = (NetworkSupportFunction *)buff;
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
            if(bufflen < sizeof(NetworkSupportFunction))
            {
                ret = -1;
                ERROR("buffer is too small\n");
                break;
            }
            NetworkSupportFunction *pcfg = (NetworkSupportFunction *)buff;
            //ret = MPU_CIU_GET_DevAblity(pcfg);
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


static int NETWORK_Ability_cb(NetworkSupportFunction *param)
{
    int ret = 0;
    
    ret = MPU_CIU_Get_ALL_ConfigGure(JP_NVR_GET_DEVICEINFO, DEVICE_ALL_ABLITY, param, sizeof(NetworkSupportFunction));
    return ret;
}

static int NETWORK_System_cb(NetworkSystem *param)
{
    int ret = 0;
    DEBUG("xh test pro\n");
    switch (param->type)
    {
    case NETWORK_SYSTEM_DEVICE_INFO:
    case NETWORK_SYSTEM_GET_RTSP_URL:
    {
         ret = MPU_CIU_Get_ALL_ConfigGure(JP_NVR_GET_DEVICEINFO, DEVICE_SYSTEM_INFO, param, sizeof(NetworkSystem));
        break;
    }
    case NETWORK_SYSTEM_REBOOT:
    case NETWORK_SYSTEM_RESET:
    case NETWORK_SYSTEM_FORMAT:
    case NETWORK_SYSTEM_SET_TIME:
    {
        ret = MPU_CIU_Set_ALL_ConfigGure(JP_NVR_SET_DEVICEINFO, DEVICE_SYSTEM_INFO, param, sizeof(NetworkSystem));
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
