/*
 * @Author: error: error: git config user.name & please set dead value or install git && error: git config user.email & please set dead value or install git & please set dead value or install git
 * @Date: 2024-05-24 10:34:43
 * @LastEditors: error: error: git config user.name & please set dead value or install git && error: git config user.email & please set dead value or install git & please set dead value or install git
 * @LastEditTime: 2024-08-28 08:42:07
 * @FilePath: \infrared\src\include\avl\CPtzCtrl.h
 * @Description: 这是默认设置,请设置`customMade`, 打开koroFileHeader查看配置 进行设置: https://github.com/OBKoro1/koro1FileHeader/wiki/%E9%85%8D%E7%BD%AE
 */
#ifndef _CPTZ_CTRL_H_
#define _CPTZ_CTRL_H_
#include <pthread.h>
#include <thread>
#include "Cserial.h"

typedef struct target_data
{
    int mode;   //0为角度 1为像素
    int angle_x;
    int angle_y;
    double target_angle;
    void *m_han; 
}target_data;

typedef struct traget_preset_st
{
    char name[64];
    double yaw;
    double pitch;
    int enable;
    int num;
}traget_preset;

class CPtzCtrl
{
private:
    /* data */
    double m_ptzYaw;
    double m_ptzPitch;
    int m_mode;
    int m_ptzstep;
    int m_enable;
    int m_fullPicWidth;
    double m_levelAngle;    //视长角
    Cserial *m_serial;
    unsigned short m_speed;
    int m_fan_enable;
    int m_heating_enable;
    std::thread m_read_uart_hread;
    int m_thread_exit;
    double m_startAngle;
    double m_endAngle;
    double m_targetAngle;
    int m_target_x;
    int m_target_y;
    void *m_han;
    traget_preset m_preset[256];
    int m_preset_size;
    pthread_mutex_t m_lock;
public:
    CPtzCtrl(void *hannle);
    ~CPtzCtrl();
    int Init();
    int UnInit();
    void Fnx_ReadUartThread();
    int SetPtzPitchAngle(double pitch);
    int GetPtzPitchAngle(double *pitch);

    int SetPtzYawAngle(double yaw);
    int GetPtzYawAngle(double *yaw);

    int SetScanMode(int mode);  //1 周扫描 2扇扫描 3定扫描 4onvif
    int GetScanMode(int *mode);

    int SetStep(double step);
    int GetStep(double *step);

    int SetPtzEnable(int enable);
    int GetPtzEnable(int *enable);
    
    int PtzZeroInit();

    int SetTargetLocation(target_data data); //mode 0 是像素，1是度
    int GetTargetLocation(target_data *data);

    int SetFanScanAngle(double startAngle, double endAngle);
    int GetFanScanAngle(double *startAngle, double *endAngle);

    int SetScanSpeed(unsigned short speed);
    int GetScanSpeed(unsigned short *speed);

    int SetFanEnable(int enable);
    int GetFanEnable();

    int SetHeatingEnbale(int enable);
    int GetHeatingEnbale();

    int SetPreset(char *preset_name);
    int GetPreset(traget_preset *presetInfo, int *num);
    int DelPreset(char *preset_name);

    int SetPresetEx(traget_preset *presetInfo, int num);


    int InitSetParamMotor();
    int PtzFullUpdate();
    int pixToAngle(int pixer);
    int AngleToPix(int angle);
    int StartYawElc();      //开启转动水平电机
    int QueryVersion();

    int DataTransm(void *data, int len, void *out);

};

#endif