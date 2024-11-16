/**
 * @file connect.h
 * @author your name (you@domain.com)
 * @brief 上层进行回调 设置参数
 * @version 0.1
 * @date 2024-06-04
 * 
 * @copyright Copyright (c) 2024
 * 
 */
#ifndef _CONNECT_H_
#define _CONNECT_H_

#include <iostream>
#include <vector>

using namespace std;


// 算法的开关设置
typedef struct AlgorithmSwitch{
    bool TargetDetectionSwitch;
    bool TargetTrackSwitch;
    bool ActivityAnalysisSwitch;
}AlgorithmSwitch;

// points设置
typedef struct AreaPoint{
    int x;
    int y;
}AreaPoint;




void*  SetAlgorithmInit(std::vector<std::vector<AreaPoint>>polygonInit);      //设置算法初始化

int SetAlgorithmSwitch(void (*fp),AlgorithmSwitch AlgorithmSwitchTypeSet); //设置算法开关



int SetMotorAngle(void (*fp),float yawSet,float pitchSet);          //设置电机角度


int SetMotorStartType(void (*fp),bool StartTypeSet);      //设置算法开始状态
 

int SetPositiveOrNegative(void (*fp),bool PositiveOrNegativeSet);   //设置电机正反转


int SetMonitorArea(void (*fp),std::vector<std::vector<AreaPoint>>polygon);            //设置算法监控区域


int SetStepAngle(void (*fp),float StepAngle) ;              //设置算法中电机的步进角度


int SetScanModel(void (*fp),int ScanModel);               //设置算法中电机的步进角度


#endif // _CONNECT_H_