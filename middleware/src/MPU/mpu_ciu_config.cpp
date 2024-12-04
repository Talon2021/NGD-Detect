#include "network.h"
//#include "onvif.h"
#include <map>
#include <string>
#include "sdk_log.h"
#include "common.h"
#include <string.h>
#include <unistd.h>
#include "CConfig.h"
#include "jpsdk.h"
#include "mpu_ciu_ptz.h"
#include "mpu_ciu_ctrl.h"
#include "mpu_ciu_dev.h"
#include "mpu_ciu_preview.h"
#include "mpu_ciu_update.h"
#include "mpu_ciu_config.h"
#include "mpu_avl_api.h"
#include "mpu_net_api.h"

static int report_peripheralCalback(peripheral_info_st info)
{
    NetworkUpload data;
    data.type = NETWORK_UPLOAD_PERIPHERAL;
    data.data.peripherail_info.yaw = info.yaw;
    data.data.peripherail_info.pitch = info.pitch;
    data.data.peripherail_info.motor_temperature = info.temp;
    data.data.peripherail_info.input_voltage = info.vol;
    data.data.peripherail_info.working_ampere = info.ampere;
    data.data.peripherail_info.longitude = info.longitude;
    data.data.peripherail_info.latitude = info.latitude;
#ifdef HTTP_UPLOAD
	NetworkUploadInfo(&data);
#endif
	return 0;
}

static int report_alarm_Calback(alarm_data *info)
{
    NetworkUpload data;
    data.type = NETWORK_UPLOAD_ALARM;
    data.data.alarm_info.type = info->type;
    data.data.alarm_info.state = NETWORK_ALARM_STATE_TRIGGER;
    data.data.alarm_info.time = info->timeTamp;
#ifdef HTTP_UPLOAD
	NetworkUploadInfo(&data);
#endif
	return 0;
}



static int MPU_CIU_SET_ConfigSystem(NetworkSystemInfo *pcfg)
{
    int ret = 0;
    time_cfg cfg;
    if(pcfg == NULL)
    {
        ERROR(" parma is err NULL\n");
        return -1;
    }
    char sign;
    int hour = 0;
    int min = 0;
    int result;
    result = sscanf(pcfg->time_zone, "UTC%c%02d:%02d", &sign, &hour, &min);
    if(result != 3)
    {
        ERROR("parma is err\n");
        return -1;
    }
    if(sign == '-')
        cfg.TimeZone = -hour * 60 - min;
    else
        cfg.TimeZone = hour * 60 + min;
    cfg.TimeFormat = pcfg->time_format;
    cfg.SummerTimeEnable = pcfg->summer_time_enable;
    ret |= AVL_Coder_SetLanguage(0, pcfg->language);
    ret |= AVL_Coder_SetTimeConfig(0, cfg);
    return ret;
}

static int MPU_CIU_GET_ConfigSystem(NetworkSystemInfo *pcfg)
{
    int ret = 0;
    time_cfg cfg;
    if(pcfg == NULL)
    {
        ERROR(" parma is err NULL\n");
        return -1;
    }
    AVL_Coder_GetLanguage(0, pcfg->language);
    AVL_Coder_GetTimeConfig(0, &cfg);
    if(cfg.TimeZone  > 0)
        snprintf(pcfg->time_zone, sizeof(pcfg->time_zone), "UTC+%02d:%02d", (cfg.TimeZone / 60), (cfg.TimeZone % 60));
    else
        snprintf(pcfg->time_zone, sizeof(pcfg->time_zone), "UTC-%02d:%02d", (cfg.TimeZone / 60), (cfg.TimeZone % 60));
    pcfg->time_format = cfg.TimeFormat;
    pcfg->summer_time_enable = (bool)cfg.SummerTimeEnable;
    return ret;
}

static int MPU_CIU_SET_ConfigCamear(NetworkCameraChipInfo *pcfg)
{
    int ret = 0;
    if(pcfg == NULL)
    {
        ERROR(" parma is err NULL\n");
        return -1;
    }
    ret |= AVL_InfraredImage_SetImageBrightness(0, pcfg->brightness);
    ret |= AVL_InfraredImage_SetImageContrast(0, pcfg->contrast);
    ret |= AVL_InfraredImage_SetInfraredImagePolarity(0, pcfg->pseudo_color);
    ret |= AVL_InfraredImage_SetInfraredImageSharpening(0, pcfg->sharpening);
    ret |= AVL_InfraredImage_SetHotspotTracking(0, pcfg->hot_spot_tracking);
    ret |= AVL_InfraredImage_ManualDefectRemoval(0, 0, pcfg->bad_pix_threshold);
    ret |= AVL_InfraredImage_SetInfraredImageElectronicZoom(0, pcfg->electronic_zoom);
    ret |= AVL_InfraredImage_SetInfraredImageAutoFocus(0, pcfg->inrared_auto_focus);
    ret |= AVL_InfraredImage_SetGasEnhanced(0, pcfg->gas_enhanced_display);
    return ret;
}

static int MPU_CIU_GET_ConfigCamear(NetworkCameraChipInfo *pcfg)
{
    if(pcfg == NULL)
    {
        ERROR(" parma is err NULL\n");
        return -1;
    }
    int opt = 0;
    AVL_InfraredImage_GetImageBrightness(0, &pcfg->brightness);
    AVL_InfraredImage_GetImageContrast(0, &pcfg->contrast);
    AVL_InfraredImage_GetInfraredImagePolarity(0, &pcfg->pseudo_color);
    AVL_InfraredImage_GetInfraredImageSharpening(0, &pcfg->sharpening);
    AVL_InfraredImage_GetHotspotTracking(0, (int *)(&pcfg->hot_spot_tracking));
    AVL_InfraredImage_GetManualDefectRemoval(0, &opt, &pcfg->bad_pix_threshold);
    AVL_InfraredImage_GetInfraredImageElectronicZoom(0, &pcfg->electronic_zoom);
    AVL_InfraredImage_GetInfraredImageAutoFocus(0, &pcfg->inrared_auto_focus);
    ///AVL_InfraredImage_GetAutoVisibleLight(0, &pcfg->visible_light_auto_focus);
    AVL_InfraredImage_GetGasEnhanced(0, &pcfg->gas_enhanced_display);
    return 0;
}

static int MPU_CIU_SET_Ptzinfo(NetworkPtzInfo *pcfg)
{
    int ret = 0;
    target_data data;
    int mode = 0;
    int enable = 0;
    if(pcfg == NULL)
    {
        ERROR(" parma is err NULL\n");
        return -1;
    }
    data.mode = pcfg->constant_scan.deflection_mode;
    data.angle_x = pcfg->constant_scan.pix.x;
    data.angle_y = pcfg->constant_scan.pix.y;
    data.target_angle = pcfg->constant_scan.yaw;

    if(pcfg->motor_enable == 0)
    {
        AVL_Ptz_SetPtzEnable(0, pcfg->motor_enable);
        AVL_Ptz_ZeroInit(0);
        return 0;
    }

    AVL_Ptz_GetPtzEnable(0, &enable);

    if(pcfg->motor_enable == 1 && enable == 0)
    {
        ret |= AVL_Ptz_SetPtzEnable(0, (int)(pcfg->motor_enable));
        ret |= AVL_Ptz_SetScanMode(0, pcfg->scan_mode);
    }
    else
    {
        AVL_Ptz_GetScanMode(0, &mode);
        if(mode != pcfg->scan_mode)
        {
            AVL_Ptz_ZeroInit(0);    //切换模式，直接归0
            ret |= AVL_Ptz_SetPtzEnable(0, (int)(pcfg->motor_enable));
            ret |= AVL_Ptz_SetScanMode(0, pcfg->scan_mode);
        }
        else
        {
            ret |= AVL_Ptz_SetPtzEnable(0,(int)(pcfg->motor_enable)); //打开
        }
    }
    //ret |= AVL_Ptz_SetPtzAngle(0, 0, pcfg->pitch);
    ret |= AVL_Ptz_SetStep(0, pcfg->step);
    if(pcfg->scan_mode == 1)
        ret |= AVL_Ptz_SetFanScanAngle(0, pcfg->fan_scanning.start_angle, pcfg->fan_scanning.end_angle);
    if(pcfg->scan_mode == 2 || pcfg->scan_mode == 3)
        ret |= AVL_Ptz_SetTargetLocation(0, data);
    
    return ret;
}

static int MPU_CIU_GET_Ptzinfo(NetworkPtzInfo *pcfg)
{
    int ret = 0;
    if(pcfg == NULL)
    {
        ERROR(" parma is err NULL\n");
        return -1;
    }
    double yaw = 0;
    pcfg->constant_scan.deflection_mode = NETWORK_CONSTAN_SCAN_DEFLECTION_ANGLE;
    AVL_Ptz_GetPtzEnable(0, (int *)(&pcfg->motor_enable));
    AVL_Ptz_GetScanMode(0, &pcfg->scan_mode);
    AVL_Ptz_GetPtzAngle(0, &yaw, &pcfg->pitch);
    AVL_Ptz_GetStep(0, &pcfg->step);
    AVL_Ptz_GetScanSpeed(0, (unsigned short *)&pcfg->speed);
    AVL_Ptz_GetFanScanAngle(0, &pcfg->fan_scanning.start_angle, &pcfg->fan_scanning.end_angle);
    pcfg->constant_scan.yaw = yaw;
    return ret;
}

static int MPU_CIU_SET_AreaConfig(NetworkAreas *pcfg)
{
    int ret = 0;
    if(pcfg == NULL)
    {
        ERROR(" parma is err NULL\n");
        return -1;
    }
    area_information area[NET_AREA_NUM_MAX] = {0};
    for(int i = 0; i < pcfg->area_num; i++)
    {
        memcpy(area[i].area_name, pcfg->area[i].area_name, NET_AREA_NAME_SIAE);
        area[i].type = 0;
        area[i].enable = pcfg->area[i].enable;
        area[i].point_num = pcfg->area[i].point_num;
        for(int j = 0; j < pcfg->area[i].point_num; j++)
        {
            area[i].point[j].x = pcfg->area[i].point[j].x;
            area[i].point[j].y = pcfg->area[i].point[j].y;
        }
    }
    ret = AVL_Area_SetAllAreaPoint(0, area, pcfg->area_num);
    return ret;       
}

static int MPU_CIU_GET_AreaConfig(NetworkAreas *pcfg)
{
    int ret = 0;
    if(pcfg == NULL)
    {
        ERROR(" parma is err NULL\n");
        return -1;
    }
    area_information area_all[NET_AREA_NUM_MAX];
    int num = 0; 
    ret  = AVL_Area_GetAreaPoint(0, area_all, &num);
    if(ret != 0)
    {
        ERROR("AVL_Area_GetAreaPoint is err ret = %d\n",ret);
        return -1;
    }
    pcfg->area_num = num;
    for(int i = 0; i < pcfg->area_num; i++)
    {
        memcpy(pcfg->area[i].area_name, area_all[i].area_name, NET_AREA_NAME_SIAE);
        pcfg->area[i].enable = area_all[i].enable;
        pcfg->area[i].point_num = area_all[i].point_num;
        pcfg->area[i].point_num_max = 8;
        fprintf(stderr," name = %s enable = %d point num = %d max num = %d \n", pcfg->area[i].area_name, pcfg->area[i].enable, pcfg->area[i].point_num, pcfg->area[i].point_num_max);
        fprintf(stderr, "area poing : ");
        for(int j = 0; j < pcfg->area[i].point_num; j++)
        {
            pcfg->area[i].point[j].x = area_all[i].point[j].x;
            pcfg->area[i].point[j].y = area_all[i].point[j].y;
            fprintf(stderr, "x = %d y = %d",pcfg->area[i].point[j].x, pcfg->area[i].point[j].y);
        }
        fprintf(stderr, "\n");
    }
    return ret;     
}

static int MPU_CIU_SET_OtherConfigInfo(NetworkOtherConfig *pcfg)
{
    int ret = 0;
    if(pcfg == NULL)
    {
        ERROR(" parma is err NULL\n");
        return -1;
    }
    AVL_Ext_SetWipersEnable(0, pcfg->wipers_enable);
    AVL_Ext_SetAutoLightEnable(0, pcfg->fill_light_ctrl);
    AVL_Ext_SetCvbsEnable(0, pcfg->cvbs_enable);

    return ret;
}

static int MPU_CIU_GET_OtherConfigInfo(NetworkOtherConfig *pcfg)
{
    if(pcfg == NULL)
    {
        ERROR(" parma is err NULL\n");
        return -1;
    }
    AVL_Ext_GetWipersEnable(0, (int *)&pcfg->wipers_enable);
    AVL_Ext_GetAutoLightEnable(0, (int *)&pcfg->fill_light_ctrl);
    AVL_Ext_GetCvbsEnable(0, (int *)&pcfg->cvbs_enable);
    printf("pcfg->wipers_enable = %d ,pcfg->fill_light_ctrl = %d pcfg->cvbs_enable = %d \n", pcfg->wipers_enable, pcfg->fill_light_ctrl, pcfg->cvbs_enable);
    return 0;
}

static int MPU_CIU_SET_NetConfig(NetworkNetworkInfo *pcfg)
{
    int ret = 0;
    if(pcfg == NULL)
    {
        ERROR(" parma is err NULL\n");
        return -1;
    }
    if(!pcfg->ipv4_enable)
    {
        ERROR(" Currently only supports ipv4\n");
        return -1;
    }
    ret |= MPU_NET_SetIPAddress(pcfg->ipv4_addr);
    ret |= MPU_NET_SetGateway(pcfg->ipv4_gateway);
    ret |= MPU_NET_SetSubnetMask(pcfg->ipv4_mask);
    ret |= MPU_NET_SetDns(pcfg->dns, NULL);
    sleep(3);
    mysystem("reboot");
    return ret;
}

static int MPU_CIU_GET_NetConfig(NetworkNetworkInfo *pcfg)
{
    int ret = 0;
    if(pcfg == NULL)
    {
        ERROR(" parma is err NULL\n");
        return -1;
    }
    pcfg->ipv6_enable = false;
    pcfg->ipv4_enable = true;
    pcfg->dhcp_enable = (bool)MPU_NET_GetDhcpEnable();
    MPU_NET_GetIPAddress(pcfg->ipv4_addr);
    MPU_NET_GetGateway(pcfg->ipv4_gateway);
    MPU_NET_GetSubnetMask(pcfg->ipv4_mask);
    MPU_NET_GetDns(pcfg->dns, NULL); //默认把网关当dns使用
    //printf("pcfg->ipv4_mask = %s \n",pcfg->ipv4_mask);

    return 0;
}

static int MPU_CIU_SET_AlgoConfig(NetworkAlgorithm *pcfg)
{
    int ret = 0;
    if(pcfg == NULL)
    {
        ERROR(" parma is err NULL\n");
        return -1;
    }
    AVL_Alg_SetDetectEnable(0,pcfg->detection_enable);
    AVL_Alg_SetTrackEnable(0,pcfg->tracking_enable);
    AVL_Alg_SetBehaviorEnable(0,pcfg->action_analyze_enable);
    return 0;
}

static int MPU_CIU_GET_AlgoConfig(NetworkAlgorithm *pcfg)
{
    if(pcfg == NULL)
    {
        ERROR(" parma is err NULL\n");
        return -1;
    }
    int all_alg_enable = 0;
    AVL_Alg_GetAlgEnable(0, &all_alg_enable);
    if(get_bit(&all_alg_enable, 0))
    {
        pcfg->detection_enable = true;
    }
    if(get_bit(&all_alg_enable, 1))
    {
        pcfg->tracking_enable = true;
    }
    if(get_bit(&all_alg_enable, 2))
    {
        pcfg->action_analyze_enable = true;
    }
    return 0;
}

static int MPU_CIU_SET_PtzConfig(NetworkPresets *pcfg)
{
    if(pcfg == NULL)
    {
        ERROR("set ptz is err \n");
        return -1;
    }
    traget_preset preset[256] = {0};
    for(int i = 0; i < pcfg->preset_num; i++)
    {
        preset[i].enable = 1;
        preset[i].num = pcfg->preset[i].num;
        memcpy(preset[i].name, pcfg->preset[i].name, sizeof(pcfg->preset[i].name));
    }
    AVL_Ptz_SetPresetEx(0, preset, pcfg->preset_num);
    return 0;
}

static int MPU_CIU_GET_PtzConfig(NetworkPresets *pcfg)
{
    traget_preset preset[256] = {0};
    int num = 0;
    AVL_Ptz_GetPreset(0, &preset[0], &num);
    pcfg->preset_num = num;
    for(int i = 0; i < pcfg->preset_num; i++)
    {
        pcfg->preset[i].num = preset[i].num;
        memcpy(pcfg->preset[i].name, preset[i].name, sizeof(pcfg->preset[i].name));
    }
    return 0;
}

int MPU_CIU_Set_ConfigGure(int type, void *buffer, unsigned int  bufflen)
{
    int ret;
    switch (type)
    {
    case CONFIG_SYSTEM_INFO:
        {
            if(bufflen < sizeof(NetworkSystemInfo))
            {
                ret = -1;
                ERROR("buffer is too small\n");
                break;
            }
            NetworkSystemInfo *pcfg = (NetworkSystemInfo *)buffer;
            ret = MPU_CIU_SET_ConfigSystem(pcfg);
            break;
        }   
    case CONFIG_CAMERA_INFO:
        {
            if(bufflen < sizeof(NetworkCameraChipInfo))
            {
                ret = -1;
                ERROR("buffer is too small\n");
                break;
            }
            NetworkCameraChipInfo *pcfg = (NetworkCameraChipInfo *)buffer;
            ret = MPU_CIU_SET_ConfigCamear(pcfg);
            break;
        } 
    case CONFIG_POSITION_INFO:
        {
            break;
        }
    case CONFIG_CALIBRATION_INFO:
        {
            break;
        }
    case CONFIG_PTZ_INFO:
        {
            if(bufflen < sizeof(NetworkPtzInfo))
            {
                ret = -1;
                ERROR("buffer is too small\n");
                break;
            }
            NetworkPtzInfo *pcfg = (NetworkPtzInfo *)buffer;
            ret = MPU_CIU_SET_Ptzinfo(pcfg);
            break;
        }
    case CONFIG_AREAS_INFO:
        {
            if(bufflen < sizeof(NetworkAreas))
            {
                ret = -1;
                ERROR("buffer is too small\n");
                break;
            }
            NetworkAreas *pcfg = (NetworkAreas *)buffer;
            ret = MPU_CIU_SET_AreaConfig(pcfg);
            break;
        }
    case CONFIG_OTHER_INFO:
        {
            if(bufflen < sizeof(NetworkOtherConfig))
            {
                ret = -1;
                ERROR("buffer is too small\n");
                break;
            }
            NetworkOtherConfig *pcfg = (NetworkOtherConfig *)buffer;
            ret = MPU_CIU_SET_OtherConfigInfo(pcfg);
            break;
        }
    case CONFIG_NETWORK_INFO:
        {
            if(bufflen < sizeof(NetworkNetworkInfo))
            {
                ret = -1;
                ERROR("buffer is too small\n");
                break;
            }
            NetworkNetworkInfo *pcfg = (NetworkNetworkInfo *)buffer;
            ret = MPU_CIU_SET_NetConfig(pcfg);
            break;
        }
    case CONFIG_ALGO_INFO:
        {
            if(bufflen < sizeof(NetworkAlgorithm))
            {
                ret = -1;
                ERROR("buffer is too small\n");
                break;
            }
            NetworkAlgorithm *pcfg = (NetworkAlgorithm *)buffer;
            ret = MPU_CIU_SET_AlgoConfig(pcfg);
            break;
        }
    case CONFIG_PTZ_PRESET:
        {
            if(bufflen < sizeof(NetworkPresets))
            {
                ret = -1;
                ERROR("buffer is too small\n");
                break;
            }
            NetworkPresets *pcfg = (NetworkPresets *)buffer;
            ret = MPU_CIU_SET_PtzConfig(pcfg);
            break;
        }
    default:
        break;
    }
    return ret;
}


int MPU_CIU_Get_ConfigGure(int type, void *buffer, unsigned int bufflen)
{
    int ret;
    switch (type)
    {
    case CONFIG_SYSTEM_INFO:
        {
            if(bufflen < sizeof(NetworkSystemInfo))
            {
                ret = -1;
                ERROR("buffer is too small\n");
                break;
            }
            NetworkSystemInfo *pcfg = (NetworkSystemInfo *)buffer;
            ret = MPU_CIU_GET_ConfigSystem(pcfg);
            break;
        }   
    case CONFIG_CAMERA_INFO:
        {
            if(bufflen < sizeof(NetworkCameraChipInfo))
            {
                ret = -1;
                ERROR("buffer is too small\n");
                break;
            }
            NetworkCameraChipInfo *pcfg = (NetworkCameraChipInfo *)buffer;
            ret = MPU_CIU_GET_ConfigCamear(pcfg);
            break;
        }
    case CONFIG_POSITION_INFO:
        {
            break;
        }
    case CONFIG_CALIBRATION_INFO:
        {
            break;
        } 
    case CONFIG_PTZ_INFO:
        {
           if(bufflen < sizeof(NetworkPtzInfo))
            {
                ret = -1;
                ERROR("buffer is too small\n");
                break;
            }
            NetworkPtzInfo *pcfg = (NetworkPtzInfo *)buffer;
            ret = MPU_CIU_GET_Ptzinfo(pcfg);
            break;
        }
    case CONFIG_AREAS_INFO:
        {
            if(bufflen < sizeof(NetworkAreas))
            {
                ret = -1;
                ERROR("buffer is too small\n");
                break;
            }
            NetworkAreas *pcfg = (NetworkAreas *)buffer;
            ret = MPU_CIU_GET_AreaConfig(pcfg);
            break;
        }
    case CONFIG_OTHER_INFO:
        {
            if(bufflen < sizeof(NetworkOtherConfig))
            {
                ret = -1;
                ERROR("buffer is too small\n");
                break;
            }
            NetworkOtherConfig *pcfg = (NetworkOtherConfig *)buffer;
            ret = MPU_CIU_GET_OtherConfigInfo(pcfg);
            break;
        }
    case CONFIG_NETWORK_INFO:
        {
            if(bufflen < sizeof(NetworkNetworkInfo))
            {
                ret = -1;
                ERROR("buffer is too small\n");
                break;
            }
            NetworkNetworkInfo *pcfg = (NetworkNetworkInfo *)buffer;
            ret = MPU_CIU_GET_NetConfig(pcfg);
            break;
        }
    case CONFIG_ALGO_INFO:
        {
            if(bufflen < sizeof(NetworkAlgorithm))
            {
                ret = -1;
                ERROR("buffer is too small\n");
                break;
            }
            NetworkAlgorithm *pcfg = (NetworkAlgorithm *)buffer;
            ret = MPU_CIU_GET_AlgoConfig(pcfg);
            break;
        }
    case CONFIG_PTZ_PRESET:
        {
            if(bufflen < sizeof(NetworkPresets))
            {
                ret = -1;
                ERROR("buffer is too small\n");
                break;
            }
            NetworkPresets *pcfg = (NetworkPresets *)buffer;
            ret = MPU_CIU_GET_PtzConfig(pcfg);
            break;
        }
    default:
        break;
    }
    return ret;
}

int MPU_CIU_Get_ALL_ConfigGure(unsigned int major, unsigned int minor, void *buffer, int bufflen)
{
    int ret  = 0;
    DEBUG("xh test pro major = %d minjor = %d buffer = %p bufflen = %d \n", major, minor, buffer, bufflen);
    switch (major)
    {
    case JP_NVR_GET_DEVICEINFO:
        {
            ret = MPU_CIU_GET_DevConfig(minor, buffer, bufflen);
            break;
        }
    case JP_NVR_GET_CONFIG:
        {
            ret = MPU_CIU_Get_ConfigGure(minor, buffer, bufflen);
            break;
        }
    case JP_NVR_CONTORL:
        {
            ret = MPU_CIU_CTRL_MODLES(minor, buffer, bufflen);
            break;
        }
    
    default:
        break;
    }
    return ret;
}

int MPU_CIU_Set_ALL_ConfigGure(unsigned int major, unsigned int minor, void *buffer, int bufflen)
{
     DEBUG("xh test pro major = %d minjor = %d buffer = %p bufflen = %d \n", major, minor, buffer, bufflen);
    int ret  = 0;
    switch (major)
    {
    case JP_NVR_SET_DEVICEINFO:
        {
            ret = MPU_CIU_SET_DevConfig(minor, buffer, bufflen);
            break;
        }
    case JP_NVR_SET_CONFIG:
        {
            ret = MPU_CIU_Set_ConfigGure(minor, buffer, bufflen);
            break;
        }
    case JP_NVR_CONTORL:
        {
            ret = MPU_CIU_CTRL_MODLES(minor, buffer, bufflen);
            break;
        }
    
    default:
        break;
    }
    return ret;
}

static int NETWORK_GetConfig_cb(NetworkConfigType type, void *buffer, int bufferlen)
{
    int ret = 0;
    fprintf(stderr,"xh test type = %d buffer = %d \n",type, bufferlen);
    switch (type)
    {
    case NETWORK_CONFIG_SYSTEM_INFO:
        ret = MPU_CIU_Get_ALL_ConfigGure(JP_NVR_GET_CONFIG, CONFIG_SYSTEM_INFO, buffer, bufferlen);
        break;
    case NETWORK_CONFIG_CAMERA_CHIP_INFO:
        ret = MPU_CIU_Get_ALL_ConfigGure(JP_NVR_GET_CONFIG, CONFIG_CAMERA_INFO, buffer, bufferlen);
        break;
    case NETWORK_CONFIG_POSITION:
        ret = MPU_CIU_Get_ALL_ConfigGure(JP_NVR_GET_CONFIG, CONFIG_POSITION_INFO, buffer, bufferlen);
        break;
    case NETWORK_CONFIG_CALIBRATION_GUN:
        ret = MPU_CIU_Get_ALL_ConfigGure(JP_NVR_GET_CONFIG, CONFIG_CALIBRATION_INFO, buffer, bufferlen);
        break;
    case NETWORK_CONFIG_PTZ_INFO:
        ret = MPU_CIU_Get_ALL_ConfigGure(JP_NVR_GET_CONFIG, CONFIG_PTZ_INFO, buffer, bufferlen);
        break;
    case NETWORK_CONFIG_AREAS:
        ret = MPU_CIU_Get_ALL_ConfigGure(JP_NVR_GET_CONFIG, CONFIG_AREAS_INFO, buffer, bufferlen);
        break;
    case NETWORK_CONFIG_OTHER_INFO:
        ret = MPU_CIU_Get_ALL_ConfigGure(JP_NVR_GET_CONFIG, CONFIG_OTHER_INFO, buffer, bufferlen);
        break;
    case NETWORK_CONFIG_NETWORK:
        ret = MPU_CIU_Get_ALL_ConfigGure(JP_NVR_GET_CONFIG, CONFIG_NETWORK_INFO, buffer, bufferlen);
        break;
    case NETWORK_CONFIG_ALGORITHM:
        ret = MPU_CIU_Get_ALL_ConfigGure(JP_NVR_GET_CONFIG, CONFIG_ALGO_INFO, buffer, bufferlen);
        break;
    case NETWORK_CONFIG_PRESETS:
        ret = MPU_CIU_Get_ALL_ConfigGure(JP_NVR_GET_CONFIG, CONFIG_PTZ_PRESET, buffer, bufferlen);
        break;
    default:
        break;
    }
    return ret;
}

static int NETWORK_SetConfig_cb(NetworkConfigType type, void *buffer, int bufferlen)
{
    int ret = 0;
    switch (type)
    {
    case NETWORK_CONFIG_SYSTEM_INFO:
        ret = MPU_CIU_Set_ALL_ConfigGure(JP_NVR_SET_CONFIG, CONFIG_SYSTEM_INFO, buffer, bufferlen);
        break;
    case NETWORK_CONFIG_CAMERA_CHIP_INFO:
        ret = MPU_CIU_Set_ALL_ConfigGure(JP_NVR_SET_CONFIG, CONFIG_CAMERA_INFO, buffer, bufferlen);
        break;
    case NETWORK_CONFIG_POSITION:
        ret = MPU_CIU_Set_ALL_ConfigGure(JP_NVR_SET_CONFIG, CONFIG_POSITION_INFO, buffer, bufferlen);
        break;
    case NETWORK_CONFIG_CALIBRATION_GUN:
        ret = MPU_CIU_Set_ALL_ConfigGure(JP_NVR_SET_CONFIG, CONFIG_CALIBRATION_INFO, buffer, bufferlen);
        break;
    case NETWORK_CONFIG_PTZ_INFO:
        ret = MPU_CIU_Set_ALL_ConfigGure(JP_NVR_SET_CONFIG, CONFIG_PTZ_INFO, buffer, bufferlen);
        break;
    case NETWORK_CONFIG_AREAS:
        ret = MPU_CIU_Set_ALL_ConfigGure(JP_NVR_SET_CONFIG, CONFIG_AREAS_INFO, buffer, bufferlen);
        break;
    case NETWORK_CONFIG_OTHER_INFO:
        ret = MPU_CIU_Set_ALL_ConfigGure(JP_NVR_SET_CONFIG, CONFIG_OTHER_INFO, buffer, bufferlen);
        break;
    case NETWORK_CONFIG_NETWORK:
        ret = MPU_CIU_Set_ALL_ConfigGure(JP_NVR_SET_CONFIG, CONFIG_NETWORK_INFO, buffer, bufferlen);
        break;
    case NETWORK_CONFIG_ALGORITHM:
        ret = MPU_CIU_Set_ALL_ConfigGure(JP_NVR_SET_CONFIG, CONFIG_ALGO_INFO, buffer, bufferlen);
        break;
    case NETWORK_CONFIG_PRESETS:
        ret = MPU_CIU_Set_ALL_ConfigGure(JP_NVR_SET_CONFIG, CONFIG_PTZ_PRESET, buffer, bufferlen);
        break;
    default:
        break;
    }
    return ret;
}

void CIU_RegisterConfig()
{
    NetworkOperationRegister(NETWORK_OPERATION_GET_CONFIG, (void *)NETWORK_GetConfig_cb);
    
    NetworkOperationRegister(NETWORK_OPERATION_SET_CONFIG, (void *)NETWORK_SetConfig_cb);
}

void CIU_UnRegisterConfig()
{
    NetworkOperationRegister(NETWORK_OPERATION_GET_CONFIG, NULL);
    
    NetworkOperationRegister(NETWORK_OPERATION_SET_CONFIG, NULL);
}

int mpu_ciu_config_init()
{
    int ret;
#ifdef HTTP_UPLOAD

    CIU_RegisterDevInfo();

    CIU_RegisterConfig();

    CIU_RegisterDevCrtl();

    CIU_RegisterUpdate();

	AVL_SetMpuCallback(report_peripheralCalback);
    AVL_Alarm_SetAlarmActionCallback(0, report_alarm_Calback);
#endif
    CIU_RegisterPreview();

   INFO("MPU_CIU_INIT is success\n");

    return 0;
}

int mpu_ciu_config_deinit()
{
#ifdef HTTP_UPLOAD
    CIU_UnRegisterDevInfo();

    CIU_UnRegisterConfig();

    CIU_UnRegisterDevCrtl();

    CIU_UnRegisterUpdate();
#endif

    return 0;
}

