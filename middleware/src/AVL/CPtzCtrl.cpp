/*
 * @Author: error: error: git config user.name & please set dead value or install git && error: git config user.email & please set dead value or install git & please set dead value or install git
 * @Date: 2024-05-24 10:33:23
 * @LastEditors: error: error: git config user.name & please set dead value or install git && error: git config user.email & please set dead value or install git & please set dead value or install git
 * @LastEditTime: 2024-08-29 14:03:48
 * @FilePath: \infrared\src\AVL\CPtzCtrl.cpp
 * @Description: 这是默认设置,请设置`customMade`, 打开koroFileHeader查看配置 进行设置: https://github.com/OBKoro1/koro1FileHeader/wiki/%E9%85%8D%E7%BD%AE
 */
#include "CPtzCtrl.h"
#include "Cserial.h"

#include "CConfig.h"
#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <functional>
#include "sdk_log.h"
#include <sys/prctl.h>
#include <math.h>
#include "connect.h"
#include "common.h"
#define PTZ_FRAME_HEAD  0xAA
#define PTZ_WRIT_BUFF 11
#define PTZ_WRIT_BUFF_1 7
#define PTZ_VIEWING_ANGLE   (5.8)
#define PTZ_STEP_ANGLE      (10.0)
#define PTZ_SLEEP_MS        (50)         
#define DEV_TTYSERIAL   "/dev/ttyS4"

#define PTZ_CTRLINFO    "ptz_ctrlinfo"

#define PTZ_PRESETSECTION   "Ptz_PresetInfo"
#define WIDTH_PIC       640
#define HEIGHT_PIC      480
#define PTZ_CHECK_SUM(x,n,sum) ({\
    int i;\
    for(i = 0; i < n; i++)\
    {\
        sum+=x[i];\
    }\
})

#ifndef MS
#define MS(ts) (unsigned int)((ts.tv_sec * 1000) + (ts.tv_nsec / 1000000))
#endif

typedef enum {
    PTZ_SCAN_WEEKLY = 0,    // 周扫描
    PTZ_SCAN_FAN,       // 扇扫描
    PTZ_SCAN_CONSTANT,  // 定扫描
    PTZ_SCAN_CONTROL,   // 控制
}PtzScanMode;
static const int ptz_protocol_head = 0x535A4859;

static int CheckPtzUartData(unsigned char *data, int size)
{
    int head = 0;
    int checkSum = 0;
    memcpy(&head, data, 4);
    if(head != ptz_protocol_head)
    {
        return -1;
    }
    PTZ_CHECK_SUM(data, 10, checkSum);
    checkSum = checkSum & 0xff;
    if(checkSum != data[10])
    {
        return -1;
    }
    return 0;
}

CPtzCtrl::CPtzCtrl(void *hannle)
{
    m_ptzYaw = 0;
    m_ptzPitch = 0;
    m_mode = 0;
    m_ptzstep = 0;
    m_enable = 0;
    m_han = hannle;
    memset(&m_preset, 0, sizeof(m_preset));
    m_preset_size = 0;
}

CPtzCtrl::~CPtzCtrl()
{
    m_ptzYaw = 0;
    m_ptzPitch = 0;
    m_mode = 0;
    m_ptzstep = 0;
    m_enable = 0;
    UnInit();
}
void CPtzCtrl::Fnx_ReadUartThread()
{
    prctl(PR_SET_NAME, "thrad_read_ptz", 0, 0, 0);
    unsigned char buf[16] = {0};
    static double last_ptzYaw = m_ptzYaw;
    int ret;
    double decimal = 0;
    struct timespec tp;
    int m_reverse = 1; //1 顺时针扫描 0逆时针扫描 
    printf("read ptz thread is start\n");
    while (!m_thread_exit)
    {
        //read uart
        ret = m_serial->UartRead(buf, 11, 100);
        if(ret > 0)
        {
            if(CheckPtzUartData(buf, 11))
            {
                continue;
            }
            pthread_mutex_lock(&m_lock);
            if(buf[5] == 0x10)
            {
                if(m_mode == PTZ_SCAN_WEEKLY || m_mode == PTZ_SCAN_FAN)
                {
                    decimal = ((buf[8] & 0xff) << 8) /10000.0 + (buf[9] & 0xff) / 10000.0;
                    m_ptzYaw = ((buf[6] & 0xff) << 8) + (buf[7] & 0xff)  + decimal;
                }
#ifdef ALG_SDK
            clock_gettime(CLOCK_MONOTONIC, &tp);
            int time_begin = MS(tp); 
            DEBUG("time_begin = %d  test cur angle m_ptzYaw = %lf\n", time_begin, m_ptzYaw);
            if(m_mode == PTZ_SCAN_FAN)
            {
                if(m_ptzYaw >= last_ptzYaw)
                {
                    SetPositiveOrNegative(m_han, 1);
                }
                else 
                {
                    SetPositiveOrNegative(m_han, 0);
                }
                last_ptzYaw = m_ptzYaw;
            }
            SetMotorAngle(m_han, (float)m_ptzYaw, (float)m_ptzPitch);
            
            
#endif                

            }

            pthread_mutex_unlock(&m_lock);
        }
    }
    return;
}

int CPtzCtrl::Init()
{
    m_levelAngle = PTZ_VIEWING_ANGLE;
    m_ptzstep = PTZ_STEP_ANGLE;
    pthread_mutex_init(&m_lock,NULL);
    m_serial = new Cserial(DEV_TTYSERIAL, 115200);
    m_serial->init();
    PtzZeroInit();
    CConfig *pcfg = CConfig::GetInstance();
    m_mode = pcfg->GetValue(PTZ_CTRLINFO, "scan_mode", (long)PTZ_SCAN_WEEKLY);
    m_ptzstep = pcfg->GetValue(PTZ_CTRLINFO, "ptz_step", (float)PTZ_STEP_ANGLE);
    m_enable = pcfg->GetValue(PTZ_CTRLINFO, "ptz_enable", (long)1);
    m_ptzPitch =  pcfg->GetValue(PTZ_CTRLINFO, "ptz_pitch", (long)0);
    m_speed = pcfg->GetValue(PTZ_CTRLINFO, "ptz_speed", (long)65535);
    m_startAngle = pcfg->GetValue(PTZ_CTRLINFO, "ptz_FanStartAngle", (long)180);
    m_endAngle = pcfg->GetValue(PTZ_CTRLINFO, "ptz_FanEndAngle", (long)360);
    m_targetAngle = pcfg->GetValue(PTZ_CTRLINFO, "ptz_TargetAngle", (long)50);
    m_target_x = pcfg->GetValue(PTZ_CTRLINFO, "ptz_TargetPix_x", (long)0);
    m_target_y = pcfg->GetValue(PTZ_CTRLINFO, "ptz_TargetPix_y", (long)0);
    m_thread_exit = 0;
    if(m_targetAngle < 0 || m_targetAngle > 360)
    {
        m_targetAngle = 0;
    }
#ifdef ALG_SDK
    if(m_enable == 0)
        SetScanModel(m_han, PTZ_SCAN_CONSTANT);
    else
        SetScanModel(m_han, PTZ_SCAN_WEEKLY);
#endif
    QueryVersion();
    InitSetParamMotor();
    m_read_uart_hread = std::thread(std::bind(&CPtzCtrl::Fnx_ReadUartThread,this));
    
   
#ifdef ALG_SDK

    SetPositiveOrNegative(m_han, 1);
    SetStepAngle(m_han, PTZ_STEP_ANGLE);
#endif
    return 0;
}

int CPtzCtrl::UnInit()
{
    m_thread_exit = 1;
    m_read_uart_hread.join();
    //SetPtzEnable(0);    //归0前，必须停止
    PtzZeroInit();
    pthread_mutex_destroy(&m_lock);
    return 0;
}

int CPtzCtrl::SetPtzPitchAngle(double pitch)
{
    //int ret = 0;
    int sum = 0;
    unsigned char buf[PTZ_WRIT_BUFF] = {0};
    pthread_mutex_lock(&m_lock);
    
    m_ptzPitch = pitch;
    m_ptzPitch = m_ptzPitch > 35 ? 35 : m_ptzPitch;
    m_ptzPitch = m_ptzPitch < -20 ? -20 : m_ptzPitch;
    
    pitch = abs(pitch);
    unsigned short intpart = (unsigned short)floor(pitch);  //整数
    double fractionalPart = pitch - intpart;    //小数
    unsigned char fractionalPartScaled = (unsigned char)floor(fractionalPart * 100);

    memcpy(buf, &ptz_protocol_head, 4);
    buf[4] = PTZ_WRIT_BUFF;
    buf[5] = 0x02;
    if(m_ptzPitch < 0)
        buf[6] = 0x03;
    else
        buf[6] = 0x00;
    buf[7] = (intpart >> 8) & 0xFF;
    buf[8] = intpart & 0xFF;
    buf[9] = fractionalPartScaled & 0xFF;
    for(int i = 0; i < PTZ_WRIT_BUFF - 1; i++ )
    {
        sum+=buf[i];
    }
    buf[10] = sum & 0xFF;
    m_serial->UartWrite(buf,PTZ_WRIT_BUFF);
    TimerMilliseconds(20);
    pthread_mutex_unlock(&m_lock);

    CConfig *pcfg = CConfig::GetInstance();
    pcfg->SetValue(PTZ_CTRLINFO, "ptz_pitch", (long)m_ptzPitch);

    
    return 0;
}

int CPtzCtrl::GetPtzPitchAngle(double *pitch)
{
    pthread_mutex_lock(&m_lock);
    *pitch = m_ptzPitch;
    pthread_mutex_unlock(&m_lock);
    return 0;
}

int CPtzCtrl::SetPtzYawAngle(double yaw)
{
    int sum = 0;
    unsigned char buf[PTZ_WRIT_BUFF] = {0};
    pthread_mutex_lock(&m_lock);

    unsigned short intpart = (unsigned short)floor(yaw);  //整数
    double fractionalPart = yaw - intpart;    //小数
    unsigned char fractionalPartScaled = (unsigned char)floor(fractionalPart * 100);

    memcpy(buf, &ptz_protocol_head, 4);
    buf[4] = PTZ_WRIT_BUFF;
    buf[5] = 0x03;

    buf[6] = 0x00;
    buf[7] = (intpart >> 8) & 0xFF;
    buf[8] = intpart & 0xFF;
    buf[9] = fractionalPartScaled & 0xFF;

    for(int i = 0; i < PTZ_WRIT_BUFF-1; i++)
    {
        sum+=buf[i];
    }
    buf[10] = sum & 0xff;
    m_serial->UartWrite(buf,PTZ_WRIT_BUFF);

    m_ptzYaw = yaw;
    CConfig *pcfg = CConfig::GetInstance();
    
    
    pcfg->SetValue(PTZ_CTRLINFO, "ptz_TargetAngle", (float)yaw);
    

    TimerMilliseconds(20);

    pthread_mutex_unlock(&m_lock);
    return 0;
}

int CPtzCtrl::GetPtzYawAngle(double *yaw)
{
    pthread_mutex_lock(&m_lock);
    *yaw = m_ptzYaw;
    pthread_mutex_unlock(&m_lock);
    return 0;
}

int CPtzCtrl::SetScanMode(int mode)
{
    pthread_mutex_lock(&m_lock);
    int sum = 0;
    unsigned char buf[PTZ_WRIT_BUFF] = {0};
    CConfig *pcfg = CConfig::GetInstance();
    m_mode = mode;
    memcpy(buf, &ptz_protocol_head, 4);
    buf[4] = 0x0B;
    buf[5] = 0x01;

    buf[6] = 0x00;
    buf[7] = 0x00;
    buf[8] = 0x00;
   
    if(mode == PTZ_SCAN_WEEKLY)
    {
        buf[9] = 0x01;
    }
    else if(mode == PTZ_SCAN_FAN)
    {
        buf[9] = 0x10;
    }
    else if(mode == PTZ_SCAN_CONSTANT || mode == PTZ_SCAN_CONTROL)
    {
        buf[9] = 0x11;
    }
    for(int i = 0; i < PTZ_WRIT_BUFF - 1; i++ )
    {
        sum+=buf[i];
    }
    buf[10] = sum & 0xFF;
    m_serial->UartWrite(buf,PTZ_WRIT_BUFF);
#ifdef ALG_SDK
    SetScanModel(m_han, mode);
#endif
    //pcfg->SetValue(PTZ_CTRLINFO, "scan_mode", (long)m_mode);
    TimerMilliseconds(20);
    pthread_mutex_unlock(&m_lock);
    
    return 0;
}

int CPtzCtrl::GetScanMode(int *mode)
{
    pthread_mutex_lock(&m_lock);
    *mode = m_mode;
    pthread_mutex_unlock(&m_lock);
    return 0;
}

int CPtzCtrl::SetStep(double step)
{
    pthread_mutex_lock(&m_lock);
    CConfig *pcfg = CConfig::GetInstance();
    m_ptzstep = step;
    pcfg->SetValue(PTZ_CTRLINFO, "ptz_step", (long)m_ptzstep);
    pthread_mutex_unlock(&m_lock);
    return 0;
}

int CPtzCtrl::GetStep(double *step)
{
    pthread_mutex_lock(&m_lock);
    *step = m_ptzstep;
    pthread_mutex_unlock(&m_lock);
    return 0;
}

int CPtzCtrl::SetPtzEnable(int enable)
{
    DEBUG("ptz enable = %d\n",enable);
    pthread_mutex_lock(&m_lock);
    int sum = 0;
    unsigned char buf[PTZ_WRIT_BUFF_1] = {0};
    memcpy(buf, &ptz_protocol_head, 4);
    buf[4] = PTZ_WRIT_BUFF_1;
    if(enable == 1)
        buf[5] = 0x06;
    else
        buf[5] = 0x07;
    for(int i = 0; i < PTZ_WRIT_BUFF_1 - 1; i++ )
    {
        sum+=buf[i];
    }
    buf[6] = sum & 0xff;
    m_serial->UartWrite(buf,PTZ_WRIT_BUFF_1);
    
    CConfig *pcfg = CConfig::GetInstance();
    m_enable = enable;
    pcfg->SetValue(PTZ_CTRLINFO, "ptz_enable", (long)m_enable);
#ifdef ALG_SDK
    if(enable == 0)
        SetScanModel(m_han, PTZ_SCAN_CONSTANT);
#endif
    TimerMilliseconds(20);
    pthread_mutex_unlock(&m_lock);
   
    return 0;
}

int CPtzCtrl::GetPtzEnable(int *enable)
{
    pthread_mutex_lock(&m_lock);
    *enable = m_enable;
    pthread_mutex_unlock(&m_lock);
    return 0;
}

int CPtzCtrl::PtzFullUpdate()
{
    double overlapAreaLength = 0; 
    int pixforangle = WIDTH_PIC/m_levelAngle;
    if(m_levelAngle > m_ptzstep)
        overlapAreaLength = (m_levelAngle - m_ptzstep) * pixforangle;

    int N = 360 /m_ptzstep;     //局部图数量 步进次数=局部次数-1
    m_fullPicWidth = N *640 - (N-1)* (int)(overlapAreaLength);
    m_ptzYaw = 0;
    return 0;
}

int CPtzCtrl::pixToAngle(int pixer)
{
    float overlap = 360 / m_fullPicWidth;
    int targetAngle = (int)(pixer * overlap);

    int lower_multiple = (targetAngle / PTZ_STEP_ANGLE) * PTZ_STEP_ANGLE;
    int upper_multiple = lower_multiple + PTZ_STEP_ANGLE;
    if (abs(targetAngle - lower_multiple) < abs(targetAngle - upper_multiple)) {
        return lower_multiple;
    } else {
        return upper_multiple;
    }
}

int CPtzCtrl::AngleToPix(int angle)
{
    float tmp = m_fullPicWidth / 360;

    return (int)(tmp * angle);
}

int CPtzCtrl::StartYawElc()
{
    int sum;
    unsigned char buf[PTZ_WRIT_BUFF_1];
    memset(buf, 0, sizeof(buf));
    memcpy(buf, &ptz_protocol_head, 4);

    buf[4] = PTZ_WRIT_BUFF_1;
    if(m_enable == 1)
        buf[5] = 0x06;
    else
        buf[5] = 0x07;
    sum = 0;
    for(int i = 0; i < PTZ_WRIT_BUFF_1-1; i++)
    {
        sum+=buf[i];
    }
    buf[6] = sum & 0xff;
    m_serial->UartWrite(buf,PTZ_WRIT_BUFF_1);
    //TimerMilliseconds(100);
    return 0;
}

int CPtzCtrl::QueryVersion()
{
    int sum = 0;
    unsigned char buf[PTZ_WRIT_BUFF_1];
    memset(buf, 0, sizeof(buf));
    memcpy(buf, &ptz_protocol_head, 4);

    buf[4] = PTZ_WRIT_BUFF_1;
    buf[5] = 0x12;
    for(int i = 0; i < PTZ_WRIT_BUFF_1-1; i++)
    {
        sum+=buf[i];
    }
    buf[6] = sum & 0xff;
    m_serial->UartWrite(buf,PTZ_WRIT_BUFF_1);
    TimerMilliseconds(20);
    return 0;
}

int CPtzCtrl::DataTransm(void *data, int len, void *out)
{
    int ret = 0;
    m_serial->UartWrite((const unsigned char *)data, len);
    if(ret != 0)
    {
        ERROR("write is err \n");
        return -1;
    }
    m_serial->UartRead((unsigned char *)out, 1024, 200);
    return 0;
}

int CPtzCtrl::PtzZeroInit()
{
    static int last_time = 0;
    int sum = 0;
    int i = 0;
    unsigned char buf[PTZ_WRIT_BUFF_1] = {0};
    if(get_ms() - last_time < 1000)
    {
        return 0;
    }
    pthread_mutex_lock(&m_lock);
    last_time = get_ms();
    memset(buf, 0, sizeof(buf));
    memcpy(buf, &ptz_protocol_head, 4);
    buf[4] = PTZ_WRIT_BUFF_1;
    buf[5] = 0x07;
    for( i = 0; i < PTZ_WRIT_BUFF_1-1; i++)
    {
        sum+=buf[i];
    }
    buf[6] = sum & 0xff;
    m_serial->UartWrite(buf, PTZ_WRIT_BUFF_1);
    
    TimerMilliseconds(20);

    memset(buf, 0, sizeof(buf));
    sum = 0;
    memcpy(buf, &ptz_protocol_head, 4);
    buf[4] = PTZ_WRIT_BUFF_1;
    buf[5] = 0x00;
    for(i = 0; i < PTZ_WRIT_BUFF_1-1; i++)
    {
        sum+=buf[i];
    }
    
    buf[6] = sum & 0xff;
    m_serial->UartWrite(buf,PTZ_WRIT_BUFF_1);
    PtzFullUpdate();
#ifdef ALG_SDK
    SetMotorStartType(m_han, 0);
#endif
    m_ptzYaw = 0;
    m_mode = -1;
    TimerMilliseconds(500);
    pthread_mutex_unlock(&m_lock);
   
    return 0;
}

int CPtzCtrl::SetTargetLocation(target_data data)
{
    
    double angle = data.target_angle;
    if(data.mode == 0)
    {
        if(angle < 0 || angle > 360)
        {
            return 0;
        }
        angle = data.target_angle;
        m_target_x = AngleToPix(data.target_angle);
        m_target_y = HEIGHT_PIC/2;
    }
    else
    {
        angle = pixToAngle(data.angle_x);
        if(angle < 0 || angle > 360)
        {
            return 0;
        }
        m_target_x = data.angle_x;
        m_target_y = data.angle_y;
    }
    DEBUG("cur tarangel = %lf ,mode =%d\n",angle, m_mode);
    pthread_mutex_lock(&m_lock);
    int sum = 0;
    unsigned short intpart = (unsigned short)floor(angle);  //整数
    double fractionalPart = angle - intpart;    //小数
    unsigned char fractionalPartScaled = (unsigned char)floor(fractionalPart * 100);

    unsigned char buf[PTZ_WRIT_BUFF] = {0};
    memcpy(buf, &ptz_protocol_head, 4);
    buf[4] = PTZ_WRIT_BUFF;
    buf[5] = 0x03;

    buf[6] = 0x00;
    buf[7] = (intpart >> 8) & 0xFF;
    buf[8] = intpart & 0xFF;
    buf[9] = fractionalPartScaled & 0xFF;

    
    for(int i = 0; i < PTZ_WRIT_BUFF-1; i++)
    {
        sum+=buf[i];
    }
    buf[10] = sum & 0xff;
    m_serial->UartWrite(buf,PTZ_WRIT_BUFF);
    CConfig *pcfg = CConfig::GetInstance();
    m_targetAngle = angle;
    m_ptzYaw = angle;

    pcfg->SetValue(PTZ_CTRLINFO, "ptz_TargetAngle", (float)angle);
    pcfg->SetValue(PTZ_CTRLINFO, "ptz_TargetPix_x", (long)m_target_x);
    pcfg->SetValue(PTZ_CTRLINFO, "ptz_TargetPix_y", (long)m_target_y);
    TimerMilliseconds(20);
#ifdef ALG_SDK
    SetMotorAngle(m_han, (float)m_ptzYaw, (float)m_ptzPitch);
#endif
    pthread_mutex_unlock(&m_lock);
   

    // if(m_enable == 1)
    // {
    //     StartYawElc();
    // }
    return 0;
}

int CPtzCtrl::GetTargetLocation(target_data *data)
{
    pthread_mutex_lock(&m_lock);
    data->target_angle = m_targetAngle;
    data->angle_x = m_target_x;
    data->angle_y = m_target_y;
    pthread_mutex_unlock(&m_lock);
    return 0;
}

int CPtzCtrl::SetFanScanAngle(double startAngle, double endAngle)
{
    pthread_mutex_lock(&m_lock);
    int sum = 0;
    unsigned short start_intpart = (unsigned short)floor(startAngle);
    unsigned short end_intpart = (unsigned short)floor(endAngle);
    unsigned char buf[PTZ_WRIT_BUFF] = {0};
    memcpy(buf, &ptz_protocol_head, 4);
    buf[4] = PTZ_WRIT_BUFF;
    buf[5] = 0x04;

    buf[6] = (start_intpart >> 8 ) & 0xFF;
    buf[7] = start_intpart & 0xFF;
    buf[8] = (end_intpart >> 8 ) & 0xFF;
    buf[9] = end_intpart & 0xFF;

    for(int i = 0; i < PTZ_WRIT_BUFF-1; i++)
    {
        sum+=buf[i];
    }
    buf[10] = sum & 0xff;
    m_ptzYaw = startAngle;
    m_serial->UartWrite(buf,PTZ_WRIT_BUFF);
    m_startAngle = startAngle;
    m_endAngle = endAngle;
    CConfig *pcfg = CConfig::GetInstance();
    pcfg->SetValue(PTZ_CTRLINFO, "ptz_FanStartAngle", (float)startAngle);
    pcfg->SetValue(PTZ_CTRLINFO, "ptz_FanEndAngle", (float)endAngle);
    TimerMilliseconds(20);
    pthread_mutex_unlock(&m_lock);
    
    return 0;
}

int CPtzCtrl::GetFanScanAngle(double *startAngle, double *endAngle)
{
    pthread_mutex_lock(&m_lock);
    *startAngle = m_startAngle;
    *endAngle = m_endAngle;
    pthread_mutex_unlock(&m_lock);
    return 0;
}

int CPtzCtrl::SetScanSpeed(unsigned short speed)
{
    pthread_mutex_lock(&m_lock);
    int sum = 0;
    unsigned char buf[PTZ_WRIT_BUFF] = {0};
    memcpy(buf, &ptz_protocol_head, 4);
    buf[4] = PTZ_WRIT_BUFF;
    buf[5] = 0x05;

    buf[6] = 0x00;
    buf[7] = 0x00;
    buf[8] = (speed >> 8) & 0xFF;
    buf[9] = speed & 0xFF;

    for(int i = 0; i < PTZ_WRIT_BUFF-1; i++)
    {
        sum+=buf[i];
    }
    buf[10] = sum & 0xff;
    m_serial->UartWrite(buf,PTZ_WRIT_BUFF);
    m_speed = speed;
    CConfig *pcfg = CConfig::GetInstance();
    pcfg->SetValue(PTZ_CTRLINFO, "ptz_speed", (long)m_speed);
    TimerMilliseconds(20);
    pthread_mutex_unlock(&m_lock);
    
   
    return 0;
}

int CPtzCtrl::GetScanSpeed(unsigned short *speed)
{
    pthread_mutex_lock(&m_lock);
    *speed = m_speed;
    pthread_mutex_unlock(&m_lock);
    return 0;
}

int CPtzCtrl::SetFanEnable(int enable)
{
    if(m_fan_enable == enable)
        return 0;
    pthread_mutex_lock(&m_lock);
    unsigned char buf[PTZ_WRIT_BUFF_1] = {0};
    int sum = 0;
    buf[4] = PTZ_WRIT_BUFF_1;
    if(enable == 1)
        buf[5] = 0x08;
    else
        buf[5] = 0x09;
    
    for(int i = 0; i < PTZ_WRIT_BUFF_1-1; i++)
    {
        sum+=buf[i];
    }
    buf[6] = sum & 0xff;
    m_serial->UartWrite(buf,PTZ_WRIT_BUFF_1);
    m_fan_enable = enable;

    TimerMilliseconds(20);
    pthread_mutex_unlock(&m_lock);
    return 0;
}

int CPtzCtrl::GetFanEnable()
{
    return m_fan_enable;
}

int CPtzCtrl::SetHeatingEnbale(int enable)
{
    if(m_heating_enable == enable)
        return 0;
    pthread_mutex_lock(&m_lock);
    int sum = 0;
    unsigned char buf[PTZ_WRIT_BUFF_1] = {0};
    memcpy(buf, &ptz_protocol_head, 4);
    buf[4] = PTZ_WRIT_BUFF_1;
    if(enable == 1)
        buf[5] = 0x0A;
    else
        buf[5] = 0x0B;
    for(int i = 0; i < PTZ_WRIT_BUFF_1-1; i++)
    {
        sum+=buf[i];
    }
    buf[6] = sum & 0xff;
    m_serial->UartWrite(buf,PTZ_WRIT_BUFF_1);
    m_heating_enable = enable;

    TimerMilliseconds(20);
    pthread_mutex_unlock(&m_lock);
    return 0;
}

int CPtzCtrl::GetHeatingEnbale()
{
    return m_heating_enable;
}

int CPtzCtrl::SetPreset(char *preset_name)
{
    if(m_preset_size >= 256)
    {
        ERROR("set preset is max\n");
        return -1;
    }
    pthread_mutex_lock(&m_lock);
    for(int i = 0; i < 8; i++)
    {
        if(m_preset[i].enable == 0)
        {
            memcpy(m_preset[i].name, preset_name, sizeof(m_preset[i].name));
            m_preset[i].yaw = m_ptzYaw;
            m_preset[i].pitch = m_ptzPitch;
            m_preset[i].enable = 1;
            break;
        }
    }
    pthread_mutex_unlock(&m_lock);
    return 0;
}

int CPtzCtrl::GetPreset(traget_preset *presetInfo, int *num)
{
    pthread_mutex_lock(&m_lock);
    *num = m_preset_size;
    memcpy(presetInfo, m_preset, sizeof(traget_preset) * m_preset_size);

    pthread_mutex_unlock(&m_lock);
    return 0;
}

int CPtzCtrl::DelPreset(char *preset_name)
{
    if(preset_name == NULL)
    {
        ERROR("param is null\n");
        return -1;
    }
    pthread_mutex_lock(&m_lock);
    for(int i = 0; i < 8; i++)
    {
        if(strcmp(preset_name, m_preset[i].name) == 0)
        {
            m_preset[i].enable = 0;
            break;
        }
    }
    pthread_mutex_unlock(&m_lock);
    return 0;
}

int CPtzCtrl::SetPresetEx(traget_preset *presetInfo, int num)
{
    if(presetInfo == NULL)
    {
        ERROR("prame is err \n");
        return -1;
    }
    pthread_mutex_lock(&m_lock);
    char pre_section[32] = {0};
    
    CConfig *pcfg = CConfig::GetInstance();
    for(int i = 0; i < num; i++)
    {
        snprintf(pre_section, sizeof(pre_section), "%s_%03d", PTZ_PRESETSECTION, i);
        pcfg->SetValue(pre_section, "name", presetInfo[i].name);
        pcfg->SetValue(pre_section, "yaw", (float)0);
        pcfg->SetValue(pre_section, "pitch", (float)0);
        pcfg->SetValue(pre_section, "enable", (long)1);
        pcfg->SetValue(pre_section, "num", (long)presetInfo[i].num);
    }
    if(m_preset_size > num)
    {
        for(int i = num; i < m_preset_size; i++)
        {
            snprintf(pre_section, sizeof(pre_section), "%s_%03d", PTZ_PRESETSECTION, i);
            pcfg->SetValue(pre_section, "enable", (long)0);
        }
    }
    m_preset_size = num;
    memcpy(m_preset, presetInfo, sizeof(traget_preset) * num);
    pthread_mutex_unlock(&m_lock);
    return 0;
}

int CPtzCtrl::InitSetParamMotor()
{
    unsigned char buf[PTZ_WRIT_BUFF] = {0};
    unsigned short start_intpart = (unsigned short)floor(m_startAngle);
    unsigned short end_intpart = (unsigned short)floor(m_endAngle);
    int sum = 0;
    int i = 0;

    // memset(buf, 0, sizeof(buf));
    // memcpy(buf, &ptz_protocol_head, 4);
    // buf[4] = PTZ_WRIT_BUFF_1;
    // buf[5] = 0x07;
    // sum = 0;
    // for( i = 0; i < PTZ_WRIT_BUFF_1-1; i++)
    // {
    //     sum+=buf[i];
    // }
    // m_serial->UartWrite(buf, PTZ_WRIT_BUFF_1);
    // TimerMilliseconds(100);   //归0前，必须停止

    /*打开使能*/
    memset(buf, 0, sizeof(buf));
    memcpy(buf, &ptz_protocol_head, 4);

    buf[4] = PTZ_WRIT_BUFF_1;
    if(m_enable == 1)
        buf[5] = 0x06;
    else
        buf[5] = 0x07;
    sum = 0;
    for(i = 0; i < PTZ_WRIT_BUFF_1-1; i++)
    {
        sum+=buf[i];
    }
    buf[6] = sum & 0xff;
    m_serial->UartWrite(buf,PTZ_WRIT_BUFF_1);
    TimerMilliseconds(100);

    /*设置俯仰角*/
   
    unsigned short intpart = (unsigned short)floor(m_ptzPitch);  //整数
    double fractionalPart = m_ptzPitch - intpart;    //小数
    unsigned char fractionalPartScaled = (unsigned char)floor(fractionalPart * 100);
    memset(buf, 0, sizeof(buf));
    memcpy(buf, &ptz_protocol_head, 4);
    buf[4] =PTZ_WRIT_BUFF;
    buf[5] = 0x02;
    if(m_ptzPitch < 0)
        buf[6] = 0x03;
    else
        buf[6] = 0x00;
    buf[7] = (intpart >> 8) & 0xFF;
    buf[8] = intpart & 0xFF;
    buf[9] = fractionalPartScaled & 0xFF;
    sum = 0;
    for( i = 0; i < PTZ_WRIT_BUFF-1; i++)
    {
        sum+=buf[i];
    }
    buf[10] = sum & 0xff;
    m_serial->UartWrite(buf,PTZ_WRIT_BUFF);

    TimerMilliseconds(100);
    
    /*设置扫描速度*/
    // memset(buf, 0, sizeof(buf));
    // memcpy(buf, &ptz_protocol_head, 4);

    // buf[4] = PTZ_WRIT_BUFF;
    // buf[5] = 0x05;

    // buf[6] = 0x00;
    // buf[7] = 0x00;
    // buf[8] = (m_speed >> 8) & 0xFF;
    // buf[9] = m_speed & 0xFF;
    // sum = 0;
    // for( i = 0; i < PTZ_WRIT_BUFF-1; i++)
    // {
    //     sum+=buf[i];
    // }
    // buf[10] = sum & 0xff;
    // m_serial->UartWrite(buf,PTZ_WRIT_BUFF);
    // TimerMilliseconds(20);

    /*设置扫描模式*/
    if(m_mode >= 0)
    {
        memset(buf, 0, sizeof(buf));
        memcpy(buf, &ptz_protocol_head, 4);
        buf[4] = PTZ_WRIT_BUFF;
        buf[5] = 0x01;

        buf[6] = 0x00;
        buf[7] = 0x00;
        buf[8] = 0x00;
        if(m_mode == PTZ_SCAN_WEEKLY)
        {
            buf[9] = 0x01;
        }
        else if(m_mode == PTZ_SCAN_FAN)
        {
            buf[9] = 0x10;
        }
        else if(m_mode == PTZ_SCAN_CONSTANT || m_mode == PTZ_SCAN_CONSTANT)
        {
            buf[9] = 0x11;
        }
        sum = 0;
        for( i = 0; i < PTZ_WRIT_BUFF-1; i++)
        {
            sum+=buf[i];
        }
        buf[10] = sum & 0xff;
        m_serial->UartWrite(buf,PTZ_WRIT_BUFF);
        TimerMilliseconds(100);
        switch (m_mode)
        {
        case PTZ_SCAN_FAN:
            memset(buf, 0, sizeof(buf));
            memcpy(buf, &ptz_protocol_head, 4);
            buf[4] = PTZ_WRIT_BUFF;
            buf[5] = 0x04;
           
            buf[6] = (start_intpart >> 8 ) & 0xFF;
            buf[7] = start_intpart & 0xFF;
            buf[8] = (end_intpart >> 8 ) & 0xFF;
            buf[9] = end_intpart & 0xFF;
            sum = 0;
            for( i = 0; i < PTZ_WRIT_BUFF-1; i++)
            {
                sum+=buf[i];
            }
            buf[10] = sum & 0xff;
            m_serial->UartWrite(buf,PTZ_WRIT_BUFF);
            break;
        case PTZ_SCAN_CONSTANT:
        case PTZ_SCAN_CONTROL:
            memset(buf, 0, sizeof(buf));
            memcpy(buf, &ptz_protocol_head, 4);
            buf[4] = PTZ_WRIT_BUFF;
            buf[5] = 0x03;
            intpart = (unsigned short)floor(m_targetAngle);
            fractionalPart = m_targetAngle - intpart;
            fractionalPartScaled = (unsigned char)floor(fractionalPart * 100);
            buf[6] = 0x00;
            buf[7] = (intpart >> 8) & 0xFF;
            buf[8] = intpart & 0xFF;
            buf[9] = fractionalPartScaled & 0xFF;
            sum = 0;
            for( i = 0; i < PTZ_WRIT_BUFF-1; i++)
            {
                sum+=buf[i];
            }
            buf[10] = sum & 0xff;
            m_serial->UartWrite(buf,PTZ_WRIT_BUFF);
            m_ptzYaw = m_targetAngle;
#ifdef ALG_SDK
            SetMotorAngle(m_han, (float)m_ptzYaw, (float)m_ptzPitch);
#endif
        default:
            break;
        }
        TimerMilliseconds(100);
    }
    return 0;
}
