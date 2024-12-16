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



static int MPU_CIU_SET_ConfigSystem(NetworkConfigSystemTimeInfo *pcfg)
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
    cfg.SummerTimeEnable = pcfg->summer_time;
    ret |= AVL_Coder_SetTimeConfig(0, cfg);
    return ret;
}

static int MPU_CIU_GET_ConfigSystem(NetworkConfigSystemTimeInfo *pcfg)
{
    int ret = 0;
    time_cfg cfg;
    if(pcfg == NULL)
    {
        ERROR(" parma is err NULL\n");
        return -1;
    }
    AVL_Coder_GetTimeConfig(0, &cfg);
    if(cfg.TimeZone  > 0)
        snprintf(pcfg->time_zone, sizeof(pcfg->time_zone), "UTC+%02d:%02d", (cfg.TimeZone / 60), (cfg.TimeZone % 60));
    else
        snprintf(pcfg->time_zone, sizeof(pcfg->time_zone), "UTC-%02d:%02d", (cfg.TimeZone / 60), (cfg.TimeZone % 60));
    pcfg->time_format = cfg.TimeFormat;
    pcfg->summer_time = (bool)cfg.SummerTimeEnable;
    return ret;
}

static int MPU_CIU_SET_IR_ConfigCamearImage(NetworkConfigCameraIrImageInfos *pcfg)
{
    int ret = 0;
    int i = 0;
    if(pcfg == NULL)
    {
        ERROR(" parma is err NULL\n");
        return -1;
    }
    if(pcfg->deal_num == -1)
    {
        for(i = 0; i < pcfg->num; i++)
        {
            ret |= AVL_InfraredImage_SetImageBrightness(i, pcfg->image_info[i].brightness);
            ret |= AVL_InfraredImage_SetImageContrast(i, pcfg->image_info[i].contrast);
            ret |= AVL_InfraredImage_SetInfraredImagePolarity(i, pcfg->image_info[i].pseudo_color);
            ret |= AVL_InfraredImage_SetInfraredImageSharpening(i, pcfg->image_info[i].sharpening);
        }
    }
    else
    {
        ret |= AVL_InfraredImage_SetImageBrightness(pcfg->deal_num, pcfg->image_info[pcfg->deal_num].brightness);
        ret |= AVL_InfraredImage_SetImageContrast(pcfg->deal_num, pcfg->image_info[pcfg->deal_num].contrast);
        ret |= AVL_InfraredImage_SetInfraredImagePolarity(pcfg->deal_num, pcfg->image_info[pcfg->deal_num].pseudo_color);
        ret |= AVL_InfraredImage_SetInfraredImageSharpening(pcfg->deal_num, pcfg->image_info[pcfg->deal_num].sharpening);
    }
    return ret;
}

static int MPU_CIU_GET_IR_ConfigCamearImage(NetworkConfigCameraIrImageInfos *pcfg)
{
    int i = 0;
    if(pcfg == NULL)
    {
        ERROR(" parma is err NULL\n");
        return -1;
    }
    int ret = 0;
    pcfg->num = MAX_CH_NUM;
    if(pcfg->deal_num == -1)
    {
        for(i = 0; i < pcfg->num; i++)
        {
            ret |= AVL_InfraredImage_GetImageBrightness(i, &(pcfg->image_info[i].brightness));
            ret |= AVL_InfraredImage_GetImageContrast(i, &(pcfg->image_info[i].contrast));
            ret |= AVL_InfraredImage_GetInfraredImagePolarity(i, &(pcfg->image_info[i].pseudo_color));
            ret |= AVL_InfraredImage_GetInfraredImageSharpening(i, &(pcfg->image_info[i].sharpening));
        }
    }
    else
    {
        ret |= AVL_InfraredImage_GetImageBrightness(pcfg->deal_num, &(pcfg->image_info[i].brightness));
        ret |= AVL_InfraredImage_GetImageContrast(pcfg->deal_num, &(pcfg->image_info[i].contrast));
        ret |= AVL_InfraredImage_GetInfraredImagePolarity(pcfg->deal_num, &(pcfg->image_info[i].pseudo_color));
        ret |= AVL_InfraredImage_GetInfraredImageSharpening(pcfg->deal_num, &(pcfg->image_info[i].sharpening));
    }
    return ret;
}

static int MPU_CIU_SET_IR_EleZoomConfig(NetworkConfigCameraIrImageZooms *pcfg)
{
    if(pcfg == NULL)
    {
        ERROR("the param is err \n");
        return -1;
    }
    int ret = 0;
    int i = 0;
    
    if(pcfg->deal_num == -1)
    {
        for(i = 0; i < pcfg->num; i++)
        {
            ret = AVL_InfraredImage_SetInfraredImageElectronicZoom(i, pcfg->image_zoom[i].electronic);
        }
    }
    else
    {
        ret = AVL_InfraredImage_SetInfraredImageElectronicZoom(pcfg->deal_num, pcfg->image_zoom[i].electronic);
    }
    return ret;
}

static int MPU_CIU_GET_IR_EleZoomConfig(NetworkConfigCameraIrImageZooms *pcfg)
{
    if(pcfg == NULL)
    {
        ERROR("the param is err \n");
        return -1;
    }
    int ret = 0;
    int i = 0;
    pcfg->num = MAX_CH_NUM;
    if(pcfg->deal_num == -1)
    {
        for(i = 0; i < pcfg->num; i++)
        {
            ret |= AVL_InfraredImage_GetInfraredImageElectronicZoom(i, &(pcfg->image_zoom[i].electronic));
        }
    }
    else
    {
        ret |= AVL_InfraredImage_GetInfraredImageElectronicZoom(pcfg->deal_num, &(pcfg->image_zoom[pcfg->deal_num].electronic));
    }
    
    return ret;
}

static int MPU_CIU_SET_IR_FocuConfig(NetworkConfigCameraIrFocusings *pcfg)
{
    if(pcfg == NULL)
    {
        ERROR("the param is err \n");
        return -1;
    }
    int ret = 0;
    int i = 0;
    if(pcfg->deal_num == -1)
    {
        for(i = 0; i < pcfg->num; i++)
        {
            ret |= AVL_InfraredImage_SetInfraredImageFocusMode(i, pcfg->focusing[i].focusing_mode);
            AVL_InfraredImage_SetAutoFocuData(i,1);
        }
    }
    else
    {
        ret |= AVL_InfraredImage_SetInfraredImageFocusMode(pcfg->deal_num, pcfg->focusing[i].focusing_mode);
        AVL_InfraredImage_SetAutoFocuData(0,1);
    }
    return ret;
}

static int MPU_CIU_GET_IR_FocuConfig(NetworkConfigCameraIrFocusings *pcfg)
{
    if(pcfg == NULL)
    {
        ERROR("the param is err \n");
        return -1;
    }
    int ret = 0;
    int i = 0;
    pcfg->num = MAX_CH_NUM;
    if(pcfg->deal_num == -1)
    {
        for(i = 0; i < pcfg->num; i++)
        {
            ret |= AVL_InfraredImage_GetInfraredImageFocusMode(i, &pcfg->focusing[i].focusing_mode);
        }
    }
    else
    {
        ret |= AVL_InfraredImage_GetInfraredImageFocusMode(pcfg->deal_num, &pcfg->focusing[pcfg->deal_num].focusing_mode);
    }
    return ret;
}

static int MPU_CIU_SET_IR_CalibrationConfig(NetworkConfigCameraIrCalibrations *pcfg)
{
    
    if(pcfg == NULL)
    {
        ERROR("the param is err \n");
        return -1;
    }
    int ret = 0;

    return ret;
}

static int MPU_CIU_GET_IR_CalibrationConfig(NetworkConfigCameraIrCalibrations *pcfg)
{
    
    if(pcfg == NULL)
    {
        ERROR("the param is err \n");
        return -1;
    }
    int ret = 0;

    return ret;
}

static int MPU_CIU_SET_VIS_ImageConfig(NetworkConfigCameraVisImageInfos *pcfg)
{
     if(pcfg == NULL)
    {
        ERROR("the param is err \n");
        return -1;
    }
    int ret = 0;
    ret |= AVL_Vis_SetBrightness(pcfg->deal_num, pcfg->image_info[pcfg->deal_num].brightness);
    ret |= AVL_Vis_SetContrast(pcfg->deal_num, pcfg->image_info[pcfg->deal_num].contrast);
    ret |= AVL_Vis_SetSaturation(pcfg->deal_num, pcfg->image_info[pcfg->deal_num].saturation);
    ret |= AVL_Vis_SetSharpness(pcfg->deal_num, pcfg->image_info[pcfg->deal_num].sharpening);
    return ret;
}

static int MPU_CIU_GET_VIS_ImageConfig(NetworkConfigCameraVisImageInfos *pcfg)
{
     if(pcfg == NULL)
    {
        ERROR("the param is err \n");
        return -1;
    }
    int ret = 0;
    int i = 0;
    pcfg->num = MAX_CH_NUM;
    for(i = 0; i < pcfg->num; i++)
    {
        ret |= AVL_Vis_GetBrightness(i, &pcfg->image_info[i].brightness);
        ret |= AVL_Vis_GetContrast(i, &pcfg->image_info[i].contrast);
        ret |= AVL_Vis_GetSaturation(i, &pcfg->image_info[i].saturation);
        ret |= AVL_Vis_GetSharpness(i, &pcfg->image_info[i].sharpening);
        DEBUG("brightness =%d contrast = %d staturation = %d sharpening= %d \n", pcfg->image_info[i].brightness, pcfg->image_info[i].contrast, pcfg->image_info[i].saturation, pcfg->image_info[i].sharpening);
    }

    return ret;
}

static int MPU_CIU_SET_VIS_EleZoomConfig(NetworkConfigCameraVisImageZooms *pcfg)    //使用红外接口
{
     if(pcfg == NULL)
    {
        ERROR("the param is err \n");
        return -1;
    }
    int ret = 0;
    int i = 0;
    
    if(pcfg->deal_num == -1)
    {
        for(i = 0; i < pcfg->num; i++)
        {
            ret = AVL_InfraredImage_SetInfraredImageElectronicZoom(i, pcfg->image_zoom[i].electronic);
        }
    }
    else
    {
        ret = AVL_InfraredImage_SetInfraredImageElectronicZoom(pcfg->deal_num, pcfg->image_zoom[i].electronic);
    }
    return ret;
}

static int MPU_CIU_GET_VIS_EleZoomConfig(NetworkConfigCameraVisImageZooms *pcfg)
{
     if(pcfg == NULL)
    {
        ERROR("the param is err \n");
        return -1;
    }
    int ret = 0;
    int i = 0;
    pcfg->num = MAX_CH_NUM;
    if(pcfg->deal_num == -1)
    {
        for(i = 0; i < pcfg->num; i++)
        {
            ret = AVL_InfraredImage_GetInfraredImageElectronicZoom(i, &(pcfg->image_zoom[i].electronic));
        }
    }
    else
    {
        ret = AVL_InfraredImage_GetInfraredImageElectronicZoom(pcfg->deal_num, &(pcfg->image_zoom[pcfg->deal_num].electronic));
    }

    return ret;
}

static int MPU_CIU_SET_VIS_FocuConfig(NetworkConfigCameraVisFocusings *pcfg)
{
     if(pcfg == NULL)
    {
        ERROR("the param is err \n");
        return -1;
    }
    int ret = 0;
    ret = AVL_Vis_SetFocuMode(pcfg->deal_num, pcfg->focusing[pcfg->deal_num].focusing_mode);
    return ret;
}

static int MPU_CIU_GET_VIS_FocuConfig(NetworkConfigCameraVisFocusings *pcfg)
{
     if(pcfg == NULL)
    {
        ERROR("the param is err \n");
        return -1;
    }
    int ret = 0;
    ret = AVL_Vis_GetFocuMode(pcfg->deal_num, &(pcfg->focusing[pcfg->deal_num].focusing_mode));
    return ret;
}

static int MPU_CIU_SET_PtzConfig(NetworkConfigPtzInfo *pcfg)
{
     if(pcfg == NULL)
    {
        ERROR("the param is err \n");
        return -1;
    }
    int ret = 0;

    return ret;
}

static int MPU_CIU_GET_PtzConfig(NetworkConfigPtzInfo *pcfg)
{
     if(pcfg == NULL)
    {
        ERROR("the param is err \n");
        return -1;
    }
    int ret = 0;

    return ret;
}

static int MPU_CIU_SET_PtzPresetsConfig(NetworkConfigPtzPresets *pcfg)
{
     if(pcfg == NULL)
    {
        ERROR("the param is err \n");
        return -1;
    }
    int ret = 0;
    int i = 0;
    traget_preset presetInfo[256] = {0};
    for(i = 0; i < pcfg->preset_num; i++)
    {
        memcpy(presetInfo[i].name, pcfg->preset[i].name, NETWORK_PRESET_NAME_MAX_SIZE);
        presetInfo[i].num = pcfg->preset[i].num;
        presetInfo[i].enable = 1;
    }
    ret = AVL_Ptz_SetPresetEx(0, presetInfo, pcfg->preset_num);
    return ret;
}

static int MPU_CIU_GET_PtzPresetsConfig(NetworkConfigPtzPresets *pcfg)
{
     if(pcfg == NULL)
    {
        ERROR("the param is err \n");
        return -1;
    }
    int i = 0;
    int ret = 0;
    traget_preset presetInfo[256] = {0};
    ret = AVL_Ptz_GetPreset(0, presetInfo, &pcfg->preset_num);
    for(i = 0; i < pcfg->preset_num; i++)
    {
        memcpy(pcfg->preset[i].name, presetInfo[i].name, NETWORK_PRESET_NAME_MAX_SIZE);
        pcfg->preset[i].num = presetInfo[i].num;
    }
    return ret;
}

static int MPU_CIU_SET_AlgoDetectConfig(NetworkConfigAlgorithmDetection *pcfg)
{
     if(pcfg == NULL)
    {
        ERROR("the param is err \n");
        return -1;
    }
    int ret = 0;

    return ret;
}

static int MPU_CIU_GET_AlgoDetectConfig(NetworkConfigAlgorithmDetection *pcfg)
{
     if(pcfg == NULL)
    {
        ERROR("the param is err \n");
        return -1;
    }
    int ret = 0;

    return ret;
}

static int MPU_CIU_SET_AlgoAreaConfig(NetworkConfigAlgorithmAreas *pcfg)
{
     if(pcfg == NULL)
    {
        ERROR("the param is err \n");
        return -1;
    }
    int ret = 0;

    return ret;
}

static int MPU_CIU_GET_AlgoAreaConfig(NetworkConfigAlgorithmAreas *pcfg)
{
     if(pcfg == NULL)
    {
        ERROR("the param is err \n");
        return -1;
    }
    int ret = 0;

    return ret;
}

static int MPU_CIU_SET_AlgoImageConfig(NetworkConfigAlgorithmImage *pcfg)
{
     if(pcfg == NULL)
    {
        ERROR("the param is err \n");
        return -1;
    }
    int ret = 0;
    ret =  AVL_InfraredImage_SetGasEnhanced(0, pcfg->gas_display_enhance);
    return ret;
}

static int MPU_CIU_GET_AlgoImageConfig(NetworkConfigAlgorithmImage *pcfg)
{
     if(pcfg == NULL)
    {
        ERROR("the param is err \n");
        return -1;
    }
    int ret = 0;
    ret =  AVL_InfraredImage_GetGasEnhanced(0, &pcfg->gas_display_enhance);
    return ret;
}

static int MPU_CIU_SET_TcpIpConfig(NetworkConfigNetworkTcpIp *pcfg)
{
     if(pcfg == NULL)
    {
        ERROR("the param is err \n");
        return -1;
    }
    int ret = 0;

    return ret;
}

static int MPU_CIU_GET_TcpIpConfig(NetworkConfigNetworkTcpIp *pcfg)
{
     if(pcfg == NULL)
    {
        ERROR("the param is err \n");
        return -1;
    }
    int ret = 0;

    return ret;
}

static int MPU_CIU_SET_ScreenInfoConfig(NetworkConfigDisplayScreenInfo *pcfg)
{
     if(pcfg == NULL)
    {
        ERROR("the param is err \n");
        return -1;
    }
    int ret = 0;

    return ret;
}

static int MPU_CIU_GET_ScreenInfoConfig(NetworkConfigDisplayScreenInfo *pcfg)
{
     if(pcfg == NULL)
    {
        ERROR("the param is err \n");
        return -1;
    }
    int ret = 0;

    return ret;
}

static int MPU_CIU_SET_ScreenCalibationConfig(NetworkConfigDisplayScreenCalibrationGun *pcfg)
{
     if(pcfg == NULL)
    {
        ERROR("the param is err \n");
        return -1;
    }
    int ret = 0;

    return ret;
}

static int MPU_CIU_GET_ScreenCalibationConfig(NetworkConfigDisplayScreenCalibrationGun *pcfg)
{
     if(pcfg == NULL)
    {
        ERROR("the param is err \n");
        return -1;
    }
    int ret = 0;

    return ret;
}

static int MPU_CIU_SET_ScreenRedDotConfig(NetworkConfigDisplayScreenRedDotInfo *pcfg)
{
     if(pcfg == NULL)
    {
        ERROR("the param is err \n");
        return -1;
    }
    int ret = 0;

    return ret;
}

static int MPU_CIU_GET_ScreenRedDotConfig(NetworkConfigDisplayScreenRedDotInfo *pcfg)
{
     if(pcfg == NULL)
    {
        ERROR("the param is err \n");
        return -1;
    }
    int ret = 0;

    return ret;
}

static int MPU_CIU_SET_OtherInfoConfig(NetworkConfigOtherInfo *pcfg)
{
     if(pcfg == NULL)
    {
        ERROR("the param is err \n");
        return -1;
    }
    int ret = 0;
    ret = AVL_Ext_SetAutoLightEnable(0, pcfg->fill_light);
    ret = AVL_Ext_SetWipersEnable(0, pcfg->fan);
    return ret;
}

static int MPU_CIU_GET_OtherInfoConfig(NetworkConfigOtherInfo *pcfg)
{
     if(pcfg == NULL)
    {
        ERROR("the param is err \n");
        return -1;
    }
    int ret = 0;
    ret = AVL_Ext_GetAutoLightEnable(0, &pcfg->fill_light);
    ret = AVL_Ext_GetWipersEnable(0, &pcfg->fan);
    return ret;
}

static int MPU_CIU_SET_OtherPreviewConfig(NetworkConfigOtherPreview *pcfg)
{
     if(pcfg == NULL)
    {
        ERROR("the param is err \n");
        return -1;
    }
    int ret = 0;
    ret =  AVL_Ext_SetCvbsEnable(0, pcfg->cvbs);
    return ret;
}

static int MPU_CIU_GET_OtherPreviewConfig(NetworkConfigOtherPreview *pcfg)
{
     if(pcfg == NULL)
    {
        ERROR("the param is err \n");
        return -1;
    }
    int ret = 0;
    ret =  AVL_Ext_GetCvbsEnable(0, &pcfg->cvbs);
    return ret;
}

static int MPU_CIU_SET_OtherPositionConfig(NetworkConfigOtherPosition *pcfg)
{
     if(pcfg == NULL)
    {
        ERROR("the param is err \n");
        return -1;
    }
    int ret = 0;

    return ret;
}

static int MPU_CIU_GET_OtherPositionConfig(NetworkConfigOtherPosition *pcfg)
{
     if(pcfg == NULL)
    {
        ERROR("the param is err \n");
        return -1;
    }
    int ret = 0;

    return ret;
}

int MPU_CIU_Set_ConfigGure(int type, void *buffer, unsigned int  bufflen)
{
    int ret;
    switch (type)
    {
    case CONFIG_SYSTEM_TIME_INFO:
    {
        if(bufflen < sizeof(NetworkConfigSystemTimeInfo))
        {
            ret = -1;
            ERROR("buffer is too small\n");
            break;
        }
        NetworkConfigSystemTimeInfo *pcfg = (NetworkConfigSystemTimeInfo *)buffer;
        ret = MPU_CIU_SET_ConfigSystem(pcfg);
        break;
    }   
    case CONFIG_CAMERA_IR_IMAGE_INFO:
    {
        if(bufflen < sizeof(NetworkConfigCameraIrImageInfos))
        {
            ret = -1;
            ERROR("buffer is too small\n");
            break;
        }
        NetworkConfigCameraIrImageInfos *pcfg = (NetworkConfigCameraIrImageInfos *)buffer;
        ret = MPU_CIU_SET_IR_ConfigCamearImage(pcfg);
        break;
    } 
    case CONFIG_CAMERA_IR_IMAGE_ENHANCE:
    {
        if(bufflen < sizeof(NetworkConfigCameraIrImageEnhance))
        {
            ret = -1;
            ERROR("buffer is too small\n");
            break;
        }
        ERROR("not subpoort ir enhance\n");
        ret = -1;
        break;
    }
    case CONFIG_CAMERA_IR_IMAGE_ZOOM:
    {
        if(bufflen < sizeof(NetworkConfigCameraIrImageZooms))
        {
            ret = -1;
            ERROR("buffer is too small\n");
            break;
        }
        NetworkConfigCameraIrImageZooms *pcfg = (NetworkConfigCameraIrImageZooms *)buffer;
        ret = MPU_CIU_SET_IR_EleZoomConfig(pcfg);
        break;
    }
    case CONFIG_CAMERA_IR_FOCUSING:
        {
            if(bufflen < sizeof(NetworkConfigCameraIrFocusings))
            {
                ret = -1;
                ERROR("buffer is too small\n");
                break;
            }
            NetworkConfigCameraIrFocusings *pcfg = (NetworkConfigCameraIrFocusings *)buffer;
            ret = MPU_CIU_SET_IR_FocuConfig(pcfg);
            break;
        }
    case CONFIG_CAMERA_IR_CALIBRATIONS:
    {
        if(bufflen < sizeof(NetworkConfigCameraIrCalibrations))
        {
            ret = -1;
            ERROR("buffer is too small\n");
            break;
        }
        NetworkConfigCameraIrCalibrations *pcfg = (NetworkConfigCameraIrCalibrations *)buffer;
        ret = MPU_CIU_SET_IR_CalibrationConfig(pcfg);
        break;
    }
    case CONFIG_CAMERA_VIS_IMAGE_INFO:
    {
        if(bufflen < sizeof(NetworkConfigCameraVisImageInfos))
        {
            ret = -1;
            ERROR("buffer is too small\n");
            break;
        }
        NetworkConfigCameraVisImageInfos *pcfg = (NetworkConfigCameraVisImageInfos *)buffer;
        ret = MPU_CIU_SET_VIS_ImageConfig(pcfg);
        break;
    }
    case CONFIG_CAMERA_VIS_IMAGE_ZOOM:
    {
        if(bufflen < sizeof(NetworkConfigCameraVisImageZooms))
        {
            ret = -1;
            ERROR("buffer is too small\n");
            break;
        }
        NetworkConfigCameraVisImageZooms *pcfg = (NetworkConfigCameraVisImageZooms *)buffer;
        ret = MPU_CIU_SET_VIS_EleZoomConfig(pcfg);
        break;
    }
    case CONFIG_CAMERA_VIS_FOCUING:
    {
        if(bufflen < sizeof(NetworkConfigCameraVisFocusings))
        {
            ret = -1;
            ERROR("buffer is too small\n");
            break;
        }
        NetworkConfigCameraVisFocusings *pcfg = (NetworkConfigCameraVisFocusings *)buffer;
        ret = MPU_CIU_SET_VIS_FocuConfig(pcfg);
        break;
    }
    case CONFIG_PTZ_INFO:
    {
        if(bufflen < sizeof(NetworkConfigPtzInfo))
        {
            ret = -1;
            ERROR("buffer is too small\n");
            break;
        }
        NetworkConfigPtzInfo *pcfg = (NetworkConfigPtzInfo *)buffer;
        ret = MPU_CIU_SET_PtzConfig(pcfg);
        break;
    }
    case CONFIG_PTZ_PRESETS:
    {
        if(bufflen < sizeof(NetworkConfigPtzPresets))
        {
            ret = -1;
            ERROR("buffer is too small\n");
            break;
        }
        NetworkConfigPtzPresets *pcfg = (NetworkConfigPtzPresets *)buffer;
        ret = MPU_CIU_SET_PtzPresetsConfig(pcfg);
        break;
    }
    case NETWORK_CONFIG_ALGORITHM_DETECTION:
    {
        if(bufflen < sizeof(NetworkConfigAlgorithmDetection))
        {
            ret = -1;
            ERROR("buffer is too small\n");
            break;
        }
        NetworkConfigAlgorithmDetection *pcfg = (NetworkConfigAlgorithmDetection *)buffer;
        ret = MPU_CIU_SET_AlgoDetectConfig(pcfg);
        break;
    }
    case CONFIG_ALGORITHM_AREAS:
    {
        if(bufflen < sizeof(NetworkConfigAlgorithmAreas))
        {
            ret = -1;
            ERROR("buffer is too small\n");
            break;
        }
        NetworkConfigAlgorithmAreas *pcfg = (NetworkConfigAlgorithmAreas *)buffer;
        ret = MPU_CIU_SET_AlgoAreaConfig(pcfg);
        break;
    }
    case CONFIG_ALGORITHM_IMAGE:
    {
        if(bufflen < sizeof(NetworkConfigAlgorithmImage))
        {
            ret = -1;
            ERROR("buffer is too small\n");
            break;
        }
        NetworkConfigAlgorithmImage *pcfg = (NetworkConfigAlgorithmImage *)buffer;
        ret = MPU_CIU_SET_AlgoImageConfig(pcfg);
        break;
    }
    case CONFIG_NETWORK_TCP_IP:
    {
        if(bufflen < sizeof(NetworkConfigNetworkTcpIp))
        {
            ret = -1;
            ERROR("buffer is too small\n");
            break;
        }
        NetworkConfigNetworkTcpIp *pcfg = (NetworkConfigNetworkTcpIp *)buffer;
        ret = MPU_CIU_SET_TcpIpConfig(pcfg);
        break;
    }
    case CONFIG_DISPLAY_SCREEN_INFO:
    {
        if(bufflen < sizeof(NetworkConfigDisplayScreenInfo))
        {
            ret = -1;
            ERROR("buffer is too small\n");
            break;
        }
        NetworkConfigDisplayScreenInfo *pcfg = (NetworkConfigDisplayScreenInfo *)buffer;
        ret = MPU_CIU_SET_ScreenInfoConfig(pcfg);
        break;
    }
    case CONFIG_DISPLAY_SCREEN_CALIBRATION_GUN:
    {
        if(bufflen < sizeof(NetworkConfigDisplayScreenCalibrationGun))
        {
            ret = -1;
            ERROR("buffer is too small\n");
            break;
        }
        NetworkConfigDisplayScreenCalibrationGun *pcfg = (NetworkConfigDisplayScreenCalibrationGun *)buffer;
        ret = MPU_CIU_SET_ScreenCalibationConfig(pcfg);
        break;
    }
    case CONFIG_DISPLAY_SCREEN_RED_DOT_INFO:
    {
        if(bufflen < sizeof(NetworkConfigDisplayScreenRedDotInfo))
        {
            ret = -1;
            ERROR("buffer is too small\n");
            break;
        }
        NetworkConfigDisplayScreenRedDotInfo *pcfg = (NetworkConfigDisplayScreenRedDotInfo *)buffer;
        ret = MPU_CIU_SET_ScreenRedDotConfig(pcfg);
        break;
    }
    case CONFIG_OTHER_INFO:
    {
        if(bufflen < sizeof(NetworkConfigOtherInfo))
        {
            ret = -1;
            ERROR("buffer is too small\n");
            break;
        }
        NetworkConfigOtherInfo *pcfg = (NetworkConfigOtherInfo *)buffer;
        ret = MPU_CIU_SET_OtherInfoConfig(pcfg);
        break;
    }
    case CONFIG_OTHER_PREVIEW:
    {
        if(bufflen < sizeof(NetworkConfigOtherPreview))
        {
            ret = -1;
            ERROR("buffer is too small\n");
            break;
        }
        NetworkConfigOtherPreview *pcfg = (NetworkConfigOtherPreview *)buffer;
        ret = MPU_CIU_SET_OtherPreviewConfig(pcfg);
        break;
    }
    case CONFIG_OTHER_POSITION:
    {
        if(bufflen < sizeof(NetworkConfigOtherPosition))
        {
            ret = -1;
            ERROR("buffer is too small\n");
            break;
        }
        NetworkConfigOtherPosition *pcfg = (NetworkConfigOtherPosition *)buffer;
        ret = MPU_CIU_SET_OtherPositionConfig(pcfg);
        break;
    }
    default:
        ERROR("the type is err \n");
        ret = -1;
        break;
    }
    return ret;
}


int MPU_CIU_Get_ConfigGure(int type, void *buffer, unsigned int bufflen)
{
    int ret;
    switch (type)
    {
    case CONFIG_SYSTEM_TIME_INFO:
    {
        if(bufflen < sizeof(NetworkConfigSystemTimeInfo))
        {
            ret = -1;
            ERROR("buffer is too small\n");
            break;
        }
        NetworkConfigSystemTimeInfo *pcfg = (NetworkConfigSystemTimeInfo *)buffer;
        ret = MPU_CIU_GET_ConfigSystem(pcfg);
        break;
    }   
    case CONFIG_CAMERA_IR_IMAGE_INFO:
    {
        if(bufflen < sizeof(NetworkConfigCameraIrImageInfos))
        {
            ret = -1;
            ERROR("buffer is too small\n");
            break;
        }
        NetworkConfigCameraIrImageInfos *pcfg = (NetworkConfigCameraIrImageInfos *)buffer;
        ret = MPU_CIU_GET_IR_ConfigCamearImage(pcfg);
        break;
    } 
    case CONFIG_CAMERA_IR_IMAGE_ENHANCE:
    {
        if(bufflen < sizeof(NetworkConfigCameraIrImageEnhance))
        {
            ret = -1;
            ERROR("buffer is too small\n");
            break;
        }
        ERROR("not support ir enhance\n");
        ret = -1;
        break;
    }
    case CONFIG_CAMERA_IR_IMAGE_ZOOM:
    {
        if(bufflen < sizeof(NetworkConfigCameraIrImageZooms))
        {
            ret = -1;
            ERROR("buffer is too small\n");
            break;
        }
        NetworkConfigCameraIrImageZooms *pcfg = (NetworkConfigCameraIrImageZooms *)buffer;
        ret = MPU_CIU_GET_IR_EleZoomConfig(pcfg);
        break;
    }
    case CONFIG_CAMERA_IR_FOCUSING:
        {
            if(bufflen < sizeof(NetworkConfigCameraIrFocusings))
            {
                ret = -1;
                ERROR("buffer is too small\n");
                break;
            }
            NetworkConfigCameraIrFocusings *pcfg = (NetworkConfigCameraIrFocusings *)buffer;
            ret = MPU_CIU_GET_IR_FocuConfig(pcfg);
            break;
        }
    case CONFIG_CAMERA_IR_CALIBRATIONS:
    {
        if(bufflen < sizeof(NetworkConfigCameraIrCalibrations))
        {
            ret = -1;
            ERROR("buffer is too small\n");
            break;
        }
        NetworkConfigCameraIrCalibrations *pcfg = (NetworkConfigCameraIrCalibrations *)buffer;
        ret = MPU_CIU_GET_IR_CalibrationConfig(pcfg);
        break;
    }
    case CONFIG_CAMERA_VIS_IMAGE_INFO:
    {
        if(bufflen < sizeof(NetworkConfigCameraVisImageInfos))
        {
            ret = -1;
            ERROR("buffer is too small\n");
            break;
        }
        NetworkConfigCameraVisImageInfos *pcfg = (NetworkConfigCameraVisImageInfos *)buffer;
        ret = MPU_CIU_GET_VIS_ImageConfig(pcfg);
        break;
    }
    case CONFIG_CAMERA_VIS_IMAGE_ZOOM:
    {
        if(bufflen < sizeof(NetworkConfigCameraVisImageZooms))
        {
            ret = -1;
            ERROR("buffer is too small\n");
            break;
        }
        NetworkConfigCameraVisImageZooms *pcfg = (NetworkConfigCameraVisImageZooms *)buffer;
        ret = MPU_CIU_GET_VIS_EleZoomConfig(pcfg);
        break;
    }
    case CONFIG_CAMERA_VIS_FOCUING:
    {
        if(bufflen < sizeof(NetworkConfigCameraVisFocusings))
        {
            ret = -1;
            ERROR("buffer is too small\n");
            break;
        }
        NetworkConfigCameraVisFocusings *pcfg = (NetworkConfigCameraVisFocusings *)buffer;
        ret = MPU_CIU_GET_VIS_FocuConfig(pcfg);
        break;
    }
    case CONFIG_PTZ_INFO:
    {
        if(bufflen < sizeof(NetworkConfigPtzInfo))
        {
            ret = -1;
            ERROR("buffer is too small\n");
            break;
        }
        NetworkConfigPtzInfo *pcfg = (NetworkConfigPtzInfo *)buffer;
        ret = MPU_CIU_GET_PtzConfig(pcfg);
        break;
    }
    case CONFIG_PTZ_PRESETS:
    {
        if(bufflen < sizeof(NetworkConfigPtzPresets))
        {
            ret = -1;
            ERROR("buffer is too small\n");
            break;
        }
        NetworkConfigPtzPresets *pcfg = (NetworkConfigPtzPresets *)buffer;
        ret = MPU_CIU_GET_PtzPresetsConfig(pcfg);
        break;
    }
    case NETWORK_CONFIG_ALGORITHM_DETECTION:
    {
        if(bufflen < sizeof(NetworkConfigAlgorithmDetection))
        {
            ret = -1;
            ERROR("buffer is too small\n");
            break;
        }
        NetworkConfigAlgorithmDetection *pcfg = (NetworkConfigAlgorithmDetection *)buffer;
        ret = MPU_CIU_GET_AlgoDetectConfig(pcfg);
        break;
    }

    case CONFIG_ALGORITHM_AREAS:
    {
        if(bufflen < sizeof(NetworkConfigAlgorithmAreas))
        {
            ret = -1;
            ERROR("buffer is too small\n");
            break;
        }
        NetworkConfigAlgorithmAreas *pcfg = (NetworkConfigAlgorithmAreas *)buffer;
        ret = MPU_CIU_GET_AlgoAreaConfig(pcfg);
        break;
    }
    case CONFIG_ALGORITHM_IMAGE:
    {
        if(bufflen < sizeof(NetworkConfigAlgorithmImage))
        {
            ret = -1;
            ERROR("buffer is too small\n");
            break;
        }
        NetworkConfigAlgorithmImage *pcfg = (NetworkConfigAlgorithmImage *)buffer;
        ret = MPU_CIU_GET_AlgoImageConfig(pcfg);
        break;
    }
    case CONFIG_NETWORK_TCP_IP:
    {
        if(bufflen < sizeof(NetworkConfigNetworkTcpIp))
        {
            ret = -1;
            ERROR("buffer is too small\n");
            break;
        }
        NetworkConfigNetworkTcpIp *pcfg = (NetworkConfigNetworkTcpIp *)buffer;
        ret = MPU_CIU_GET_TcpIpConfig(pcfg);
        break;
    }
    case CONFIG_DISPLAY_SCREEN_INFO:
    {
        if(bufflen < sizeof(NetworkConfigDisplayScreenInfo))
        {
            ret = -1;
            ERROR("buffer is too small\n");
            break;
        }
        NetworkConfigDisplayScreenInfo *pcfg = (NetworkConfigDisplayScreenInfo *)buffer;
        ret = MPU_CIU_GET_ScreenInfoConfig(pcfg);
        break;
    }
    case CONFIG_DISPLAY_SCREEN_CALIBRATION_GUN:
    {
        if(bufflen < sizeof(NetworkConfigDisplayScreenCalibrationGun))
        {
            ret = -1;
            ERROR("buffer is too small\n");
            break;
        }
        NetworkConfigDisplayScreenCalibrationGun *pcfg = (NetworkConfigDisplayScreenCalibrationGun *)buffer;
        ret = MPU_CIU_GET_ScreenCalibationConfig(pcfg);
        break;
    }
    case CONFIG_DISPLAY_SCREEN_RED_DOT_INFO:
    {
        if(bufflen < sizeof(NetworkConfigDisplayScreenRedDotInfo))
        {
            ret = -1;
            ERROR("buffer is too small\n");
            break;
        }
        NetworkConfigDisplayScreenRedDotInfo *pcfg = (NetworkConfigDisplayScreenRedDotInfo *)buffer;
        ret = MPU_CIU_GET_ScreenRedDotConfig(pcfg);
        break;
    }
    case CONFIG_OTHER_INFO:
    {
        if(bufflen < sizeof(NetworkConfigOtherInfo))
        {
            ret = -1;
            ERROR("buffer is too small\n");
            break;
        }
        NetworkConfigOtherInfo *pcfg = (NetworkConfigOtherInfo *)buffer;
        ret = MPU_CIU_GET_OtherInfoConfig(pcfg);
        break;
    }
    case CONFIG_OTHER_PREVIEW:
    {
        if(bufflen < sizeof(NetworkConfigOtherPreview))
        {
            ret = -1;
            ERROR("buffer is too small\n");
            break;
        }
        NetworkConfigOtherPreview *pcfg = (NetworkConfigOtherPreview *)buffer;
        ret = MPU_CIU_GET_OtherPreviewConfig(pcfg);
        break;
    }
    case CONFIG_OTHER_POSITION:
    {
        if(bufflen < sizeof(NetworkConfigOtherPosition))
        {
            ret = -1;
            ERROR("buffer is too small\n");
            break;
        }
        NetworkConfigOtherPosition *pcfg = (NetworkConfigOtherPosition *)buffer;
        ret = MPU_CIU_GET_OtherPositionConfig(pcfg);
        break;
    }
    default:
        ERROR("the type is err \n");
        ret = -1;
        break;
    }
    return ret;
}

int MPU_CIU_Get_ALL_ConfigGure(unsigned int major, unsigned int minor, void *buffer, int bufflen, void *reserve, int reserveLen)
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
            ret = MPU_CIU_CTRL_MODLES(minor, buffer, bufflen, reserve, reserveLen);
            break;
        }
    
    default:
        break;
    }
    return ret;
}

int MPU_CIU_Set_ALL_ConfigGure(unsigned int major, unsigned int minor, void *buffer, int bufflen, void *reserve, int reserveLen)
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
            ret = MPU_CIU_CTRL_MODLES(minor, buffer, bufflen, reserve, reserveLen);
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
    switch (type)
    {
    case NETWORK_CONFIG_SYSTEM_TIME_INFO:
        ret = MPU_CIU_Get_ALL_ConfigGure(JP_NVR_GET_CONFIG, CONFIG_SYSTEM_TIME_INFO, buffer, bufferlen, NULL, 0);
        break;
    case NETWORK_CONFIG_CAMERA_IR_IMAGE_INFO:
        ret = MPU_CIU_Get_ALL_ConfigGure(JP_NVR_GET_CONFIG, CONFIG_CAMERA_IR_IMAGE_INFO, buffer, bufferlen, NULL, 0);
        break;
    case NETWORK_CONFIG_CAMERA_IR_IMAGE_ENHANCE:
        ret = MPU_CIU_Get_ALL_ConfigGure(JP_NVR_GET_CONFIG, CONFIG_CAMERA_IR_IMAGE_ENHANCE, buffer, bufferlen, NULL, 0);
        break;
    case NETWORK_CONFIG_CAMERA_IR_IMAGE_ZOOM:
        ret = MPU_CIU_Get_ALL_ConfigGure(JP_NVR_GET_CONFIG, CONFIG_CAMERA_IR_IMAGE_ZOOM, buffer, bufferlen, NULL, 0);
        break;
    case NETWORK_CONFIG_CAMERA_IR_FOCUSING:
        ret = MPU_CIU_Get_ALL_ConfigGure(JP_NVR_GET_CONFIG, CONFIG_CAMERA_IR_FOCUSING, buffer, bufferlen, NULL, 0);
        break;
    case NETWORK_CONFIG_CAMERA_IR_CALIBRATIONS:
        ret = MPU_CIU_Get_ALL_ConfigGure(JP_NVR_GET_CONFIG, CONFIG_CAMERA_IR_CALIBRATIONS, buffer, bufferlen, NULL, 0);
        break;
    case NETWORK_CONFIG_CAMERA_VIS_IMAGE_INFO:
        ret = MPU_CIU_Get_ALL_ConfigGure(JP_NVR_GET_CONFIG, CONFIG_CAMERA_VIS_IMAGE_INFO, buffer, bufferlen, NULL, 0);
        break;
    case NETWORK_CONFIG_CAMERA_VIS_IMAGE_ZOOM:
        ret = MPU_CIU_Get_ALL_ConfigGure(JP_NVR_GET_CONFIG, CONFIG_CAMERA_VIS_IMAGE_ZOOM, buffer, bufferlen, NULL, 0);
        break;
    case NETWORK_CONFIG_CAMERA_VIS_FOCUSING:
        ret = MPU_CIU_Get_ALL_ConfigGure(JP_NVR_GET_CONFIG, CONFIG_CAMERA_VIS_FOCUING, buffer, bufferlen, NULL, 0);
        break;
    case NETWORK_CONFIG_PTZ_INFO:
        ret = MPU_CIU_Get_ALL_ConfigGure(JP_NVR_GET_CONFIG, CONFIG_PTZ_INFO, buffer, bufferlen, NULL, 0);
        break;
    case NETWORK_CONFIG_PTZ_PRESETS:
        ret = MPU_CIU_Get_ALL_ConfigGure(JP_NVR_GET_CONFIG, CONFIG_PTZ_PRESETS, buffer, bufferlen, NULL, 0);
        break;
     case NETWORK_CONFIG_ALGORITHM_DETECTION:
        ret = MPU_CIU_Get_ALL_ConfigGure(JP_NVR_GET_CONFIG, CONFIG_ALGORITHM_DETECTION, buffer, bufferlen, NULL, 0);
        break;
     case NETWORK_CONFIG_ALGORITHM_AREAS:
        ret = MPU_CIU_Get_ALL_ConfigGure(JP_NVR_GET_CONFIG, CONFIG_ALGORITHM_AREAS, buffer, bufferlen, NULL, 0);
        break;
     case NETWORK_CONFIG_NETWORK_TCP_IP:
        ret = MPU_CIU_Get_ALL_ConfigGure(JP_NVR_GET_CONFIG, CONFIG_NETWORK_TCP_IP, buffer, bufferlen, NULL, 0);
        break;
     case NETWORK_CONFIG_DISPLAY_SCREEN_INFO:
        ret = MPU_CIU_Get_ALL_ConfigGure(JP_NVR_GET_CONFIG, CONFIG_DISPLAY_SCREEN_INFO, buffer, bufferlen, NULL, 0);
        break;
     case NETWORK_CONFIG_DISPLAY_SCREEN_CALIBRATION_GUN:
        ret = MPU_CIU_Get_ALL_ConfigGure(JP_NVR_GET_CONFIG, CONFIG_DISPLAY_SCREEN_CALIBRATION_GUN, buffer, bufferlen, NULL, 0);
        break;
     case NETWORK_CONFIG_DISPLAY_SCREEN_RED_DOT_INFO:
        ret = MPU_CIU_Get_ALL_ConfigGure(JP_NVR_GET_CONFIG, CONFIG_DISPLAY_SCREEN_RED_DOT_INFO, buffer, bufferlen, NULL, 0);
        break;
     case NETWORK_CONFIG_OTHER_INFO:
        ret = MPU_CIU_Get_ALL_ConfigGure(JP_NVR_GET_CONFIG, CONFIG_OTHER_INFO, buffer, bufferlen, NULL, 0);
        break;
     case NETWORK_CONFIG_OTHER_PREVIEW:
        ret = MPU_CIU_Get_ALL_ConfigGure(JP_NVR_GET_CONFIG, CONFIG_OTHER_PREVIEW, buffer, bufferlen, NULL, 0);
        break;
     case NETWORK_CONFIG_OTHER_POSITION:
        ret = MPU_CIU_Get_ALL_ConfigGure(JP_NVR_GET_CONFIG, CONFIG_OTHER_POSITION, buffer, bufferlen, NULL, 0);
        break;
    default:
        ERROR(" the prarm is not support \n");
        ret = -1;
        break;
    }
    return ret;
}

static int NETWORK_SetConfig_cb(NetworkConfigType type, void *buffer, int bufferlen)
{
    int ret = 0;
    switch (type)
    {
    case NETWORK_CONFIG_SYSTEM_TIME_INFO:
        ret = MPU_CIU_Set_ALL_ConfigGure(JP_NVR_SET_CONFIG, CONFIG_SYSTEM_TIME_INFO, buffer, bufferlen, NULL, 0);
        break;
    case NETWORK_CONFIG_CAMERA_IR_IMAGE_INFO:
        ret = MPU_CIU_Set_ALL_ConfigGure(JP_NVR_SET_CONFIG, CONFIG_CAMERA_IR_IMAGE_INFO, buffer, bufferlen, NULL, 0);
        break;
    case NETWORK_CONFIG_CAMERA_IR_IMAGE_ENHANCE:
        ret = MPU_CIU_Set_ALL_ConfigGure(JP_NVR_SET_CONFIG, CONFIG_CAMERA_IR_IMAGE_ENHANCE, buffer, bufferlen, NULL, 0);
        break;
    case NETWORK_CONFIG_CAMERA_IR_IMAGE_ZOOM:
        ret = MPU_CIU_Set_ALL_ConfigGure(JP_NVR_SET_CONFIG, CONFIG_CAMERA_IR_IMAGE_ZOOM, buffer, bufferlen, NULL, 0);
        break;
    case NETWORK_CONFIG_CAMERA_IR_FOCUSING:
        ret = MPU_CIU_Set_ALL_ConfigGure(JP_NVR_SET_CONFIG, CONFIG_CAMERA_IR_FOCUSING, buffer, bufferlen, NULL, 0);
        break;
    case NETWORK_CONFIG_CAMERA_IR_CALIBRATIONS:
        ret = MPU_CIU_Set_ALL_ConfigGure(JP_NVR_SET_CONFIG, CONFIG_CAMERA_IR_CALIBRATIONS, buffer, bufferlen, NULL, 0);
        break;
    case NETWORK_CONFIG_CAMERA_VIS_IMAGE_INFO:
        ret = MPU_CIU_Set_ALL_ConfigGure(JP_NVR_SET_CONFIG, CONFIG_CAMERA_VIS_IMAGE_INFO, buffer, bufferlen, NULL, 0);
        break;
    case NETWORK_CONFIG_CAMERA_VIS_IMAGE_ZOOM:
        ret = MPU_CIU_Set_ALL_ConfigGure(JP_NVR_SET_CONFIG, CONFIG_CAMERA_VIS_IMAGE_ZOOM, buffer, bufferlen, NULL, 0);
        break;
    case NETWORK_CONFIG_CAMERA_VIS_FOCUSING:
        ret = MPU_CIU_Set_ALL_ConfigGure(JP_NVR_SET_CONFIG, CONFIG_CAMERA_VIS_FOCUING, buffer, bufferlen, NULL, 0);
        break;
    case NETWORK_CONFIG_PTZ_INFO:
        ret = MPU_CIU_Set_ALL_ConfigGure(JP_NVR_SET_CONFIG, CONFIG_PTZ_INFO, buffer, bufferlen, NULL, 0);
        break;
    case NETWORK_CONFIG_PTZ_PRESETS:
        ret = MPU_CIU_Set_ALL_ConfigGure(JP_NVR_SET_CONFIG, CONFIG_PTZ_PRESETS, buffer, bufferlen, NULL, 0);
        break;
     case NETWORK_CONFIG_ALGORITHM_DETECTION:
        ret = MPU_CIU_Set_ALL_ConfigGure(JP_NVR_SET_CONFIG, CONFIG_ALGORITHM_DETECTION, buffer, bufferlen, NULL, 0);
        break;
     case NETWORK_CONFIG_ALGORITHM_AREAS:
        ret = MPU_CIU_Set_ALL_ConfigGure(JP_NVR_SET_CONFIG, CONFIG_ALGORITHM_AREAS, buffer, bufferlen, NULL, 0);
        break;
     case NETWORK_CONFIG_NETWORK_TCP_IP:
        ret = MPU_CIU_Set_ALL_ConfigGure(JP_NVR_SET_CONFIG, CONFIG_NETWORK_TCP_IP, buffer, bufferlen, NULL, 0);
        break;
     case NETWORK_CONFIG_DISPLAY_SCREEN_INFO:
        ret = MPU_CIU_Set_ALL_ConfigGure(JP_NVR_SET_CONFIG, CONFIG_DISPLAY_SCREEN_INFO, buffer, bufferlen, NULL, 0);
        break;
     case NETWORK_CONFIG_DISPLAY_SCREEN_CALIBRATION_GUN:
        ret = MPU_CIU_Set_ALL_ConfigGure(JP_NVR_SET_CONFIG, CONFIG_DISPLAY_SCREEN_CALIBRATION_GUN, buffer, bufferlen, NULL, 0);
        break;
     case NETWORK_CONFIG_DISPLAY_SCREEN_RED_DOT_INFO:
        ret = MPU_CIU_Set_ALL_ConfigGure(JP_NVR_SET_CONFIG, CONFIG_DISPLAY_SCREEN_RED_DOT_INFO, buffer, bufferlen, NULL, 0);
        break;
     case NETWORK_CONFIG_OTHER_INFO:
        ret = MPU_CIU_Set_ALL_ConfigGure(JP_NVR_SET_CONFIG, CONFIG_OTHER_INFO, buffer, bufferlen, NULL, 0);
        break;
     case NETWORK_CONFIG_OTHER_PREVIEW:
        ret = MPU_CIU_Set_ALL_ConfigGure(JP_NVR_SET_CONFIG, CONFIG_OTHER_PREVIEW, buffer, bufferlen, NULL, 0);
        break;
     case NETWORK_CONFIG_OTHER_POSITION:
        ret = MPU_CIU_Set_ALL_ConfigGure(JP_NVR_SET_CONFIG, CONFIG_OTHER_POSITION, buffer, bufferlen, NULL, 0);
        break;
    default:
        ERROR(" the prarm is not support \n");
        ret = -1;
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

