#include "CExtctrl.h"
#include "json_pack.hpp"
#include "MessageManager.h"
#include "common.h"
#include "sdk_log.h"
#include "jpsdk.h"

#define EXT_SECITONCFG      "CEXT_cfg"


#define EXT_WIPERSKEY       "WipersEnable"
#define EXT_AUTOLIGHTKEY    "AutoLightEnable"
#define EXT_CVBSKEY         "CvbsEnable"
#define EXT_TEMPERATURE_MAX "MaxTemperature"
#define EXT_TEMPERATURE_MIN "MINTemperature"
#define EXT_TEMPERATURE_MODE "TemperatureMode"
int CExtctrl::LoadParam()
{
    int value;
    TemperatureCfg tem;
    int mode;
    value = m_Cconfig->GetValue(EXT_SECITONCFG, EXT_WIPERSKEY, (long)0);
    SetWipersEnable(value);

    value = m_Cconfig->GetValue(EXT_SECITONCFG, EXT_AUTOLIGHTKEY, (long)0);
    SetAutoLightEnable(value);

    value = m_Cconfig->GetValue(EXT_SECITONCFG, EXT_CVBSKEY, (long)0);
    SetCvbsEnable(value);
    tem.min_temperature = m_Cconfig->GetValue(EXT_SECITONCFG, EXT_TEMPERATURE_MIN, (long)30);
    tem.max_temperature = m_Cconfig->GetValue(EXT_SECITONCFG, EXT_TEMPERATURE_MAX, (long)40);
    SetTemCtrlCfg(tem);

    mode = m_Cconfig->GetValue(EXT_SECITONCFG, EXT_TEMPERATURE_MODE, (long)2);
    SetTemperatureMode(mode);
    return 0;
}

CExtctrl::CExtctrl(void *handle, int ch)
{
    m_WipersEnable = 0;
    m_AutoLightEnable = 0;
    m_CvbsEnable = 0;
    m_init = 0;
    m_Cconfig = NULL;
    m_handle = handle;
    m_ch = ch;
}

CExtctrl::~CExtctrl()
{
    UnInlt();
}

int CExtctrl::Init()
{
    m_Cconfig = CConfig::GetInstance();
    pthread_mutex_init(&m_Lock, NULL);
    m_init = 1;

    LoadParam();
    return 0;
}

int CExtctrl::UnInlt()
{
    return 0;
}

int CExtctrl::SetWipersEnable(int enable)
{
    if(!m_init)
    {
        return -1;
    }
    pthread_mutex_lock(&m_Lock);
    if(enable == m_WipersEnable)
    {
        pthread_mutex_unlock(&m_Lock);
        return 0;
    }
    int ret = 0;
    m_WipersEnable = enable;

    //JPSys_SetWiperEnable(enable);
    
    CConfig *pcfg = CConfig::GetInstance();
    m_Cconfig->SetValue(EXT_SECITONCFG, EXT_WIPERSKEY, (long)enable);

    pthread_mutex_unlock(&m_Lock);
    return 0;
}

int CExtctrl::GetWipersEnable(int *enable)
{
    pthread_mutex_lock(&m_Lock);
    *enable = m_WipersEnable;
    pthread_mutex_unlock(&m_Lock);
    return 0;
}

int CExtctrl::SetAutoLightEnable(int enable)
{
    if(!m_init)
    {
        return -1;
    }
    pthread_mutex_lock(&m_Lock);
    if(enable == m_AutoLightEnable)
    {
        pthread_mutex_unlock(&m_Lock);
        return 0;
    }
    int ret = 0;
    m_AutoLightEnable = enable;
    if(m_AutoLightEnable == 2)  //自动检测模式
    {
        //JPSys_SetLightMode(2);
    }
    else
    {
        //JPSys_SetLightMode(1);
        if(m_AutoLightEnable == 0)
        {
            //JPSys_SetLightEnable(0);
        }
        else if(m_AutoLightEnable == 1)
        {
            //JPSys_SetLightEnable(1);
        }
    }
    m_Cconfig->SetValue(EXT_SECITONCFG, EXT_AUTOLIGHTKEY, (long)enable);

    pthread_mutex_unlock(&m_Lock);
    return 0;
}

int CExtctrl::GetAutoLightEnable(int *enable)
{
    pthread_mutex_lock(&m_Lock);
    *enable = m_AutoLightEnable;
    pthread_mutex_unlock(&m_Lock);
    return 0;
}

int CExtctrl::SetCvbsEnable(int enable)
{
    if(!m_init)
    {
        return -1;
    }
    int ret;
    pthread_mutex_lock(&m_Lock);
    if(enable == m_CvbsEnable)
    {
        pthread_mutex_unlock(&m_Lock);
        return 0;
    }
    m_CvbsEnable = enable;
    m_Cconfig->SetValue(EXT_SECITONCFG, EXT_CVBSKEY, (long)enable);

    
    pthread_mutex_unlock(&m_Lock);
    return 0;
}

int CExtctrl::GetCvbsEnable(int *enable)
{
    pthread_mutex_lock(&m_Lock);
    *enable = m_CvbsEnable;
    pthread_mutex_unlock(&m_Lock);
    return 0;
}

int CExtctrl::CtrlDevFactoryReset()
{
    pthread_mutex_lock(&m_Lock);
    int ret = 0;

    JsonConfigExt info(_Code(FACTORY_RESET_CODE, "factory_reset"), "set");
    info.data = DataConfigBody();
    std::string json_data;
    JsonPackData<JsonConfigExt>(info, json_data);
    MessageManager *msghandle = MessageManager::getInstance();
    std::shared_ptr<receMessage> out_msg;
    ret = msghandle->MSG_SendMessage(0, FACTORY_RESET_CODE, json_data, 1, MQTTMSGTIMEOUT, out_msg);
    if(ret != FACTORY_RESET_CODE)
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

    pthread_mutex_unlock(&m_Lock);
    return 0;
}

int CExtctrl::CtrlDevClear()
{
    int ret = 0;

    JsonConfigExt info(_Code(DEV_FORMAT_CODE, "format"), "set");
    info.data = DataConfigBody();
    std::string json_data;
    JsonPackData<JsonConfigExt>(info, json_data);
    MessageManager *msghandle = MessageManager::getInstance();
    std::shared_ptr<receMessage> out_msg;
    ret = msghandle->MSG_SendMessage(0, DEV_FORMAT_CODE, json_data, 1, MQTTMSGTIMEOUT, out_msg);
    if(ret != DEV_FORMAT_CODE)
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

    return 0;
}

int CExtctrl::GetDevVersionInfo(DevInfo_st *info)
{
    pthread_mutex_lock(&m_Lock);
    memcpy(info, &m_DevVersion, sizeof(DevInfo_st));
    pthread_mutex_unlock(&m_Lock);
    return 0;
}

int CExtctrl::SetTemCtrlCfg(TemperatureCfg TemCfg)
{
    pthread_mutex_lock(&m_Lock);
    memcpy(&m_TemCfg, &TemCfg, sizeof(TemperatureCfg));

    JPSys_SetAutoTemperaCfg(TemCfg);
    m_Cconfig->SetValue(EXT_SECITONCFG, EXT_TEMPERATURE_MIN, (long)TemCfg.min_temperature);
    m_Cconfig->SetValue(EXT_SECITONCFG, EXT_TEMPERATURE_MAX, (long)TemCfg.max_temperature);

    pthread_mutex_unlock(&m_Lock);
    return 0;
}

int CExtctrl::GetTemCtrlCfg(TemperatureCfg *TemCfg)
{
    pthread_mutex_lock(&m_Lock);
    memcpy(&TemCfg, &m_TemCfg, sizeof(TemperatureCfg));
    pthread_mutex_unlock(&m_Lock);
    return 0;
}

int CExtctrl::SetTemperatureMode(int mode)
{
    pthread_mutex_lock(&m_Lock);
    if(m_TemperMode == mode)
    {
        pthread_mutex_unlock(&m_Lock);
        return 0;
    }
    JPSys_SetHeatMode(mode);
    m_Cconfig->SetValue(EXT_SECITONCFG, EXT_TEMPERATURE_MODE, (long)mode);
    m_TemperMode = mode;
    pthread_mutex_unlock(&m_Lock);
    return 0;
}

int CExtctrl::GetTemperaTureMode(int *mode)
{
    pthread_mutex_lock(&m_Lock);
    *mode = m_TemperMode;
    pthread_mutex_unlock(&m_Lock);
    return 0;
}
