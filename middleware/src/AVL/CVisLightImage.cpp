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
    m_auto_focu_enable = 0;
}

CVisLightImage::~CVisLightImage()
{
    UnInit();
}

int CVisLightImage::LoadParam()
{
    int brightness;
    int contrast;
    int auto_focu_enable;
    int saturation;
    int sharpness;

    brightness = m_cconfig->GetValue(VIS_LIGHT_SECTION_CFG, "brightness", (long)50);
    SetBrightness(brightness);

    contrast = m_cconfig->GetValue(VIS_LIGHT_SECTION_CFG, "contrast", (long)50);
    SetContrast(contrast);

    auto_focu_enable = m_cconfig->GetValue(VIS_LIGHT_SECTION_CFG, "auto_focu_enable", (long)0);
    SetAutoFocuEnabele(auto_focu_enable);

    saturation = m_cconfig->GetValue(VIS_LIGHT_SECTION_CFG, "saturation", (long)50);
    SetSaturation(saturation);

    sharpness = m_cconfig->GetValue(VIS_LIGHT_SECTION_CFG, "sharpness", (long)50);
    SetSharpness(sharpness);


    return 0;
}
int CVisLightImage::Init()
{
    m_cconfig = CConfig::GetInstance();
    pthread_mutex_init(&m_Lock, NULL);
    LoadParam();
    m_init = 1;
    
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
    pthread_mutex_lock(&m_Lock);
    if(value == m_brightness)
    {
        pthread_mutex_unlock(&m_Lock);
        return 0;
    }
   
    JsonConfigExt info(_Code(VIS_PIC_BRIGHTNESS_CODE, "vis_pic_brightness"), "set");
    info.data = DataConfigBody();
    info.data->value = std::to_string(value);  
    std::string json_data;
    JsonPackData<JsonConfigExt>(info, json_data);
    MessageManager *msghandle = MessageManager::getInstance();
    std::shared_ptr<receMessage> out_msg;
    ret = msghandle->MSG_SendMessage(0, VIS_PIC_BRIGHTNESS_CODE, json_data, 1, 100, out_msg);
    if(ret != VIS_PIC_BRIGHTNESS_CODE)
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
    pthread_mutex_lock(&m_Lock);
    if(value == m_contrast)
    {
        pthread_mutex_unlock(&m_Lock);
        return 0;
    }
    JsonConfigExt info(_Code(VIS_PIC_CONTRAST_CODE, "vis_pic_contrast"), "set");
    info.data = DataConfigBody();
    info.data->value = std::to_string(value);  
    std::string json_data;
    JsonPackData<JsonConfigExt>(info, json_data);
    MessageManager *msghandle = MessageManager::getInstance();
    std::shared_ptr<receMessage> out_msg;
    ret = msghandle->MSG_SendMessage(0, VIS_PIC_CONTRAST_CODE, json_data, 1, 100, out_msg);
    if(ret != VIS_PIC_CONTRAST_CODE)
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

int CVisLightImage::SetAutoFocuEnabele(int enable)
{
    if(!m_init)
    {
        return -1;
    }
    int ret = 0;
    pthread_mutex_lock(&m_Lock);
    if(enable == m_auto_focu_enable)
    {
        pthread_mutex_unlock(&m_Lock);
        return 0;
    }
    JsonConfigExt info(_Code(VIS_AUTO_FOCU_CODE, "autofocus_vis"), "set");
    info.data = DataConfigBody();
    info.data->value = std::to_string(enable); 
    info.data->type = "continue";
    std::string json_data;
    JsonPackData<JsonConfigExt>(info, json_data);
    MessageManager *msghandle = MessageManager::getInstance();
    std::shared_ptr<receMessage> out_msg;
    ret = msghandle->MSG_SendMessage(0, VIS_AUTO_FOCU_CODE, json_data, 1, 100, out_msg);
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

    m_auto_focu_enable = enable;
    m_cconfig->SetValue(VIS_LIGHT_SECTION_CFG,"auto_focu_enable",(long)m_auto_focu_enable);

    return 0;
}

int CVisLightImage::GetAutoFocuEnabele(int *enable)
{
    if(!m_init)
    {
        ERROR("vis light is not init\n");
        return -1;
    }
    pthread_mutex_lock(&m_Lock);
    *enable = m_auto_focu_enable;
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
   
    JsonConfigExt info(_Code(VIS_PIC_STAURATION_CODE, "vis_pic_saturation"), "set");
    info.data = DataConfigBody();
    info.data->value = std::to_string(value); 
    std::string json_data;
    JsonPackData<JsonConfigExt>(info, json_data);
    MessageManager *msghandle = MessageManager::getInstance();
    std::shared_ptr<receMessage> out_msg;
    ret = msghandle->MSG_SendMessage(0, VIS_PIC_STAURATION_CODE, json_data, 1, 100, out_msg);
    if(ret != VIS_PIC_STAURATION_CODE)
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
    m_saturation = value;
    m_cconfig->SetValue(VIS_LIGHT_SECTION_CFG,"saturation",(long)m_saturation);
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
    JsonConfigExt info(_Code(VIS_PIC_SHARPNESS_CODE, "vis_pic_sharpness"), "set");
    info.data = DataConfigBody();
    info.data->value = std::to_string(value); 
    std::string json_data;
    JsonPackData<JsonConfigExt>(info, json_data);
    MessageManager *msghandle = MessageManager::getInstance();
    std::shared_ptr<receMessage> out_msg;
    ret = msghandle->MSG_SendMessage(0, VIS_PIC_SHARPNESS_CODE, json_data, 1, 100, out_msg);
    if(ret != VIS_PIC_SHARPNESS_CODE)
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
    m_sharpness = value;
    m_cconfig->SetValue(VIS_LIGHT_SECTION_CFG,"sharpness",(long)m_sharpness);
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
