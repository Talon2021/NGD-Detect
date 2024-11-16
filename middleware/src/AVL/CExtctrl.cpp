#include "CExtctrl.h"

#define EXT_SECITONCFG      "CEXT_cfg"


#define EXT_WIPERSKEY       "WipersEnable"
#define EXT_AUTOLIGHTKEY    "AutoLightEnable"
#define EXT_CVBSKEY         "CvbsEnable"

int CExtctrl::LoadParam()
{
    int value;
    value = m_Cconfig->GetValue(EXT_SECITONCFG, EXT_WIPERSKEY, (long)0);
    SetWipersEnable(value);

    value = m_Cconfig->GetValue(EXT_SECITONCFG, EXT_AUTOLIGHTKEY, (long)0);
    SetAutoLightEnable(value);

    value = m_Cconfig->GetValue(EXT_SECITONCFG, EXT_CVBSKEY, (long)0);
    SetCvbsEnable(value);
    return 0;
}

CExtctrl::CExtctrl(void *handle)
{
    m_WipersEnable = 0;
    m_AutoLightEnable = 0;
    m_CvbsEnable = 0;
    m_init = 0;
    m_Cconfig = NULL;
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
    m_WipersEnable = enable;
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
    m_AutoLightEnable = enable;
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

int CExtctrl::GetDevInfo(DevInfo_st *info)
{
    
    return 0;
}
