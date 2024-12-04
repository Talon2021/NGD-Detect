#include "CExtctrl.h"
#include "json_pack.hpp"
#include "MessageManager.h"
#include "common.h"
#include "sdk_log.h"

#define EXT_SECITONCFG      "CEXT_cfg"


#define EXT_WIPERSKEY       "WipersEnable"
#define EXT_AUTOLIGHTKEY    "AutoLightEnable"
#define EXT_CVBSKEY         "CvbsEnable"

int CExtctrl::LoadParam()
{
    int value;
    int ret;
    value = m_Cconfig->GetValue(EXT_SECITONCFG, EXT_WIPERSKEY, (long)0);
    SetWipersEnable(value);

    value = m_Cconfig->GetValue(EXT_SECITONCFG, EXT_AUTOLIGHTKEY, (long)0);
    SetAutoLightEnable(value);

    value = m_Cconfig->GetValue(EXT_SECITONCFG, EXT_CVBSKEY, (long)0);
    SetCvbsEnable(value);
#ifdef PROCESS_CTRL
    //while(1)
    {
        JsonConfigExt info(_Code(DEV_VERSION_CODE, "device_version"), "get");
        std::string json_data;
        JsonPackData<JsonConfigExt>(info, json_data);
        MessageManager *msghandle = MessageManager::getInstance();
        std::shared_ptr<receMessage> out_msg;
        ret = msghandle->MSG_SendMessage(0, DEV_VERSION_CODE, json_data, 1, 100, out_msg);
        if(ret == DEV_VERSION_CODE)
        {
            JsonConfigExt out_response;
            JsonParseData<JsonConfigExt>(out_response, out_msg->recv_data);
            if(out_response.data->soft_version.has_value())
            {
                const std::string& str = out_response.data->soft_version.value();
                memcpy(m_DevVersion.sort_version, str.c_str(), str.size() + 1);
            }
            if(out_response.data->hard_version.has_value())
            {
                const std::string& str = out_response.data->hard_version.value();
                memcpy(m_DevVersion.hart_version, str.c_str(), str.size() + 1);
            }
            if(out_response.data->serial_number.has_value())
            {
                const std::string& str = out_response.data->serial_number.value();
                memcpy(m_DevVersion.serial_number, str.c_str(), str.size() + 1);
            }
            //break;
            
        }
    }
    //while(1)
    {
        JsonConfigExt info(_Code(DEV_RTSP_URL_CODE, "device_rtsp"), "get");
        std::string json_data;
        JsonPackData<JsonConfigExt>(info, json_data);
        MessageManager *msghandle = MessageManager::getInstance();
        std::shared_ptr<receMessage> out_msg;
        ret = msghandle->MSG_SendMessage(0, DEV_RTSP_URL_CODE, json_data, 1, 100, out_msg);
        if(ret == DEV_RTSP_URL_CODE)
        {
            JsonConfigExt out_response;
            JsonParseData<JsonConfigExt>(out_response, out_msg->recv_data);
            if(out_response.data->rtsp_url.has_value())
            {
               
                const std::vector<std::string> str = out_response.data->rtsp_url.value();
                for(int i = 0; i < str.size(); i++)
                {
                    memcpy(m_DevVersion.rtsp_url[i], str[i].c_str(), str[i].size() + 1);
                }
                
            }
            //break;
        }
    }
#endif

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
#ifdef PROCESS_CTRL
    JsonConfigExt info(_Code(WIPERS_CODE, "wipers"), "set");
    info.data = DataConfigBody();
    info.data->value = std::to_string(enable);  
    std::string json_data;
    JsonPackData<JsonConfigExt>(info, json_data);
    MessageManager *msghandle = MessageManager::getInstance();
    std::shared_ptr<receMessage> out_msg;
    ret = msghandle->MSG_SendMessage(0, WIPERS_CODE, json_data, 1, 100, out_msg);
    if(ret != WIPERS_CODE)
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

    CConfig *pcfg = CConfig::GetInstance();
    m_Cconfig->SetValue(EXT_SECITONCFG, EXT_WIPERSKEY, (long)enable);
#endif
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
#ifdef PROCESS_CTRL
    JsonConfigExt info(_Code(AUTO_FILL_LIGHT_CODE, "auto_fill_light"), "set");
    info.data = DataConfigBody();
    info.data->value = std::to_string(enable);  
    std::string json_data;
    JsonPackData<JsonConfigExt>(info, json_data);
    MessageManager *msghandle = MessageManager::getInstance();
    std::shared_ptr<receMessage> out_msg;
    ret = msghandle->MSG_SendMessage(0, AUTO_FILL_LIGHT_CODE, json_data, 1, 100, out_msg);
    if(ret != AUTO_FILL_LIGHT_CODE)
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

    CConfig *pcfg = CConfig::GetInstance();
    m_Cconfig->SetValue(EXT_SECITONCFG, EXT_AUTOLIGHTKEY, (long)enable);
#endif
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
#ifdef PROCESS_CTRL
    JsonConfigExt info(_Code(CVBS_CODE, "cvbs"), "set");
    info.data = DataConfigBody();
    info.data->value = std::to_string(enable);  
    std::string json_data;
    JsonPackData<JsonConfigExt>(info, json_data);
    MessageManager *msghandle = MessageManager::getInstance();
    std::shared_ptr<receMessage> out_msg;
    ret = msghandle->MSG_SendMessage(0, CVBS_CODE, json_data, 1, 100, out_msg);
    if(ret != CVBS_CODE)
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
    m_Cconfig->SetValue(EXT_SECITONCFG, EXT_CVBSKEY, (long)enable);
#endif
    
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
#ifdef PROCESS_CTRL
    JsonConfigExt info(_Code(FACTORY_RESET_CODE, "factory_reset"), "set");
    info.data = DataConfigBody();
    std::string json_data;
    JsonPackData<JsonConfigExt>(info, json_data);
    MessageManager *msghandle = MessageManager::getInstance();
    std::shared_ptr<receMessage> out_msg;
    ret = msghandle->MSG_SendMessage(0, FACTORY_RESET_CODE, json_data, 1, 100, out_msg);
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
#endif
    pthread_mutex_unlock(&m_Lock);
    return 0;
}

int CExtctrl::CtrlDevClear()
{
    int ret = 0;
#ifdef PROCESS_CTRL
    JsonConfigExt info(_Code(DEV_FORMAT_CODE, "format"), "set");
    info.data = DataConfigBody();
    std::string json_data;
    JsonPackData<JsonConfigExt>(info, json_data);
    MessageManager *msghandle = MessageManager::getInstance();
    std::shared_ptr<receMessage> out_msg;
    ret = msghandle->MSG_SendMessage(0, DEV_FORMAT_CODE, json_data, 1, 100, out_msg);
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
#endif
    return 0;
}

int CExtctrl::GetDevVersionInfo(DevInfo_st *info)
{
    pthread_mutex_lock(&m_Lock);
    memcpy(info, &m_DevVersion, sizeof(DevInfo_st));
    pthread_mutex_unlock(&m_Lock);
    return 0;
}
