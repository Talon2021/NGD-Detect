#ifndef __CEXTCTRL_H
#define __CEXTCTRL_H

#include "CConfig.h"
#include <pthread.h>

typedef struct DevInfo_st
{
    char sort_version[64];
    char hart_version[64];
    char serial_number[64];
    char ir_rtsp_url[10][64];
    char vis_rtsp_url[10][64];
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
    
    DevInfo_st m_DevVersion;

    void *m_handle;
    int m_ch;
    int LoadParam();
public:
    CExtctrl(void *handle, int ch);
    ~CExtctrl();

    int Init();
    int UnInlt();

    int SetWipersEnable(int enable);
    int GetWipersEnable(int *enable);

    int SetAutoLightEnable(int enable);
    int GetAutoLightEnable(int *enable);

    int SetCvbsEnable(int enable);
    int GetCvbsEnable(int *enable);

    int CtrlDevFactoryReset();
    int CtrlDevClear();

    int GetDevVersionInfo(DevInfo_st *info);
};



#endif