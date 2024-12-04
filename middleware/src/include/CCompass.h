/*
 * @Author: error: error: git config user.name & please set dead value or install git && error: git config user.email & please set dead value or install git & please set dead value or install git
 * @Date: 2024-05-16 14:58:39
 * @LastEditors: error: error: git config user.name & please set dead value or install git && error: git config user.email & please set dead value or install git & please set dead value or install git
 * @LastEditTime: 2024-06-20 15:06:45
 * @FilePath: \panoramic_code\src\include\avl\CCompass.h
 * @Description: 这是默认设置,请设置`customMade`, 打开koroFileHeader查看配置 进行设置: https://github.com/OBKoro1/koro1FileHeader/wiki/%E9%85%8D%E7%BD%AE
 */
#ifndef __CCOMPASS_H_
#define __CCOMPASS_H_

#include <pthread.h>
#include "mahony_filter.h"
#include "CCGyro.h"
#include <thread>
// 定义一个结构体来保存滤波器的状态
#define LSM303SORTLEN 10
typedef struct
{
    double sort[LSM303SORTLEN]; // 滤波数组
    int index;                  // 当前索引
    double sum;                 // 数值的累加和
    bool initialized;           // 是否初始化完成
} MovingAverageFilter;


class CCompass
{
private:
    double m_yawAngle;
    int m_samples;
    int m_fd;
    int m_init;
    
    MovingAverageFilter compassFilterx;
    MovingAverageFilter compassFiltery;
    
    bool filterInitializedx;

    //MAHONY_FILTER_t MahonyFilter;

    int calibration_flag;
    double calibration_offset_angle;

    pthread_mutex_t m_lock;
    std::thread m_Lsm303Thread;
    int m_Lsm303Exit;
    void *m_han;
public:
    
    CCompass(void *handle, int ch);
    ~CCompass();
    int Init();
    int UnInit();
    int Read_Lsm303_data();

    int GetYawAngle(double *value);
    int SetYawAngel(double value);
    int SetCalibrationSamples(int value);
    int GetCalibrationSamples(int *value);
    int SetCalibrationFlag(int flag);
    int GetCalibrationFlag(int *flag);
    void Fnx_Lsm303Thread();

};


#endif
