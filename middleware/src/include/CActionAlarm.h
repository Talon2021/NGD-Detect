/*
 * @Author: error: error: git config user.name & please set dead value or install git && error: git config user.email & please set dead value or install git & please set dead value or install git
 * @Date: 2024-05-17 10:15:41
 * @LastEditors: error: error: git config user.name & please set dead value or install git && error: git config user.email & please set dead value or install git & please set dead value or install git
 * @LastEditTime: 2024-06-19 09:07:07
 * @FilePath: \panoramic_code\src\include\avl\CActionAlarm.h
 * @Description: 这是默认设置,请设置`customMade`, 打开koroFileHeader查看配置 进行设置: https://github.com/OBKoro1/koro1FileHeader/wiki/%E9%85%8D%E7%BD%AE
 */
#ifndef __ACTION_ALARM_H_
#define __ACTION_ALARM_H_

#include <time.h>
#include <pthread.h>


typedef enum
{
	ALARM_EVENT,
}event_type;

typedef enum 
{
    ALARM_TYPE_OVER_BOUNDARY,            // 越界  
    ALARM_TYPE_AREA_INTRUDE,             // 区域入侵
    ALARM_TYPE_ABNORMAL_ACTION,          // 异常行为
    ALARM_TYPE_FIRE_POINT_DETECTION,     // 火点检测
    ALARM_TYPE_TEMPERATURE_ABNORMAL,     // 温度异常
    ALARM_TYPE_VOLTAGE_ABNORMAL,         // 电压异常
    ALARM_TYPE_AMPERE_ABNORMAL,          // 电流异常
    ALARM_TYPE_GAS_LEAKAGE,          // 气体泄漏
}alarm_type;

typedef struct _alarm_data_st
{
	int vaild;
	int type;
	unsigned int timeTamp;
}alarm_data;
typedef int (*ALARMCALLBACK)(alarm_data *info);

class CActionAlarm
{
private:
    ALARMCALLBACK m_FnxalarmCallback;
    int m_bEnable;
	pthread_mutex_t m_lock;
	void *m_han;
public:
    CActionAlarm(void *handle, int ch);
    ~CActionAlarm();
    int Init();
    int UnInit();
    int EnableAction(int Action, int bEnable);
    int SetAlarmActionCallback(ALARMCALLBACK callback);
	int PushEnevt(int type, void *buffer);
};






#endif