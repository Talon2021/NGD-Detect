/*
 * @Author: error: error: git config user.name & please set dead value or install git && error: git config user.email & please set dead value or install git & please set dead value or install git
 * @Date: 2024-05-14 15:33:32
 * @LastEditors: error: error: git config user.name & please set dead value or install git && error: git config user.email & please set dead value or install git & please set dead value or install git
 * @LastEditTime: 2024-07-04 15:55:25
 * @FilePath: \panoramic_code\src\common\common.h
 * @Description: 这是默认设置,请设置`customMade`, 打开koroFileHeader查看配置 进行设置: https://github.com/OBKoro1/koro1FileHeader/wiki/%E9%85%8D%E7%BD%AE
 */
#ifndef _COMMON_H_
#define _COMMON_H_

#ifdef __cplusplus
extern "C"
{
#endif

#include <stdbool.h>
#include <pthread.h>
#include <libgen.h>
#include <stdio.h>
#include <sys/select.h>
#include <errno.h>
#define SDK_FREE(a)     \
    do                  \
    {                   \
        if ((a))        \
        {               \
            free((a));  \
            (a) = NULL; \
        }               \
    } while (0)

#ifndef ALGPRINT
#define ALGPRINT(x...)    do{printf("[ALG PRT][%s:%d:%s]", basename((char *)__FILE__),__LINE__,__FUNCTION__);printf(x);}while(0)
#endif 

#ifndef TimerSeconds
static inline void TimerSeconds(unsigned long seconds)
{
    if (seconds < 0)
    {
        return;
    }

    int err = -1;
    struct timeval tv = { 0, 0 };

    tv.tv_sec = seconds;
    tv.tv_usec = 0;

    do
    {
        err = select(0, NULL, NULL, NULL, &tv);
    } while ((err < 0) && (EINTR == errno));
}
#endif

#ifndef TimerMilliseconds
// select实现毫秒级别定时器
static inline void TimerMilliseconds(unsigned long mSec)
{
    if (mSec < 0)
    {
        return;
    }

    int err = -1;
    struct timeval tv = { 0, 0 };

    tv.tv_sec = mSec / 1000;
    tv.tv_usec = (mSec % 1000) * 1000;

    do
    {
        err = select(0, NULL, NULL, NULL, &tv);
    } while ((err < 0) && (EINTR == errno));
}
#endif

#ifndef TimerMicroseconds
// select实现微妙级别定时器
static inline void TimerMicroseconds(unsigned long uSec)
{
    if (uSec < 0)
    {
        return;
    }

    int err = -1;
    struct timeval tv = { 0, 0 };

    tv.tv_sec = uSec / 1000000;
    tv.tv_usec = uSec % 1000000;

    do
    {
        err = select(0, NULL, NULL, NULL, &tv);
    } while ((err < 0) && (EINTR == errno));
}
#endif

/* select精确睡眠(秒) */
#define SelectSleepSec(X) TimerSeconds(X)
/* select精确睡眠(毫秒) */
#define SelectSleepMSec(X) TimerMilliseconds(X)
/* select精确睡眠(微妙) */
#define SelectSleepUSec(X) TimerMicroseconds(X)


#define JP_NVR_GET_DEVICEINFO       1   //主命令
#define JP_NVR_SET_DEVICEINFO       2

#define DEVICE_ALL_ABLITY           0XFFFFFFFF
#define DEVICE_SYSTEM_INFO          1

#define JP_NVR_GET_CONFIG           3 //主命令
#define JP_NVR_SET_CONFIG           4

#define CONFIG_SYSTEM_INFO          1
#define CONFIG_CAMERA_INFO          2
#define CONFIG_POSITION_INFO        3
#define CONFIG_CALIBRATION_INFO     4
#define CONFIG_PTZ_INFO             5
#define CONFIG_AREAS_INFO           6
#define CONFIG_OTHER_INFO           7
#define CONFIG_NETWORK_INFO         8
#define CONFIG_ALGO_INFO            9
#define CONFIG_PTZ_PRESET           10

#define JP_NVR_CONTORL              5 //主命令

#define CONTORL_SNAP                1
#define CONTORL_RECORD              2
#define CONTORL_BAD_PIX             3
#define CONTORL_RANG_DISTANCE       4
#define CONTORL_TRACK               5
#define CONTORL_SHUTTER_CALIBRATION 6
#define CONTORL_SHUTTER_PTZ         7
#define CONTORL_INFRARED_ELECTRIC_FOCUS  8

#define JP_PTZ_CONTORL              6 //主命令

#define JP_PTZ_CTRL_UP              1
#define JP_PTZ_CTRL_DOWN            2
#define JP_PTZ_CTRL_LEFT            3
#define JP_PTZ_CTRL_RIGHT           4
#define JP_PTZ_CTRL_HOME            5
#define JP_PTZ_CTRL_PRESET          6

/*========================================process code============================================*/

#define IR_PSEUDO_CONFIG_CODE          510
#define IR_ZOOM_CONFIG_CODE            511
#define SCREE_BRIGHTNESS_CODE          512
#define SCREE_CONTRAST_CODE            513
#define GAS_ENHANCEMENT_CODE           514
#define IR_PIC_BRIGHTNESS_CODE         515
#define IR_PIC_CONTRAST_CODE           516 
#define AUTO_FILL_LIGHT_CODE           520
#define WIPERS_CODE                    521
#define CVBS_CODE                      522
#define IR_AUTO_FOCU_CODE              523
#define VIS_AUTO_FOCU_CODE             524
#define VIS_PIC_BRIGHTNESS_CODE        525
#define VIS_PIC_CONTRAST_CODE          526
#define VIS_PIC_STAURATION_CODE        527
#define VIS_PIC_SHARPNESS_CODE         528

#define DEV_RTSP_URL_CODE              531
#define DEV_VERSION_CODE               530     

#define IR_ELEC_FOCU_CODE              600
#define FACTORY_RESET_CODE             601
#define DEV_FORMAT_CODE                602


/* 线程管理资源 */
typedef struct
{
    int m_Exit;
    pthread_t m_ThreadId; //线程ID
    int m_Pri;
} ThreadInfo_t;

typedef struct tagH264DataPacketHeader
{
    unsigned int        uDataPacketStartCode; //固定为0xB7010000
    unsigned int        uFrameNo;				//媒体数据包的序号(音视频独立编号)
    unsigned char       byFrameType;
    unsigned char       byLoadType;				//媒体数据包的信息,具体含义依赖于帧类型
    unsigned int        uShowTime;				//表示媒体数据包相对于上一帧的偏差时间(频率90Khz)
    unsigned long       u32RealTimeSec;
    unsigned long       u32RealTimeUSec;
    unsigned short      uReserved;
    unsigned int        uOverloadLen;			//表示媒体数据包的数据长度
}__attribute__((packed)) H264DATAPACKETHEADER, *PH264DATAPACKETHEADER;

int Comm_CreateThread(ThreadInfo_t* pThInfo, int Pri, void* (*start_routine)(void*));

int Comm_DestroyThread(ThreadInfo_t* pThInfo);

int get_bit(int *data, int index);

void set_bit(int *data, int index, bool ch);

int mysystem(const char *cmdstring);

unsigned long long get_ms();

int get_localip(const char * eth_name, char *local_ip_addr);
int get_localgetway(const char * eth_name, char *netway);
int get_localmask(const char * eth_name, char *msak);
int get_localmac(const char * eth_name, char *mac);
#ifdef __cplusplus
}
#endif

#endif