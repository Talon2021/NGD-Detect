/*
 * @Author: error: error: git config user.name & please set dead value or install git && error: git config user.email & please set dead value or install git & please set dead value or install git
 * @Date: 2024-05-17 10:15:29
 * @LastEditors: error: error: git config user.name & please set dead value or install git && error: git config user.email & please set dead value or install git & please set dead value or install git
 * @LastEditTime: 2024-06-19 14:25:25
 * @FilePath: \panoramic_code\src\AVL\CActionAlarm.cpp
 * @Description: 这是默认设置,请设置`customMade`, 打开koroFileHeader查看配置 进行设置: https://github.com/OBKoro1/koro1FileHeader/wiki/%E9%85%8D%E7%BD%AE
 */
#include "CActionAlarm.h"
#include "CConfig.h"
#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include "common.h"

#include "sdk_log.h"

#define ALARM_ACTION    "alarm_action"
static int CB_AlarmFenceCode(int type, void *buffer, void *userdata)
{
    alarm_data data = {0};
    data.timeTamp = time(NULL);
    CActionAlarm *ptr = (CActionAlarm *)userdata;
    ptr->PushEnevt(type, buffer);
    //m_FnxalarmCallback(data);
    return 0;
}

CActionAlarm::CActionAlarm(void *handle, int ch)
{
    m_bEnable = 0;
    m_FnxalarmCallback = NULL;
    m_han = handle;
}

CActionAlarm::~CActionAlarm()
{
    UnInit();
}
int CActionAlarm::Init()
{
    pthread_mutex_init(&m_lock,NULL);
    CConfig *pcfg = CConfig::GetInstance();
    m_bEnable = pcfg->GetValue(ALARM_ACTION,"alarm_bEnable",(long)1);

    return 0;
}
int CActionAlarm::UnInit()
{
    m_bEnable = 0;
    m_FnxalarmCallback = NULL; 
    return 0;
}

int CActionAlarm::EnableAction(int Action, int bEnable)
{
    
    int bitPosition = (Action & 0xFF0000) >> 16;

    if (bitPosition > 31)
        return -1;

    // 启用或禁用动作
    

    if (bEnable)
        m_bEnable |= (1 << bitPosition);
    else
        m_bEnable &= ~(1 << bitPosition);

    CConfig *pcfg = CConfig::GetInstance();
    pcfg->SetValue(ALARM_ACTION,"alarm_bEnable",(long)m_bEnable);
    return 0;
}

int CActionAlarm::SetAlarmActionCallback(ALARMCALLBACK callback)
{
    m_FnxalarmCallback = callback;
    return 0;
}

int CActionAlarm::PushEnevt(int type, void *buffer)
{
    pthread_mutex_lock(&m_lock);
    switch (type)
    {
    case ALARM_EVENT:
    {
        alarm_data *pcfg = (alarm_data *)buffer;
        m_FnxalarmCallback(pcfg);
        break;
    }
    default:
        break;
    }
    pthread_mutex_unlock(&m_lock);
    return 0;
}
