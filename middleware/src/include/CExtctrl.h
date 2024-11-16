#ifndef __CEXTCTRL_H
#define __CEXTCTRL_H

#include "CConfig.h"
#include <pthread.h>

typedef struct DevInfo_st
{
    char sort_version[32];
    char hart_version[32];
    char serial_number[32];
}DevInfo_st;


class CExtctrl
{
private:
    int m_WipersEnable;
    int m_AutoLightEnable;
    int m_CvbsEnable;

    CConfig *m_Cconfig;
    pthread_mutex_t m_Lock;
    int m_init;
    
    int LoadParam();
public:
    CExtctrl(void *handle);
    ~CExtctrl();

    int Init();
    int UnInlt();

    int SetWipersEnable(int enable);
    int GetWipersEnable(int *enable);

    int SetAutoLightEnable(int enable);
    int GetAutoLightEnable(int *enable);

    int SetCvbsEnable(int enable);
    int GetCvbsEnable(int *enable);

    int GetDevInfo(DevInfo_st *info);
};



#endif