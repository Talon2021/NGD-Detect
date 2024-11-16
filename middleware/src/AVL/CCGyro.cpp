/*
 * @Author: error: error: git config user.name & please set dead value or install git && error: git config user.email & please set dead value or install git & please set dead value or install git
 * @Date: 2024-05-13 15:58:29
 * @LastEditors: error: error: git config user.name & please set dead value or install git && error: git config user.email & please set dead value or install git & please set dead value or install git
 * @LastEditTime: 2024-08-27 14:55:29
 * @FilePath: \panoramic_code\src\AVL\CCGyro.cpp
 * @Description: 这是默认设置,请设置`customMade`, 打开koroFileHeader查看配置 进行设置: https://github.com/OBKoro1/koro1FileHeader/wiki/%E9%85%8D%E7%BD%AE
 */
#include "CCGyro.h"
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
#include <sys/prctl.h>

#include "sdk_log.h"

#include <functional>

void CCGyro::Fnx_Mpu6500Thread()
{
    prctl(PR_SET_NAME, "thrad_mpu6500", 0, 0, 0);
    while (!m_Mpu6500Exit)
    {
        Read_Gyro_data();
        usleep(150*1000);
    }
    return;
}

CCGyro::CCGyro(void *hannle)
{
    m_PitchAngle = 0;
    m_rollAngle = 0;
    m_CalibrationSample = 0;
    bias_gx = 0;
    bias_gy = 0; 
    bias_gz = 0;
    bias_PitchAngle = 0; 
    bias_RollAngle = 0;
    m_CalibrationFlag = 0;
    m_Mpu6500Exit = 0;
    m_han = hannle;
}

CCGyro::~CCGyro()
{
    UnInit();
}

int CCGyro::Init()
{
    char cmd[256];
    m_CalibrationSample = 1000;
    m_CalibrationFlag = 0;
    
    unsigned int value;
    pthread_mutex_init(&m_AngleMutex,NULL);
    mahony_init(&MahonyFilter, G_Kp, G_Ki, 0.01);
    himm(0xfe02000c, 0xffff0011);
    value = getHimm(0xfe02000c);
    value &= ~(3 << 8);
    value |= 0xffff0000;
    himm(0xfe02000c, value);

    SetGpioDir(0, 15, 1);

    SetGpioData(0, 15, 1);

    snprintf(cmd, sizeof(cmd), "rmmod /ko/mpu6500.ko");
    mysystem(cmd);
    snprintf(cmd, sizeof(cmd), "insmod /ko/mpu6500.ko");
    mysystem(cmd);
    usleep(1000 * 100);

    m_fd =  open("/dev/mpu6500", O_RDWR);
    if(m_fd < 0)
    {
        ERROR("can't open mpu6500 \n");
        return -1;
    }
    m_Mpu6500Exit = 0;

    m_Mpu6500Thread = std::thread(std::bind(&CCGyro::Fnx_Mpu6500Thread, this));


    m_init = 1;
    return 0;
}

int CCGyro::UnInit()
{
    char cmd[256];
    m_Mpu6500Exit = 1;
    m_Mpu6500Thread.join();
    //sleep(1);//等待线程退出

    snprintf(cmd, sizeof(cmd), "rmmod /ko/mpu6500.ko");
    mysystem(cmd);
    if(m_fd > 0)
    {
        close(m_fd);
        m_fd = 0;
    }
    pthread_mutex_destroy(&m_AngleMutex);
    m_init = 0;
    return 0;
}

int CCGyro::GetGyroAngle(double *picth_angle, double *roll_angle, double *temp)
{
    if(m_init != 1)
    {
        //ERROR("Gyro is no init \n");
        return -1;
    }
    pthread_mutex_lock(&m_AngleMutex);
    *picth_angle = m_PitchAngle;
    *roll_angle = m_rollAngle;
    *temp = m_temp;
    pthread_mutex_unlock(&m_AngleMutex);
    return 0;
}

int CCGyro::SetCalibrationSamples(int samples)
{
    if(m_init != 1)
    {
        ERROR("Gyro is no init \n");
        return -1;
    }
    pthread_mutex_lock(&m_AngleMutex);
    m_CalibrationSample = samples;
    pthread_mutex_unlock(&m_AngleMutex);

    return 0;
}

int CCGyro::GetCalibrationSamples()
{
    if(m_init != 1)
    {
        ERROR("Gyro is no init \n");
        return -1;
    }
    pthread_mutex_lock(&m_AngleMutex);
    int ret = m_CalibrationSample;
    pthread_mutex_unlock(&m_AngleMutex);

    return ret;
}

int CCGyro::SetCalibration(int flag)
{
    if(m_init != 1)
    {
        ERROR("Gyro is no init \n");
        return -1;
    }
    pthread_mutex_lock(&m_AngleMutex);
    m_CalibrationFlag = flag;
    pthread_mutex_unlock(&m_AngleMutex);
    return 0;
}

int CCGyro::GetCalibrationStatus()
{
    if(m_init != 1)
    {
        ERROR("Gyro is no init \n");
        return -1;
    }
    pthread_mutex_lock(&m_AngleMutex);
    int ret = m_CalibrationFlag;
    pthread_mutex_unlock(&m_AngleMutex);
    return ret;
}

int CCGyro::SetGyroAngle(double picth_angle, double roll_angle, double temp)
{
    if(m_init != 1)
    {
        ERROR("Gyro is no init \n");
        return -1;
    }
    pthread_mutex_lock(&m_AngleMutex);
    m_PitchAngle = picth_angle;
    m_rollAngle = roll_angle;
    m_temp = temp;
    pthread_mutex_unlock(&m_AngleMutex);
    return 0;
}

int CCGyro::GetTemp(double *value)
{
    if(m_init != 1)
    {
        ERROR("Gyro is no init \n");
        return -1;
    }
    pthread_mutex_lock(&m_AngleMutex);
    *value = m_temp;
    pthread_mutex_unlock(&m_AngleMutex);
    return 0;
}

int CCGyro::Read_Gyro_data()
{
    if(m_init != 1)
    {
        ERROR("Gyro is no init \n");
        return -1;
    }
    int ret;
    short databuf[7] = {0};
    double accel_x = 0,accel_y = 0,accel_z = 0;  //加速度计
    double gyro_x = 0,gyro_y = 0,gyro_z = 0;     //角速度
    double temp = 0;
    int sample = 0;
    int i;
    // short ax,ay,az;
    // short gx,gy,gz;
    if(m_CalibrationFlag == 1)
    {
        float sum_x = 0.0, sum_y = 0.0, sum_z = 0.0;
        sample = m_CalibrationSample;
        /*校准角速度*/
        for(i = 0; i < sample; ++i)
        {
            ret = read(m_fd, databuf, sizeof(databuf));
            if (ret == 0)
            {
                gyro_x = databuf[4];
                gyro_y = databuf[5];
                gyro_z = databuf[6];
            }
            sum_x += gyro_x;
            sum_y += gyro_y;
            sum_z += gyro_z;
            usleep(500);
        }
        bias_gx = sum_x / sample;
        bias_gy = sum_y / sample;
        bias_gz = sum_z / sample;

        /*校准真实角度*/
        double sum_pitch = 0.0, sum_roll = 0.0;
        for(i = 0; i < sample; ++i)
        {
            ret = read(m_fd, databuf, sizeof(databuf));
            if (ret == 0)
            {
                accel_x = databuf[0];
                accel_y = databuf[1];
                accel_z = databuf[2];
                temp = databuf[3];
                gyro_x = databuf[4] - bias_gx;
                gyro_y = databuf[5] - bias_gy;
                gyro_z = databuf[6] - bias_gz;

                    //转换真实数据
                accel_x = (short)((int)(accel_x * ACCEL_MEASURING_RANGE) >> 15);
                accel_y = (short)((int)(accel_y * ACCEL_MEASURING_RANGE) >> 15);
                accel_z = (short)((int)(accel_z * ACCEL_MEASURING_RANGE) >> 15);
                gyro_x = (short)((int)(gyro_x * GYRO_MEASURING_RANGE) >> 15);
                gyro_y = (short)((int)(gyro_y * GYRO_MEASURING_RANGE) >> 15);
                gyro_z = (short)((int)(gyro_z * GYRO_MEASURING_RANGE) >> 15);

                Axis3f acc = {(double)accel_x, (double)accel_y, (double)accel_z};
                Axis3f gyro = {(double)gyro_x, (double)gyro_y, (double)gyro_z};
                mahony_input(&(MahonyFilter), gyro, acc);
                mahony_update(&(MahonyFilter));
                mahony_output(&(MahonyFilter));
            }
            sum_pitch += MahonyFilter.pitch;
            sum_roll += MahonyFilter.roll;
            usleep(500);
        }
        bias_PitchAngle = sum_pitch / sample;
        bias_RollAngle = sum_roll / sample;


        //SetGyroAngle(MahonyFilter.pitch - bias_PitchAngle, MahonyFilter.roll - bias_RollAngle);
        SetGyroAngle(MahonyFilter.roll - bias_RollAngle, MahonyFilter.pitch - bias_PitchAngle,((temp / 333.87) +21.0));
        m_CalibrationFlag = 0;
        return 0;
    }
    ret = read(m_fd, databuf, sizeof(databuf));
    if(ret == 0)
    {
        accel_x = (double)databuf[0];
        accel_y = (double)databuf[1];
        accel_z = (double)databuf[2];
        temp = (double)databuf[3];
        gyro_x = (double)(databuf[4] - bias_gx);
        gyro_y = (double)(databuf[5] - bias_gy);
        gyro_z = (double)(databuf[6] - bias_gz);
        // ax = databuf[0];
        // ay = databuf[1];
        // az = databuf[2];

        // gx = databuf[4];
        // gy = databuf[5];
        // gz = databuf[6];
        //printf("ax =%d ay=%d az=%d gx =%d gy = %d gz= %d\n",ax,ay,az,gx,gy,gz);
        //printf("bias_gx = %d bias_gy= %d bias_gz = %d \n",bias_gx, bias_gy, bias_gz);
        //printf("bias_PitchAngle = %lf bias_RollAngle= %lf\n",bias_PitchAngle, bias_RollAngle);
        //printf("===============accel_x  = %lf accel_y  = %lf  accel_z  = %lf \n",accel_x, accel_y, accel_z);
        //printf("===============gyro_x  = %lf gyro_y  = %lf  gyro_z  = %lf \n",gyro_x, gyro_y, gyro_z);
        //转换真实数据
        accel_x = (double)((accel_x * ACCEL_MEASURING_RANGE)/ 32768);
        accel_y = (double)((accel_y * ACCEL_MEASURING_RANGE)/ 32768);
        accel_z = (double)((accel_z * ACCEL_MEASURING_RANGE) / 32768);
        gyro_x = (double)((gyro_x * GYRO_MEASURING_RANGE) / 32768);
        gyro_y = (double)((gyro_y * GYRO_MEASURING_RANGE) / 32768);
        gyro_z = (double)((gyro_z * GYRO_MEASURING_RANGE) / 32768);

        //printf("accel_x  = %lf accel_y  = %lf  accel_z  = %lf \n",accel_x, accel_y, accel_z);
        //printf("gyro_x  = %lf gyro_y  = %lf  gyro_z  = %lf \n",gyro_x, gyro_y, gyro_z);
        Axis3f acc = {(double)accel_x, (double)accel_y, (double)accel_z};
        Axis3f gyro = {(double)gyro_x, (double)gyro_y, (double)gyro_z};
        mahony_input(&(MahonyFilter), gyro, acc);
        mahony_update(&(MahonyFilter));
        mahony_output(&(MahonyFilter));
        //SetGyroAngle(MahonyFilter.pitch - bias_PitchAngle, MahonyFilter.roll - bias_RollAngle);  
        SetGyroAngle(MahonyFilter.roll - bias_RollAngle, MahonyFilter.pitch - bias_PitchAngle,((temp / 333.87) +21.0)); //芯片摆放位置关系，俯仰和和横滚互换位置
    }
    return 0;
}
