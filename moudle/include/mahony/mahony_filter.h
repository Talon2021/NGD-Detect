/*
 * @Author: error: error: git config user.name & please set dead value or install git && error: git config user.email & please set dead value or install git & please set dead value or install git
 * @Date: 2024-05-15 17:06:22
 * @LastEditors: error: error: git config user.name & please set dead value or install git && error: git config user.email & please set dead value or install git & please set dead value or install git
 * @LastEditTime: 2024-05-16 16:39:27
 * @FilePath: \panoramic_code\src\moudle\include\mahony\mahony_filter.h
 * @Description: 这是默认设置,请设置`customMade`, 打开koroFileHeader查看配置 进行设置: https://github.com/OBKoro1/koro1FileHeader/wiki/%E9%85%8D%E7%BD%AE
 */
#ifndef _MAHONY_FILTER_H
#define _MAHONY_FILTER_H

#ifdef __cplusplus
extern "C"
{
#endif

#define DEG2RAD 0.01745329251994329577f
#define RAD2DEG 57.2957795130823208768f

// #include "Tiger_math.h"

/*************************************
完成时间：2023年09月02日 
功能介绍：实现mahony姿态角解算算法的模块封装
知乎账号：龙胆也
B站账号：华南小虎队
***************************************/

struct MAHONY_FILTER_t;

typedef struct _Axis3f
{
    double x;
    double y;
    double z;
}Axis3f;

// 定义 MAHONY_FILTER_t 结构体，用于封装 Mahony 滤波器的数据和函数
struct MAHONY_FILTER_t
{
    // 输入参数
    float Kp, Ki;          // 比例和积分增益
    float dt;              // 采样时间间隔
    Axis3f  gyro, acc;     // 陀螺仪和加速度计数据

    // 过程参数
    float exInt, eyInt, ezInt;                // 积分误差累计
    float q0, q1, q2, q3;            // 四元数
    float rMat[3][3];               // 旋转矩阵

    // 输出参数
    float pitch, roll, yaw;         // 姿态角：俯仰角，滚转角，偏航角

    // 函数指针
    void (*mahony_init)(struct MAHONY_FILTER_t *mahony_filter, float Kp, float Ki, float dt);
    void (*mahony_input)(struct MAHONY_FILTER_t *mahony_filter, Axis3f gyro, Axis3f acc);
    void (*mahony_update)(struct MAHONY_FILTER_t *mahony_filter);
    void (*mahony_output)(struct MAHONY_FILTER_t *mahony_filter);
    void (*RotationMatrix_update)(struct MAHONY_FILTER_t *mahony_filter);
};




// 函数声明
void mahony_init(struct MAHONY_FILTER_t *mahony_filter, float Kp, float Ki, float dt);          // 初始化函数
void mahony_input(struct MAHONY_FILTER_t *mahony_filter, Axis3f gyro, Axis3f acc);              // 输入数据函数
void mahony_update(struct MAHONY_FILTER_t *mahony_filter);                                      // 更新滤波器函数
void mahony_output(struct MAHONY_FILTER_t *mahony_filter);                                      // 输出姿态角函数
void RotationMatrix_update(struct MAHONY_FILTER_t *mahony_filter);                              // 更新旋转矩阵函数
#ifdef __cplusplus
}
#endif

#endif