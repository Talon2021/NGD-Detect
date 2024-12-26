/*
 * @Author: error: error: git config user.name & please set dead value or install git && error: git config user.email & please set dead value or install git & please set dead value or install git
 * @Date: 2024-07-03 09:19:37
 * @LastEditors: error: error: git config user.name & please set dead value or install git && error: git config user.email & please set dead value or install git & please set dead value or install git
 * @LastEditTime: 2024-09-02 09:09:47
 * @FilePath: \Car_ai\jpsdk\src\common\meida_common.h
 * @Description: 这是默认设置,请设置`customMade`, 打开koroFileHeader查看配置 进行设置: https://github.com/OBKoro1/koro1FileHeader/wiki/%E9%85%8D%E7%BD%AE
 */
#ifndef _MEDIA_COMMON_H
#define _MEDIA_COMMON_H

#include <errno.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <libgen.h>
#include <semaphore.h>
#include <unistd.h>
#include <sys/time.h>
#include <sys/select.h>

#include "log.h"

#ifndef MS
#define MS(ts) (unsigned int)((ts.tv_sec * 1000) + (ts.tv_nsec / 1000000))
#endif

#define ipc_mem_new0(t, sz) (t*)calloc(sz, sizeof(t));

#define MAX_JPEG_BUFFER_SIZE  (512 * 1024)

#ifndef TRUE
#define TRUE 1
#endif

#ifndef FALSE
#define FALSE 0
#endif

#define MAX_STREAM_NUM 4

#define ISP_FPS_MAX           (30)

#define VI_PIPELINE_CHN      (1)
#define VI_PIPE_DVP_CHN      (0)

#define VI_CHN_BYPASS_NUM    (0)
#define VI_CHN_MAIN_NUM      (1)
#define VI_CHN_ALGO_NUM      (3)

#define VI_INFRARED_DVP      (0)
#define VI_INFRAED_WIDTH     (640)      //640
#define VI_INFRAED_HEIGHT    (512)      //512




#define VI_CHN_ALGO_FPS      (25)
#define VI_CHN_MAIN_FPS      (25)
#define VI_CHN_BYPASS_FPS    (25)


#define VI_FRAME_WIDTH_BYPASS  (2592)
#define VI_FRAME_HEIGHT_BYPASS (1944)

#define VI_FRAME_WIDTH_ALGO  (1280)
#define VI_FRAME_HEIGHT_ALGO (720)

#define VI_FRAME_WIDTH_MAIN  (1024)
#define VI_FRAME_HEIGHT_MAIN (768)
     
#define RGA_CHN_DISPLAY         (0)
#define RGA_CHN_ALGO            (2)
#define RGA_CHN_ENCODE          (1)

#define RGA_CHN_ENCODE_WIDTH    (1280)
#define RGA_CHN_ENCODE_HEIGHT   (720)

#define RGA_CHN_ALGO_WIDTH      (640)//(512)//(640)
#define RGA_CHN_ALGO_HEIGHT     (640)//(512)//(640)

#define RGA_CHN_ALGO_DISPLAY_WIDTH    (1920)
#define RGA_CHN_ALGO_DISPLAY_HEIGHT   (1080)  

#define VENC_CHN_H264           (0)

#define VENC_CHN_H264_WIDTH     (1280)
#define VENC_CHN_H264_HEIGHT    (720)

#define VENC_CHN_H264_FPS        (25)

#define VENC_CHN_JPEG        (1)

#define CAP_JPEG_WIDHT       (1280)
#define CAP_JPEG_HEIGHT      (720)

#define VO_CHN_DISPLAY          (0)
#define VO_FRAME_WIDTH_MAIN  (1080)
#define VO_FRAME_HEIGHT_MAIN (1920)
#define VO_FRAME_FPS         (50)

#define VO_CHN_UI_DISPLAY    (1)

#define AUDIO_IN_CHN_NUM     (0)
#define AUDIO_OUT_CHN_NUM    (0)
#define AUDIO_ENC_CHN_NUM    (0)
#define AUDIO_DEC_CHN_NUM    (0)

#define ALGO_DETECT_RESULT_MAX_SIZE  128

#define MAX_STREAM_NUM 4

#define RV1109_IQFILE_NORMAL_DIR "/oem/etc/iqfiles/"
#define OSD_FONT_FILE                 "/oem/custom/font/Roboto-Black.ttf"

#if 1

#define LOG_DEV_FILE        "/root/app/ai/logs/log_sdk"      // log日志文件，为空表示标准输出打印，不写入日志
#define LOG_PREFIX_STR      "SDK"               // log信息前缀，为空表示默认"LOG"，一般为模块名
#define LOG_SHOW_LEVEL       LOG_DEBUG          // 需要显示的log的等级(小于该值的log信息不会显示和写入日志)
#define LOG_PRT_ENABLE       ENABLE_LOG_CONSOLE // 是否需要输出到终端显示(支持接口动态修改)
#define LOG_MAX_CHAR_COUNT (1024)               // 每行最大的字符个数
#define LOG_FLUSH_TIME_SEC   (30)                // 当前刷新日志文件的时间间隔(log_file非空有效，为0表示不刷新)
#define LOG_IO_BUGGER_SIZE   (1024)                // IO缓冲区大小(log_file非空有效, 为0表示不设置，使用系统默认buffer)


extern void *g_pSdkLogHandler;
extern log_param_t g_log_param;

#ifndef SDK_DBG
#define SDK_DBG(format, args...) do { \
    if ((!g_pSdkLogHandler)) { \
        g_pSdkLogHandler = log_init(&g_log_param); \
    }\
    if (NULL == g_pSdkLogHandler){ \
        fprintf(stderr, "Fail to init log module!\n"); \
    } else { \
        log_print(g_pSdkLogHandler, LOG_DEBUG, "[%s-%d] " format, basename((char *)__FILE__), __LINE__, ##args); \
    } \
} while (0)
#endif 

#ifndef SDK_INF
#define SDK_INF(format, args...) do { \
    if ((!g_pSdkLogHandler)) { \
        g_pSdkLogHandler = log_init(&g_log_param); \
    } \
    if (NULL == g_pSdkLogHandler){ \
        fprintf(stderr, "Fail to init log module!\n"); \
    } else { \
        log_print(g_pSdkLogHandler, LOG_INFO, "[%s-%d] " format, basename((char *)__FILE__), __LINE__, ##args); \
    } \
} while (0)
#endif 

#ifndef SDK_WARN
#define SDK_WARN(format, args...) do { \
    if ((!g_pSdkLogHandler)) { \
        g_pSdkLogHandler = log_init(&g_log_param); \
    } \
    if (NULL == g_pSdkLogHandler) { \
        fprintf(stderr, "Fail to init log module!\n"); \
    }else { \
        log_print(g_pSdkLogHandler, LOG_WARN, "[%s-%d] " format, basename((char *)__FILE__), __LINE__, ##args); \
    } \
} while (0)
#endif 

#ifndef SDK_ERR
#define SDK_ERR(format, args...) do { \
    if ((!g_pSdkLogHandler)) { \
        g_pSdkLogHandler = log_init(&g_log_param); \
    } \
    if (NULL == g_pSdkLogHandler) { \
        fprintf(stderr, "Fail to init log module!\n"); \
    } else { \
        log_print(g_pSdkLogHandler, LOG_ERROR, "[%s-%d] " format, basename((char *)__FILE__), __LINE__, ##args); \
    } \
} while (0)
#endif

#ifndef SDK_PRT_ENABLE
#define SDK_PRT_ENABLE(enable) do { \
    if ((!g_pSdkLogHandler)) { \
        g_pSdkLogHandler = log_init(&g_log_param); \
    } \
    if (NULL == g_pSdkLogHandler) { \
        fprintf(stderr, "Fail to init log module!\n"); \
    } else { \
        log_print_console_enable(g_pSdkLogHandler, (enable)); \
    } \
} while (0)
#endif

#endif


#ifndef ALIGN_UP
#define ALIGN_UP(x, a) ((((x) + ((a)-1)) / a) * a)
#endif

#ifndef ALIGN_DOWN
#define ALIGN_DOWN(x, a) (((x) / (a)) * (a))
#endif



typedef unsigned int        uint32_t;
typedef int                 sint32_t;
typedef char                sint8_t;
typedef unsigned long long  uint64t;

#ifndef BOOL_T
#define BOOL_T
//typedef int BOOL;
#define BOOL int
#endif

#ifndef UInt32_T
#define UInt32_T
//typedef unsigned int UInt32;
#define UInt32 unsigned int
#endif

#define ALGPRINT(format, args...) do{printf("[ALGPRINT][%s-%d]" format, __FILE__, __LINE__, ##args);}while(0)
#ifndef TimerSeconds
// select实现秒级定时器
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

#define FREE_STR(str)  \
    do                 \
    {                  \
        if (str)       \
            free(str); \
        str = NULL;    \
    } while (0)


#define SDK_ALLOC(ptr, type ,num)                                                \
    do                                                                     \
    {                                                                      \
        ptr = NULL;                                                        \
        ptr = (type *)calloc(1, num);                                              \
        if (!(ptr))                                                        \
        {                                                                  \
            SDK_ERR("Fail to calloc ptr: %s.\n", strerror(errno));         \
        }                                                                  \
    } while (0)


#define SDK_FREE(a)     \
    do                  \
    {                   \
        if ((a))        \
        {               \
            free((a));  \
            (a) = NULL; \
        }               \
    } while (0)



#define FRAME_START_CODE (0xB7010000)

typedef struct tagH264DataPacketHeader {
    unsigned int uDataPacketStartCode; //0xB7010000
    unsigned int uFrameNo;
    unsigned char byFrameType;
    unsigned char byLoadType;
    unsigned int uShowTime;
    unsigned long u32RealTimeSec;
    unsigned long u32RealTimeUSec;
    unsigned short uReserved;
    unsigned int uOverloadLen;
} __attribute__((packed)) H264DATAPACKETHEADER, *PH264DATAPACKETHEADER;

/* 自定义读写锁实现 */
typedef enum {
    HY_UNLOCK = 0,
    HY_RDLOCK = 1,
    HY_WRLOCK = 2,
} HY_RWLockState_t;

typedef struct
{
    pthread_mutex_t m_Lock;
    UInt32 m_ReadLockNum;
    UInt32 m_WriteLockNum;
    HY_RWLockState_t m_LockState;
} HY_RWLodk_t;

typedef enum {
    NormalMode = 0, //普通模式(编解码)
    UpgradeMode = 1, //升级模式
} enSdkMode;

typedef struct tagJpegData{
    size_t size;
    void *data;
}JpegData_t;

typedef struct
{
    int m_Exit;
    pthread_t m_ThreadId; //线程ID
    int m_Pri;
} ThreadInfo_t;

typedef struct
{
    unsigned int loadType;
    int frameType;
    unsigned int channel;
    unsigned char* pDataBuf;
    unsigned int uiDataLen;
    unsigned int pts_us;
} FRAME_HOOK_INFO_LOCAL;


typedef enum algo_type
{
    ALGO_HUMAN,
    ALGO_CAR,
    ALGO_ANIMAL
}algo_type;

typedef struct ALARM_INFO_ST
{
    int event_type;
    unsigned long long event_time;
    int Alarm_Strength;     //报警强度 如 远距离报警，近距离报警     
    //扩展
}ALARM_INFO;

typedef enum{
    SPEECH_HUMAN_ALARM = 0,  //人形报警
    SPEECH_CAT_ALARM,
    SPEECH_ANIMAL_ALARM,

    SPEECH_ANIMAL_ALL
} SpeechType_T;

typedef enum AlarmTypeAudio
{
    ALERT_CROSS_LINE = 0,   //撞线
    ALERT_INVASION,     //入侵
    ALERT_BEHAVIOR,     //行为
    ALERT_FIRE,         //火
    ALERT_TEMP,         //温度
    ALERT_CIRCUIT,      //电路

    ALRET_HUAMN = 10,
    ALRET_CAR,
    ALERT_ANIMAL,

    ALERT_CUSTOM = 20,
    AlERT_NULL
}AlarmTypeAudio;

#define MAX_GAS_NUM             (32)
/* 注意：是以最上边框和最左边框为基准的,均按照百分比 */
typedef struct
{
    int left;     /* 区域的左边与图像的最左边的距离 */
    int top;      /* 区域的上边与图像的最上边的距离 */
    int right;    /* 区域的右边与图像的最左边的距离 */
    int bottom;   /* 区域的下边与图像的最上边的距离 */
} DETECT_RECTH;

typedef struct _human_attr
{
    unsigned char u8Valid;    // 该索引是否有效
    DETECT_RECTH stGasRecth;
} GAS_ATTR;

typedef struct _gas_detect_result
{
    uint64_t frame_id;
    unsigned char u8GasNum;
    GAS_ATTR stGasAttr[MAX_GAS_NUM];
    unsigned char *gas_mask;

}GAS_DETECT_RESULT;

typedef int (*ALGODETECTCALLBACK)(void *data, int datalen);   

uint64_t GetSystemBootDuratingMSecond(void);

int mysystem(const char *cmdstring);

int HY_Res_SDK_Lock();

int HY_Res_SDK_UnLock();

void HY_RES_SDK_init();

int Comm_CreateThread(ThreadInfo_t* pThInfo, int Pri, void* (*start_routine)(void*));

int Comm_DestroyThread(ThreadInfo_t* pThInfo);
#endif