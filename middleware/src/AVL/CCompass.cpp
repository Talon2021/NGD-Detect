/*
 * @Author: error: error: git config user.name & please set dead value or install git && error: git config user.email & please set dead value or install git & please set dead value or install git
 * @Date: 2024-05-16 14:58:17
 * @LastEditors: error: error: git config user.name & please set dead value or install git && error: git config user.email & please set dead value or install git & please set dead value or install git
 * @LastEditTime: 2024-06-20 15:19:24
 * @FilePath: \panoramic_code\src\AVL\CCompass.cpp
 * @Description: 这是默认设置,请设置`customMade`, 打开koroFileHeader查看配置 进行设置: https://github.com/OBKoro1/koro1FileHeader/wiki/%E9%85%8D%E7%BD%AE
 */
#include "CCompass.h"
#include "common.h"
#include "memmap.h"
#include "gpio.h"
#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include "CConfig.h"
#include <math.h>
#include "sdk_log.h"
#include <sys/prctl.h>
#include <functional>
#define SENSORS_Gause_PER_LSB_CFG (double)((2 * 50) / 65536.0)
#define COMPASS_SECTION     "compass_z"


static void InitializeMovingAverageFilter(MovingAverageFilter *filter)
{
    for (int i = 0; i < LSM303SORTLEN; ++i)
    {
        filter->sort[i] = 0.0;
    }
    filter->index = 0;
    filter->sum = 0.0;
    filter->initialized = false;
}

static void ApplyMovingAverageFilter(MovingAverageFilter *filter, double newValue, double *filteredValue)
{
    // 更新累加和
    filter->sum = filter->sum - filter->sort[filter->index] + newValue;

    // 存储新的角度值至滤波数组
    filter->sort[filter->index] = newValue;

    // 更新索引
    filter->index = (filter->index + 1) % LSM303SORTLEN;

    // 检查数组是否已完全初始化
    if (!filter->initialized && filter->index == LSM303SORTLEN - 1)
    {
        filter->initialized = true;
    }

    // 计算并返回平均值
    if (filter->initialized)
    {
        *filteredValue = filter->sum / LSM303SORTLEN;
    }
    else
    {
        *filteredValue = newValue;
    }
}

void CCompass::Fnx_Lsm303Thread()
{
    prctl(PR_SET_NAME, "thrad_lsm303", 0, 0, 0);
   
    while (!m_Lsm303Exit)
    {
        Read_Lsm303_data();
        usleep(150*1000);
    }
    return;
}

CCompass::CCompass(void *handle, int ch)
{
    m_yawAngle = 0;
    m_samples = 0;
    m_fd = 0;
    m_init = 0;
    m_Lsm303Exit = 1;
    memset(&compassFilterx, 0, sizeof(MovingAverageFilter));
    memset(&compassFiltery, 0, sizeof(MovingAverageFilter));
    calibration_flag = 0;
    calibration_offset_angle = 90;
    m_han = handle;
}

CCompass::~CCompass()
{
    UnInit();
}

int CCompass::Init()
{
    if(m_init == 1)
        return 0;
    m_Lsm303Exit = 0;
    char cmd[256] = {0};
    unsigned int value;
    pthread_mutex_init(&m_lock,NULL);

    //mahony_init(&MahonyFilter, G_Kp, G_Ki, 0.01);
    InitializeMovingAverageFilter(&compassFilterx);
    InitializeMovingAverageFilter(&compassFiltery);
    himm(0xfe02000c, 0xffff0011);

    value = getHimm(0xfe02000c);
    value &= ~(3 << 8);
    value |= 0xffff0000;
    himm(0xfe02000c, value);

    SetGpioDir(0, 15, 1);
    SetGpioData(0, 15, 1);

    usleep(1000 * 100);

    snprintf(cmd, sizeof(cmd), "rmmod /ko/mg.ko");
    mysystem(cmd);

    snprintf(cmd, sizeof(cmd), "insmod /ko/mg.ko");
    mysystem(cmd);

    usleep(1000 * 100);

    m_fd = open("/dev/mg", O_RDWR);
    if(m_fd < 0)
    {
        ERROR("open mg is err\n");
        return -1;
    }
    CConfig *pcfg = CConfig::GetInstance();
    m_samples = pcfg->GetValue(COMPASS_SECTION,"samples",(long)0);
    if(m_samples == 0)
    {
        m_samples = 1000;
        pcfg->SetValue(COMPASS_SECTION,"samples",(long)(m_samples));
    }

    m_Lsm303Thread = std::thread(std::bind(&CCompass::Fnx_Lsm303Thread,this));
    m_init = 1;
    return 0;
}

int CCompass::UnInit()
{
    if(m_init == 0)
        return 0;
    char cmd[256] = {0};
    m_Lsm303Exit = 1;
    m_Lsm303Thread.join();

    snprintf(cmd, sizeof(cmd), "rmmod /ko/mg.ko");
    mysystem(cmd);
   
    if(m_fd)
    {
        close(m_fd);
        m_fd = 0;
    }
        
    m_yawAngle = 0;
    m_samples = 0;
    pthread_mutex_destroy(&m_lock);
    m_init = 0;
    return 0;
}

int CCompass::Read_Lsm303_data()
{
    if(!m_init)
    {
        return 1;
    }

    int ret;
    short databuf[7] = {0};
    int i;
    double mx = 0, my = 0, mz = 0;
    double filter_x = 0, filter_y = 0; //filter_z = 0;
    if(calibration_flag == 1)
    {
        double sumX = 0.0, sumY = 0.0, sumZ = 0.0;
        double offsetX = 0, offsetY  = 0; //offsetZ  = 0;
        for( i = 0; i < m_samples; i++)
        {
            ret = read(m_fd, databuf, sizeof(databuf));
            if(ret == 0)
            {
                mx = -(double)(databuf[0] * SENSORS_Gause_PER_LSB_CFG);
                my = (double)(databuf[1] * SENSORS_Gause_PER_LSB_CFG);
                mz = (double)(databuf[2] * SENSORS_Gause_PER_LSB_CFG);
                sumX += mx;
                sumY += my;
                sumZ += mz;
            }
        }
        offsetX = sumX / m_samples;
        offsetY = sumY / m_samples;
        //offsetZ = sumZ / m_samples;
        double avgX = offsetX;
        double avgY = offsetY;
        double calibration_heading = atan2(avgY, avgX) * (180 / M_PI);
        
        if (calibration_heading < 0) {
            calibration_heading += 360;
        }
        calibration_offset_angle = calibration_heading - 0.0;       //0度校准
        double yawAngle = calibration_heading;
        SetYawAngel(yawAngle);
        calibration_flag = 0;
        return 0;
    }
    ret = read(m_fd, databuf, sizeof(databuf));
    if(ret == 0)
    {
        mx = -(double)(databuf[0] * SENSORS_Gause_PER_LSB_CFG);
        my = (double)(databuf[1] * SENSORS_Gause_PER_LSB_CFG);
        mz = (double)(databuf[2] * SENSORS_Gause_PER_LSB_CFG);
        //printf("mx = %lf my=%lf mz = %lf\n",mx,my,mz);

        filter_x = 0;
        ApplyMovingAverageFilter(&compassFilterx, mx, &filter_x);
        mx = filter_x;

        filter_y = 0;
        ApplyMovingAverageFilter(&compassFiltery, my, &filter_y);
        my = filter_y;
        float heading = atan2(my, mx);
        heading = heading * (180 / M_PI);
        if (heading < 0)
        {
            heading += 360;
        }
        double yawAngle = fmod((heading+calibration_offset_angle) , 360.0);
        SetYawAngel(yawAngle);
    }
    return 0;
}

int CCompass::GetYawAngle(double *value)
{
    if(!m_init)
    {
        return -1;
    }
    pthread_mutex_lock(&m_lock);
    *value = m_yawAngle;
    pthread_mutex_unlock(&m_lock);
    return 0;
}

int CCompass::SetYawAngel(double value)
{
    if(!m_init)
    {
        return -1;
    }
    pthread_mutex_lock(&m_lock);
    m_yawAngle = value;
    pthread_mutex_unlock(&m_lock);
    return 0;
}

int CCompass::SetCalibrationSamples(int value)
{
    if(!m_init)
    {
        return -1;
    }
    m_samples = value;
    CConfig *pcfg = CConfig::GetInstance();
    pcfg->SetValue(COMPASS_SECTION,"samples",(long)(m_samples));
    return 0;
}

int CCompass::GetCalibrationSamples(int *value)
{
    if(!m_init)
    {
        return -1;
    }
    *value = m_samples;
    return 0;
}

int CCompass::SetCalibrationFlag(int flag)
{
    if(!m_init)
    {
        return -1;
    }
    calibration_flag = flag;
    return 0;
}

int CCompass::GetCalibrationFlag(int *flag)
{
    if(!m_init)
    {
        return -1;
    }
    *flag = calibration_flag;
    return 0;
}
