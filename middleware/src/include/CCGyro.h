/*
 * @Author: error: error: git config user.name & please set dead value or install git && error: git config user.email & please set dead value or install git & please set dead value or install git
 * @Date: 2024-05-13 15:59:10
 * @LastEditors: error: error: git config user.name & please set dead value or install git && error: git config user.email & please set dead value or install git & please set dead value or install git
 * @LastEditTime: 2024-05-27 15:06:45
 * @FilePath: \panoramic_code\src\include\avl\CCGyro.h
 * @Description: 这是默认设置,请设置`customMade`, 打开koroFileHeader查看配置 进行设置: https://github.com/OBKoro1/koro1FileHeader/wiki/%E9%85%8D%E7%BD%AE
 */
#ifndef CCGYRO_H
#define CCGYRO_H

#include <pthread.h>
#include "mahony_filter.h"
#include <thread>
#define G_Kp 10.0f      // 比例增益支配率收敛到加速度计/磁强计
#define G_Ki 0.001f    // 积分增益支配率的陀螺仪偏见的衔接
#define ACCEL_MEASURING_RANGE       2.0f        //加速度量程
#define GYRO_MEASURING_RANGE        250.0f        //角速度量程

class CCGyro
{
private:
    double m_PitchAngle;
    double m_rollAngle;
    int m_CalibrationSample;
    int m_init;
    double m_temp;
    int m_CalibrationFlag;
    pthread_mutex_t m_AngleMutex;
    int m_fd;
    MAHONY_FILTER_t MahonyFilter;
    short bias_gx, bias_gy, bias_gz;
    double bias_PitchAngle, bias_RollAngle;
    
    std::thread m_Mpu6500Thread;
    int m_Mpu6500Exit;
    void *m_han;
    
public:
    
    CCGyro(void *handle, int ch);
    ~CCGyro();
    int Init();

    int UnInit();

    int GetGyroAngle(double *picth_angle, double *roll_angle, double *temp);

    int SetCalibrationSamples(int samples);

    int GetCalibrationSamples();

    int SetCalibration(int flag);

    int GetCalibrationStatus();
    
    int SetGyroAngle(double picth_angle, double roll_angle, double temp);

    int GetTemp(double *value);
    
    int Read_Gyro_data();

    void Fnx_Mpu6500Thread();
};





#endif