#include "CVisLightImage.h"
#include "MessageManager.h"
#include "json_pack.hpp"
#include "sdk_log.h"
#include "common.h"
#define VIS_LIGHT_SECTION_CFG   "vis_light_section_cfg"

CVisLightImage::CVisLightImage(void *handle, int ch)
{
    m_init = 0;
    m_brightness = 0;
    m_contrast = 0;
    m_focu_mode = -1;
}

CVisLightImage::~CVisLightImage()
{
    UnInit();
}

int CVisLightImage::LoadParam()
{
    int brightness;
    int contrast;
    int focu_mode;
    int saturation;
    int sharpness;
    float digitalzoom;

    brightness = m_cconfig->GetValue(VIS_LIGHT_SECTION_CFG, "brightness", (long)5);
    SetBrightness(brightness);

    contrast = m_cconfig->GetValue(VIS_LIGHT_SECTION_CFG, "contrast", (long)5);
    SetContrast(contrast);

    focu_mode = m_cconfig->GetValue(VIS_LIGHT_SECTION_CFG, "focu_mode", (long)0);
    SetFocuMode(focu_mode);

    saturation = m_cconfig->GetValue(VIS_LIGHT_SECTION_CFG, "saturation", (long)5);
    SetSaturation(saturation);

    sharpness = m_cconfig->GetValue(VIS_LIGHT_SECTION_CFG, "sharpness", (long)5);
    SetSharpness(sharpness);

    digitalzoom = m_cconfig->GetValue(VIS_LIGHT_SECTION_CFG,"electron_zoom",(float)1);

    SetDigitalZoom(digitalzoom);
    return 0;
}
int CVisLightImage::Init()
{
    m_cconfig = CConfig::GetInstance();
    pthread_mutex_init(&m_Lock, NULL);
    m_serial = new Cserial("/dev/ttyS7", 9600);
    if(m_serial->init() != 0)
    {
        return -1;
    }
    m_init = 1;
    
    //LoadParam();
   
    
    return 0;
}

int CVisLightImage::UnInit()
{
    m_init = 0;
    return 0;
}

int CVisLightImage::SetBrightness(int value)
{
    if(!m_init)
    {
        ERROR("vis light is not init\n");
        return -1;
    }
    int ret = 0;
    unsigned char data[9] = {0x81, 0x01, 0x09, 0x8c, 0x00, 0x00, 0x00, 0x00, 0xFF};
    pthread_mutex_lock(&m_Lock);
    if(value == m_brightness)
    {
        pthread_mutex_unlock(&m_Lock);
        return 0;
    }
    data[7] = value & 0xff;

    m_serial->UartWrite(data, 9);

    m_brightness = value;
    m_cconfig->SetValue(VIS_LIGHT_SECTION_CFG,"brightness",(long)value);
    
    pthread_mutex_unlock(&m_Lock);
    return 0;
}

int CVisLightImage::GetBrightness(int *value)
{
    if(!m_init)
    {
        ERROR("vis light is not init\n");
        return -1;
    }
    pthread_mutex_lock(&m_Lock);
    *value = m_brightness;
    pthread_mutex_unlock(&m_Lock);
    return 0;
}

int CVisLightImage::SetContrast(int value)
{
    if(!m_init)
    {
        ERROR("vis light is not init\n");
        return -1;
    }
    int ret = 0;
    unsigned char data[9] = {0x81, 0x01, 0x09, 0x8D, 0x00, 0x00, 0x00, 0x00, 0xFF};
    pthread_mutex_lock(&m_Lock);
    if(value == m_contrast)
    {
        pthread_mutex_unlock(&m_Lock);
        return 0;
    }
    
    data[7] = value & 0xff;

    m_serial->UartWrite(data, 9);

    m_contrast = value;
    m_cconfig->SetValue(VIS_LIGHT_SECTION_CFG,"contrast",(long)m_contrast);
    
    pthread_mutex_unlock(&m_Lock);
    return 0;
}

int CVisLightImage::GetContrast(int *value)
{
    if(!m_init)
    {
        ERROR("vis light is not init\n");
        return -1;
    }
    pthread_mutex_lock(&m_Lock);
    *value = m_contrast;
    pthread_mutex_unlock(&m_Lock);
    return 0;
}

int CVisLightImage::SetFocuMode(int mode)
{
    if(!m_init)
    {
        return -1;
    }
    int ret = 0;
    int enable;
    pthread_mutex_lock(&m_Lock);
    if(mode == m_focu_mode)
    {
        pthread_mutex_unlock(&m_Lock);
        return 0;
    }
    if(m_CtrlFnxCb.Vis_SetAfocesMode == NULL)
    {
        ERROR("set pic Vis_SetAfocesMode cb is NULL");
        pthread_mutex_unlock(&m_Lock);
        return -1;
    }
#if 0
    if(m_focu_mode == 0)    //0 自动调焦
    {
        enable = 1;
    }
    else
    {
        enable = 0;
    }
    JsonConfigExt info(_Code(VIS_AUTO_FOCU_CODE, "focusmode_vis"), "set");
    info.data = DataConfigBody();
    info.data->value = std::to_string(enable); 
    info.data->type = "continue";
    std::string json_data;
    JsonPackData<JsonConfigExt>(info, json_data);
    MessageManager *msghandle = MessageManager::getInstance();
    std::shared_ptr<receMessage> out_msg;
    ret = msghandle->MSG_SendMessage(0, VIS_AUTO_FOCU_CODE, json_data, 1, MQTTMSGTIMEOUT, out_msg);
    if(ret != VIS_AUTO_FOCU_CODE)
    {
        ERROR("get reply message is err \n");
        pthread_mutex_unlock(&m_Lock);
        return -1;
    }
    DataConfigResponse out_response;
    JsonParseData<DataConfigResponse>(out_response, out_msg->recv_data);
    if(std::stoi(out_response.status.value()) != 0)
    {
        ERROR(" reply message is status = %s \n", out_response.status->c_str());
        pthread_mutex_unlock(&m_Lock);
        return -1;
    }
#endif
    ret = m_CtrlFnxCb.Vis_SetAfocesMode(mode);
    if(ret != 0)
    {
        ERROR("set Vis_SetAfocesMode is fail ret = %d \n", ret);
        pthread_mutex_unlock(&m_Lock);
        return -1;
    }

    m_focu_mode = mode;
    m_cconfig->SetValue(VIS_LIGHT_SECTION_CFG,"focu_mode",(long)m_focu_mode);
     pthread_mutex_unlock(&m_Lock);
    return 0;
}

int CVisLightImage::GetFocuMode(int *mode)
{
    if(!m_init)
    {
        ERROR("vis light is not init\n");
        return -1;
    }
    pthread_mutex_lock(&m_Lock);
    *mode = m_focu_mode;
    pthread_mutex_unlock(&m_Lock);
    return 0;
}

int CVisLightImage::SetAutoFocuData(int mode)
{
    if(!m_init)
    {
        return -1;
    }
    int ret = 0;
    if(m_CtrlFnxCb.Vis_SetAfocesStatus == NULL)
    {
        ERROR("set pic Vis_SetAfocesStatus cb is NULL");
        pthread_mutex_unlock(&m_Lock);
        return -1;
    }

    pthread_mutex_lock(&m_Lock);
    ret = m_CtrlFnxCb.Vis_SetAfocesStatus(mode);
    if(ret != 0)
    {
        ERROR("set Vis_SetAfocesStatus is fail ret = %d \n", ret);
        pthread_mutex_unlock(&m_Lock);
        return -1;
    }
    pthread_mutex_unlock(&m_Lock);
    return 0;
}

int CVisLightImage::SetSaturation(int value)
{
    if(!m_init)
    {
        return -1;
    }
    int ret = 0;
    pthread_mutex_lock(&m_Lock);
    if(value == m_saturation)
    {
        pthread_mutex_unlock(&m_Lock);
        return 0;
    }
    unsigned char data[6] = {0x81, 0x01, 0x09, 0x16, 0x00, 0xFF};

    data[4] = value & 0xff;
   
    m_saturation = value;
    m_cconfig->SetValue(VIS_LIGHT_SECTION_CFG,"saturation",(long)m_saturation);
    pthread_mutex_unlock(&m_Lock);
    return 0;
}

int CVisLightImage::GetSaturation(int *value)
{
    if(!m_init)
    {
        return -1;
    }
    pthread_mutex_lock(&m_Lock);
    *value = m_saturation;
    pthread_mutex_unlock(&m_Lock);
    return 0;
}

int CVisLightImage::SetSharpness(int value)
{
    if(!m_init)
    {
        return -1;
    }
    int ret = 0;
    pthread_mutex_lock(&m_Lock);
    if(value == m_sharpness)
    {
        pthread_mutex_unlock(&m_Lock);
        return 0;
    }
    unsigned char data[9] = {0x81, 0x01, 0x04, 0x42, 0x00, 0x00, 0x00, 0x00, 0xFF};

    data[7] = value & 0xff;

    m_serial->UartWrite(data, 9);

    m_sharpness = value;
    m_cconfig->SetValue(VIS_LIGHT_SECTION_CFG,"sharpness",(long)m_sharpness);
     pthread_mutex_unlock(&m_Lock);
    return 0;
}

int CVisLightImage::GetSharpness(int *value)
{
    if(!m_init)
    {
        return -1;
    }
    pthread_mutex_lock(&m_Lock);
    *value = m_sharpness;
    pthread_mutex_unlock(&m_Lock);

    return 0;
}

int CVisLightImage::SetDigitalZoom(float value)
{
    if(!m_init)
    {
        return -1;
    }
    int ret = 0;
    pthread_mutex_lock(&m_Lock);
    if(m_CtrlFnxCb.Vis_SetPicDigitalZoom == NULL)
    {
        ERROR("set pic Vis_SetPicDigitalZoom cb is NULL");
        pthread_mutex_unlock(&m_Lock);
        return -1;
    }
    ret = m_CtrlFnxCb.Vis_SetPicDigitalZoom(value);
    if(ret != 0)
    {
        ERROR("set Vis_SetPicDigitalZoom is fail ret = %d \n", ret);
        pthread_mutex_unlock(&m_Lock);
        return -1;
    }
    m_cconfig->SetValue(VIS_LIGHT_SECTION_CFG,"electron_zoom",(float)value);
    m_DigitalZoom = value;
    pthread_mutex_unlock(&m_Lock);
    return 0;
}

int CVisLightImage::GetDigitalZoom(float *value)
{
    if(!m_init)
    {
        return -1;
    }
    pthread_mutex_lock(&m_Lock);
    *value = m_DigitalZoom;
    pthread_mutex_unlock(&m_Lock);
    return 0;
}

int CVisLightImage::RegisterVisCtrlCb(VisControlFunctions cb)
{
    pthread_mutex_lock(&m_Lock);
    memcpy(&m_CtrlFnxCb, &cb, sizeof(VisControlFunctions));
    pthread_mutex_unlock(&m_Lock);
    LoadParam();
    return 0;
}
