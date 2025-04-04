
#include "CCInfraredImage.h"
#include "Cserial.h"
#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include "json_pack.hpp"
#include "sdk_log.h"
#include "MessageManager.h"
#include "network_common.h"
#define SERIAL_BUFFER_LEN  8


#define IR_CONFIG_SECTION       "infrareImage"
// void *CCInfraredImage::g_threadImage(void *lpParameter)
// {
//     int ret;
//     prctl(PR_SET_NAME, "g_threadImage", 0, 0, 0);
//     ThreadInfo_t* pParent = (ThreadInfo_t*)lpParameter;
//     char buf[SERIAL_BUFFER_LEN];
//     while(1)
//     {
//         if (1 == pParent->m_Exit){
//             break;
//         }
//         ret = m_serial.UartRead(buf, SERIAL_BUFFER_LEN, READ_SERIAL_MS);
//         if(ret > 0)
//         {

//         }
//         usleep(100*1000);
//     }
// }
std::map<int, std::string> pseudo_enum = {
    {NETWORK_PSEUDO_COLOR_WHITE_HOT, "white_hot"},
    {NETWORK_PSEUDO_COLOR_BLACK_HOT, "black_hot"},
    {NETWORK_PSEUDO_COLOR_IRON_RED, "ironbow_forward"},
    {NETWORK_PSEUDO_COLOR_IRONBOW_REVERSE, "ironbow_reverse"},
    {NETWORK_PSEUDO_COLOR_LAVA_FORWARD, "lava_forward"},
    {NETWORK_PSEUDO_COLOR_LAVA_REVERSE, "lava_reverse"},
    {NETWORK_PSEUDO_COLOR_RAINBOW, "rainbow_forward"},
    {NETWORK_PSEUDO_COLOR_RAINBOW_REVERSE, "rainbow_reverse"},
    {NETWORK_PSEUDO_COLOR_RAINBOWHC_FORWARD, "rainbowhc_forward"},
    {NETWORK_PSEUDO_COLOR_RAINBOWHC_REVERSE, "rainbowhc_reverse"}
};

int CCInfraredImage::LoadParam()
{
    int value;
    value = m_cconfig->GetValue(IR_CONFIG_SECTION,"birgthness",(long)50);
    SetImageBrightness(value);

    value = m_cconfig->GetValue(IR_CONFIG_SECTION,"Contrast",(long)50);
    SetImageContrast(value);

    // value = m_cconfig->GetValue(IR_CONFIG_SECTION,"Sharpening",(long)0);
    // SetInfraredImageSharpening(value);

    value = m_cconfig->GetValue(IR_CONFIG_SECTION,"Polarity",(long)0);
    SetInfraredImagePolarity(value);

    // value = m_cconfig->GetValue(IR_CONFIG_SECTION,"saturation",(long)0);
    // SetImagesaturation(value);

    m_ElectronicZoom = m_cconfig->GetValue(IR_CONFIG_SECTION,"electron_zoom",(long)1);
    SetInfraredImageElectronicZoom(m_ElectronicZoom);

    m_FocusMode = m_cconfig->GetValue(IR_CONFIG_SECTION,"focus_mode",(long)0);
    SetInfraredImageFocusMode(m_FocusMode);

    m_GasEnhanced = m_cconfig->GetValue(IR_CONFIG_SECTION,"gas_enhanced",(long)1);
    SetGasEnhanced(m_GasEnhanced);

    return 0;
}

CCInfraredImage::CCInfraredImage(void *handle, int ch)
{
    m_serial = NULL;
    m_init = 0;
    //Comm_CreateThread(&m_serialThread, 0, g_threadImage);
    m_han = handle;
    memset(&m_CtrlFnxCb, 0, sizeof(IRControlFunctions));
}

int CCInfraredImage::Init()
{
    if(m_init == 1)
    {
        return -1;
    }
    int ret = 0;
    pthread_mutex_init(&m_Lock, NULL);
    m_serial = new Cserial(G_UART_DEV5, 115200);
    ret = m_serial->init();
    if(ret != 0)
    {
        return -1;
    }
    m_cconfig = CConfig::GetInstance();;
    m_init = 1;

    //LoadParam();
    
    return 0;
}

int CCInfraredImage::UnInit()
{
     if(m_serial)
    {
        delete m_serial;
        m_serial = NULL;
    }
    m_init = 0;
    return 0;
}

CCInfraredImage::~CCInfraredImage()
{
   UnInit();
}

int CCInfraredImage::SetImageBrightness(int value)
{
    if(!m_init)
    {
        return -1;
    }
    int ret = 0;
    pthread_mutex_lock(&m_Lock);
    if(m_brightness == value)
    {
        pthread_mutex_unlock(&m_Lock);
        return 0;
    }
    if(m_CtrlFnxCb.IR_SetPicbrightness == NULL)
    {
        ERROR("set pic IR_SetPicbrightness cb is NULL");
        pthread_mutex_unlock(&m_Lock);
        return -1;
    }
#if 0
    int ret = 0;
    JsonConfigExt info(_Code(IR_PIC_BRIGHTNESS_CODE, "brightness_ir"), "set");
    info.data = DataConfigBody();
    info.data->value = std::to_string(value);  
    std::string json_data;
    JsonPackData<JsonConfigExt>(info, json_data);
    MessageManager *msghandle = MessageManager::getInstance();
    std::shared_ptr<receMessage> out_msg;
    ret = msghandle->MSG_SendMessage(0, IR_PIC_BRIGHTNESS_CODE, json_data, 1, MQTTMSGTIMEOUT, out_msg);
    if(ret != IR_PIC_BRIGHTNESS_CODE)
    {
        ERROR("get reply message is err \n");
        pthread_mutex_unlock(&m_Lock);
        return -1;
    }

    std::cout<<"recv ir brightness = " <<out_msg->recv_data<<std::endl;
    DataConfigResponse out_response;
   
    JsonParseData<DataConfigResponse>(out_response, out_msg->recv_data);
    if(std::stoi(out_response.status.value()) != 0)
    {
        ERROR(" reply message is status = %s \n", out_response.status->c_str());
        pthread_mutex_unlock(&m_Lock);
        return -1;
    }
#endif
    
    
    ret = m_CtrlFnxCb.IR_SetPicbrightness(value);
    if(ret != 0)
    {
        ERROR("set IR_SetPicContrast is fail ret = %d \n", ret);
        pthread_mutex_unlock(&m_Lock);
        return -1;
    }
    
    m_cconfig->SetValue(IR_CONFIG_SECTION,"birgthness",(long)value);
    m_brightness = value;
    
    pthread_mutex_unlock(&m_Lock);
    return 0;
}

int CCInfraredImage::GetImageBrightness(int *value)
{
    if(!m_init)
    {
        return -1;
    }

    pthread_mutex_lock(&m_Lock);
    *value = m_brightness;
    pthread_mutex_unlock(&m_Lock);
    return 0;
}

int CCInfraredImage::SetImageContrast(int value)
{
    if(!m_init)
    {
        return -1;
    }
    int ret = 0;
    pthread_mutex_lock(&m_Lock);
    if(m_contrast == value)
    {
        pthread_mutex_unlock(&m_Lock);
        return 0;
    }
    if(m_CtrlFnxCb.IR_SetPicContrast == NULL)
    {
        ERROR("set pic IR_SetPicContrast cb is NULL");
        pthread_mutex_unlock(&m_Lock);
        return -1;
    }
#if 0
    int ret = 0;
    JsonConfigExt info(_Code(IR_PIC_CONTRAST_CODE, "contrast_ir"), "set");
    info.data = DataConfigBody();
    info.data->value = std::to_string(value);  
    std::string json_data;
    JsonPackData<JsonConfigExt>(info, json_data);
    MessageManager *msghandle = MessageManager::getInstance();
    std::shared_ptr<receMessage> out_msg;
    ret = msghandle->MSG_SendMessage(0, IR_PIC_CONTRAST_CODE, json_data, 1, MQTTMSGTIMEOUT, out_msg);
    if(ret != IR_PIC_CONTRAST_CODE)
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
    
    
    ret = m_CtrlFnxCb.IR_SetPicContrast(value);
    if(ret != 0)
    {
        ERROR("set IR_SetPicContrast is fail ret = %d \n", ret);
        pthread_mutex_unlock(&m_Lock);
        return -1;
    }
    
    m_cconfig->SetValue(IR_CONFIG_SECTION,"Contrast",(long)value);
    m_contrast = value;
    pthread_mutex_unlock(&m_Lock);
    return 0;
}

int CCInfraredImage::GetImageContrast(int *value)
{
    if(!m_init)
    {
        return -1;
    }
    pthread_mutex_lock(&m_Lock);
    *value = m_contrast;
    pthread_mutex_unlock(&m_Lock);
    return 0;
}

int CCInfraredImage::SetImagesaturation(int value)
{
    if(!m_init)
    {
        return -1;
    }
    pthread_mutex_lock(&m_Lock);
    if(m_saturation == value)
    {
        pthread_mutex_unlock(&m_Lock);
        return 0;
    }
    m_cconfig->SetValue(IR_CONFIG_SECTION,"saturation",(long)value);
    m_saturation = value;

    pthread_mutex_unlock(&m_Lock);
    return 0;
}

int CCInfraredImage::GetImagesaturation(int *value)
{
    pthread_mutex_lock(&m_Lock);
    *value = m_saturation;
    pthread_mutex_unlock(&m_Lock);
    return 0;
}

int CCInfraredImage::SetHotspotTracking(int value)
{
    if(!m_init)
    {
        return -1;
    }
    pthread_mutex_lock(&m_Lock);
    int ret = 0;
    char buf[SERIAL_BUFFER_LEN]= {0};
    buf[0] = 0xAA;
    buf[1] = 0xB3;
    buf[2] = value;

    buf[7] = (buf[1] + buf[2] + buf[3] + buf[4] + buf[5] + buf[6])  & 0xff;
    ret = m_serial->UartWrite(buf, SERIAL_BUFFER_LEN);
    if(ret != 0){
        pthread_mutex_unlock(&m_Lock);
        ERROR("write is err\n");
        return -1;
    }
    m_cconfig->SetValue(IR_CONFIG_SECTION,"HotspotTracking_enable",(long)value);
    memset(buf, 0, SERIAL_BUFFER_LEN);
    ret = m_serial->UartRead(buf,SERIAL_BUFFER_LEN,READ_SERIAL_MS);
    if(ret > 0)
    {
        if(buf[0] == 0x55 && buf[1] == 0XC5)
        {
            pthread_mutex_unlock(&m_Lock);
            return 0;
        }
        
    }
    pthread_mutex_unlock(&m_Lock);
    return 0;
}

int CCInfraredImage::GetHotspotTracking(int *value)
{
    if(!m_init)
    {
        return -1;
    }
    pthread_mutex_lock(&m_Lock);
    *value = m_cconfig->GetValue(IR_CONFIG_SECTION,"HotspotTracking_enable",(long)0);
    pthread_mutex_unlock(&m_Lock);
    return 0;
}

int CCInfraredImage::ManualDefectRemoval(int opt, int value)
{
    if(!m_init)
    {
        return -1;
    }
    int ret  = 0;

    pthread_mutex_lock(&m_Lock);
    char cmd[SERIAL_BUFFER_LEN]= {0};
    switch (opt)
    {
    case 0:
        cmd[1] = 0xc4;
        cmd[2] = value;
        m_cconfig->SetValue(IR_CONFIG_SECTION,"Removal_threshold",(long)value);
        break;
    case 1:
        cmd[1] = 0xb8;
        cmd[2] = value;
        //m_cconfig->SetValue(IR_CONFIG_SECTION,"Removal_enable",(long)value);
        break;
    case 2:
        cmd[1] = 0xb9;
        cmd[2] = value;
        //m_cconfig->SetValue(IR_CONFIG_SECTION,"Removal_enable",(long)0);
        break;
    case 3:
        cmd[1] = 0xba;
        cmd[2] = value;
        //m_cconfig->SetValue(IR_CONFIG_SECTION,"Removal_save",(long)value);
        break;
    
    default:
        break;
    }
    
    cmd[7] = (cmd[1] + cmd[2] + cmd[3] + cmd[4] + cmd[5] + cmd[6]) & 0xff;
    ret = m_serial->UartWrite(cmd, SERIAL_BUFFER_LEN);
    if(ret != 0){
        pthread_mutex_unlock(&m_Lock);
        ERROR("write is err\n");
        return -1;
    }
    memset(cmd, 0, SERIAL_BUFFER_LEN);
    ret = m_serial->UartRead(cmd,SERIAL_BUFFER_LEN,READ_SERIAL_MS);
    if(ret > 0)
    {
        if(cmd[0] == 0x55 && cmd[1] == 0XC5)
        {
            pthread_mutex_unlock(&m_Lock);
            return 0;
        }
    }
    pthread_mutex_unlock(&m_Lock);
    return 0;
}

int CCInfraredImage::GetManualDefectRemoval(int *opt, int *value)
{
    if(!m_init)
    {
        return -1;
    }
    pthread_mutex_lock(&m_Lock);
    *value = m_cconfig->GetValue(IR_CONFIG_SECTION,"Removal_threshold",(long)20);
    pthread_mutex_unlock(&m_Lock);
    return 0;
}

int CCInfraredImage::SetPictureInPictureSwitch(int value)
{
    return 0;
}

int CCInfraredImage::GetPictureInPictureSwitch(int *value)
{
    return 0;
}

int CCInfraredImage::SetInfraredImageSharpening(int ddelv)
{
    if(!m_init)
    {
        return -1;
    }
    int ret;
    pthread_mutex_lock(&m_Lock);
    if(m_sharp == ddelv)
    {
        pthread_mutex_unlock(&m_Lock);
        return 0;
    }
    m_cconfig->SetValue(IR_CONFIG_SECTION,"Sharpening",(long)ddelv);
    m_sharp = ddelv;
    pthread_mutex_unlock(&m_Lock);
    return 0;
}

int CCInfraredImage::GetInfraredImageSharpening(int *ddelv)
{
    if(!m_init)
    {
        return -1;
    }
    pthread_mutex_lock(&m_Lock);
    *ddelv = m_sharp;
    pthread_mutex_unlock(&m_Lock);
    return 0;
}

int CCInfraredImage::SetInfraredImagePolarity(int value)
{
    if(!m_init)
    {
        return -1;
    }
    int ret;
    pthread_mutex_lock(&m_Lock);
    if(m_pseudo == value)
    {
        pthread_mutex_unlock(&m_Lock);
        return 0;
    }
    if(m_CtrlFnxCb.IR_SetPicPreMode == NULL)
    {
        ERROR("set pic IR_SetPicPreMode cb is NULL");
        pthread_mutex_unlock(&m_Lock);
        return -1;
    }
#if 0
    JsonConfigExt info(_Code(IR_PSEUDO_CONFIG_CODE, "pseudo"), "set");
    info.data = DataConfigBody();
    info.data->value = pseudo_enum[value]; 
    std::string json_data;
    JsonPackData<JsonConfigExt>(info, json_data);
    MessageManager *msghandle = MessageManager::getInstance();
    std::shared_ptr<receMessage> out_msg;
    ret = msghandle->MSG_SendMessage(0, IR_PSEUDO_CONFIG_CODE, json_data, 1, MQTTMSGTIMEOUT, out_msg);
    if(ret != IR_PSEUDO_CONFIG_CODE)
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
    ret = m_CtrlFnxCb.IR_SetPicPreMode(value);
    if(ret != 0)
    {
        ERROR("set IR_SetPicPreMode is fail ret = %d \n", ret);
        pthread_mutex_unlock(&m_Lock);
        return -1;
    }
    
    m_cconfig->SetValue(IR_CONFIG_SECTION,"Polarity",(long)value);
    m_pseudo = value;
    
    pthread_mutex_unlock(&m_Lock);
    return 0;
}

int CCInfraredImage::GetInfraredImagePolarity(int *value)
{
    if(!m_init)
    {
        return -1;
    }
    pthread_mutex_lock(&m_Lock);
    *value = m_pseudo;
    pthread_mutex_unlock(&m_Lock);
    return 0;
}

int CCInfraredImage::InfraredImageSaveCmd()
{
    if(!m_init)
    {
        return -1;
    }
    int ret;
    char cmd[8] = {
        0,
    };
    pthread_mutex_lock(&m_Lock);
    cmd[0] = 0xaa;
    cmd[1] = 0xc1;
    cmd[2] = 0x01;
    cmd[7] = (cmd[1] + cmd[2] + cmd[3] + cmd[4] + cmd[5] + cmd[6]) & 0xff;
    ret = m_serial->UartWrite(cmd, SERIAL_BUFFER_LEN);
    sleep(3);
    pthread_mutex_unlock(&m_Lock);
    return ret;
}

int CCInfraredImage::SetInfraredImagePAL(int status)
{
    if(!m_init)
    {
        return -1;
    }
    int ret = 0;
    char cmd[SERIAL_BUFFER_LEN] = {
        0,
    };
    pthread_mutex_lock(&m_Lock);
    cmd[0] = 0xaa;
    cmd[1] = 0xc6;
    cmd[2] = status;
    cmd[7] = (cmd[1] + cmd[2] + cmd[3] + cmd[4] + cmd[5] + cmd[6]) & 0xff;
    ret = m_serial->UartWrite(cmd, SERIAL_BUFFER_LEN);
    if(ret != 0){
        pthread_mutex_unlock(&m_Lock);
        ERROR("write is err\n");
        return -1;
    }
    m_cconfig->SetValue(IR_CONFIG_SECTION,"PAL_status",(long)status);
    memset(cmd, 0, SERIAL_BUFFER_LEN);
    ret = m_serial->UartRead(cmd,SERIAL_BUFFER_LEN,READ_SERIAL_MS);
    if(ret > 0)
    {
        if(cmd[0] == 0x55 && cmd[1] == 0XC5)
        {
            pthread_mutex_unlock(&m_Lock);
            return 0;
        }
        
    }
    pthread_mutex_unlock(&m_Lock);
    return 0;
}

int CCInfraredImage::GetInfraredImagePAL(int *status)
{
    if(!m_init)
    {
        return -1;
    }
    pthread_mutex_lock(&m_Lock);
    *status = m_cconfig->GetValue(IR_CONFIG_SECTION,"PAL_status",(long)0);
    pthread_mutex_unlock(&m_Lock);
    return 0;
}

int CCInfraredImage::SetInfraredImageElectronicZoom(float value)
{
    if(!m_init)
    {
        return -1;
    }
    pthread_mutex_lock(&m_Lock);
    int ret;
    if(m_ElectronicZoom == value)
    {
        pthread_mutex_unlock(&m_Lock);
        return 0;
    }
    if(m_CtrlFnxCb.IR_SetPicDigitalZoom == NULL)
    {
        ERROR("set pic IR_SetPicDigitalZoom cb is NULL");
        pthread_mutex_unlock(&m_Lock);
        return -1;
    }
#if 0
    JsonConfigExt info(_Code(IR_ZOOM_CONFIG_CODE, "zoom"), "set");
    info.data = DataConfigBody();
    info.data->value = std::to_string(value);  
    std::string json_data;
    JsonPackData<JsonConfigExt>(info, json_data);
    MessageManager *msghandle = MessageManager::getInstance();
    std::shared_ptr<receMessage> out_msg;
    ret = msghandle->MSG_SendMessage(0, IR_ZOOM_CONFIG_CODE, json_data, 1, MQTTMSGTIMEOUT, out_msg);
    if(ret != IR_ZOOM_CONFIG_CODE)
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
    ret = m_CtrlFnxCb.IR_SetPicDigitalZoom(value);
    if(ret != 0)
    {
        ERROR("set IR_SetPicDigitalZoom is fail ret = %d \n", ret);
        pthread_mutex_unlock(&m_Lock);
        return -1;
    }
    
    m_cconfig->SetValue(IR_CONFIG_SECTION,"electron_zoom",(float)value);

    m_ElectronicZoom = value;
    pthread_mutex_unlock(&m_Lock);
    
    return 0;
}

int CCInfraredImage::GetInfraredImageElectronicZoom(float *value)
{
    if(!m_init)
    {
        return -1;
    }
    pthread_mutex_lock(&m_Lock);
    *value = m_ElectronicZoom;

    pthread_mutex_unlock(&m_Lock);
    return 0;
}

int CCInfraredImage::SetInfraredImageFocusMode(int mode)
{
    if(!m_init)
    {
        return -1;
    }
    pthread_mutex_lock(&m_Lock);
    int ret = 0;
    int enable = 0;
    int msg_flag = 0;
    if(m_FocusMode == mode)
    {
        pthread_mutex_unlock(&m_Lock);
        return 0;
    }
    if(m_CtrlFnxCb.IR_SetAfocesMode == NULL)
    {
        ERROR("set pic IR_SetAfocesMode cb is NULL");
        pthread_mutex_unlock(&m_Lock);
        return -1;
    }
#if 0
    if(mode == 0)       //0 自动调焦 1 电动调焦
    {
        msg_flag = 1;
        enable= 1;
    }
    else if(m_FocusMode == 0 && mode != 0)
    {
        msg_flag = 1;
        enable = 0;
    }
    if(msg_flag)
    {
        JsonConfigExt info(_Code(IR_AUTO_FOCU_CODE, "focusmode_ir"), "set");
        info.data = DataConfigBody();
        info.data->value = std::to_string(enable); 
        info.data->type = "continue";
        std::string json_data;
        JsonPackData<JsonConfigExt>(info, json_data);
        MessageManager *msghandle = MessageManager::getInstance();
        std::shared_ptr<receMessage> out_msg;
        ret = msghandle->MSG_SendMessage(0, IR_AUTO_FOCU_CODE, json_data, 1, MQTTMSGTIMEOUT, out_msg);
        if(ret != IR_AUTO_FOCU_CODE)
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
    }
#endif 
    ret = m_CtrlFnxCb.IR_SetAfocesMode(mode);
    if(ret != 0)
    {
        ERROR("set IR_SetAfocesMode is fail ret = %d \n", ret);
        pthread_mutex_unlock(&m_Lock);
        return -1;
    }
    m_cconfig->SetValue(IR_CONFIG_SECTION,"focus_mode",(long)mode);
    m_FocusMode = mode;
    pthread_mutex_unlock(&m_Lock);
    return 0;
}

int CCInfraredImage::GetInfraredImageFocusMode(int *mode)
{
    if(!m_init)
    {
        return -1;
    }
    pthread_mutex_lock(&m_Lock);
    *mode = m_FocusMode;
    pthread_mutex_unlock(&m_Lock);
    return 0;
}

int CCInfraredImage::SetGasEnhanced(int enable)
{
    if(!m_init)
    {
        return -1;
    }
    int ret = 0;
    pthread_mutex_lock(&m_Lock);
    if(enable == m_GasEnhanced)
    {
        pthread_mutex_unlock(&m_Lock);
        return 0;
    }
    if(m_CtrlFnxCb.IR_SetGasEnhanced == NULL)
    {
        ERROR("set pic IR_SetGasEnhanced cb is NULL");
        pthread_mutex_unlock(&m_Lock);
        return -1;
    }
#if 0
    JsonConfigExt info(_Code(GAS_ENHANCEMENT_CODE, "gas_enhancement"), "set");
    info.data = DataConfigBody();
    info.data->value = std::to_string(enable);  
    std::string json_data;
    JsonPackData<JsonConfigExt>(info, json_data);
    MessageManager *msghandle = MessageManager::getInstance();
    std::shared_ptr<receMessage> out_msg;
    ret = msghandle->MSG_SendMessage(0, GAS_ENHANCEMENT_CODE, json_data, 1, MQTTMSGTIMEOUT, out_msg);
    if(ret != GAS_ENHANCEMENT_CODE)
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
    ret = m_CtrlFnxCb.IR_SetGasEnhanced(enable);
    if(ret != 0)
    {
        ERROR("set IR_SetGasEnhanced is fail ret = %d \n", ret);
        pthread_mutex_unlock(&m_Lock);
        return -1;
    }
    m_cconfig->SetValue(IR_CONFIG_SECTION,"gas_enhanced",(long)enable);
    m_GasEnhanced = enable;
    pthread_mutex_unlock(&m_Lock);
    return 0;
}

int CCInfraredImage::GetGasEnhanced(int *enable)
{
    if(!m_init)
    {
        return -1;
    }
    pthread_mutex_lock(&m_Lock);
    *enable = m_GasEnhanced;
    pthread_mutex_unlock(&m_Lock);
    return 0;
}

int CCInfraredImage::SetElectricFocu(int action)
{
    if(!m_init)
    {
        return -1;
    }
    int ret = 0;
    pthread_mutex_lock(&m_Lock);
    if(m_CtrlFnxCb.IR_SetAfocesStatus == NULL)
    {
        ERROR("set pic IR_SetAfocesStatus cb is NULL");
        pthread_mutex_unlock(&m_Lock);
        return -1;
    }
#if 0
    JsonConfigExt info(_Code(IR_ELEC_FOCU_DATA_CODE, "elecfocus_ir"), "set");
    info.data = DataConfigBody();
    info.data->action = std::to_string(action); 
    std::string json_data;
    JsonPackData<JsonConfigExt>(info, json_data);
    MessageManager *msghandle = MessageManager::getInstance();
    std::shared_ptr<receMessage> out_msg;
    ret = msghandle->MSG_SendMessage(0, IR_ELEC_FOCU_DATA_CODE, json_data, 1, MQTTMSGTIMEOUT, out_msg);
    if(ret != IR_ELEC_FOCU_DATA_CODE)
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
    ret = m_CtrlFnxCb.IR_SetAfocesStatus(1);
    if(ret != 0)
    {
        ERROR("set IR_SetAfocesStatus is fail ret = %d \n", ret);
        pthread_mutex_unlock(&m_Lock);
        return -1;
    }
    pthread_mutex_unlock(&m_Lock);
    return 0;
}

int CCInfraredImage::SetAutoFocuData(int type)
{

    if(!m_init)
    {
        return -1;
    }
    int ret = 0;
    std::string action;
    pthread_mutex_lock(&m_Lock);
    if(m_CtrlFnxCb.IR_SetAfocesStatus == NULL)
    {
        ERROR("set pic IR_SetAfocesStatus cb is NULL");
        pthread_mutex_unlock(&m_Lock);
        return -1;
    }
#if 0 
    if(type == 0)
    {
        action = "single";
    }
    else
    {
        action = "continue";
    }
    JsonConfigExt info(_Code(IR_AUTO_FOCU_DATA_CODE, "autofocus_ir"), "set");
    info.data = DataConfigBody();
    info.data->type = action; 
    std::string json_data;
    JsonPackData<JsonConfigExt>(info, json_data);
    MessageManager *msghandle = MessageManager::getInstance();
    std::shared_ptr<receMessage> out_msg;
    ret = msghandle->MSG_SendMessage(0, IR_AUTO_FOCU_DATA_CODE, json_data, 1, MQTTMSGTIMEOUT, out_msg);
    if(ret != IR_AUTO_FOCU_DATA_CODE)
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

    ret = m_CtrlFnxCb.IR_SetAfocesStatus(type);
    if(ret != 0)
    {
        ERROR("set IR_SetAfocesStatus is fail ret = %d \n", ret);
        pthread_mutex_unlock(&m_Lock);
        return -1;
    }
    
    pthread_mutex_unlock(&m_Lock);
    return 0;
}

int CCInfraredImage::RegisterIrCtrlCb(IRControlFunctions cb)
{
    pthread_mutex_lock(&m_Lock);
    memcpy(&m_CtrlFnxCb, &cb, sizeof(IRControlFunctions));
    pthread_mutex_unlock(&m_Lock);
    LoadParam();
    return 0;
}
