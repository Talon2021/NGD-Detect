/*
 * @Author: error: error: git config user.name & please set dead value or install git && error: git config user.email & please set dead value or install git & please set dead value or install git
 * @Date: 2024-07-03 17:27:11
 * @LastEditors: error: error: git config user.name & please set dead value or install git && error: git config user.email & please set dead value or install git & please set dead value or install git
 * @LastEditTime: 2024-08-20 17:01:06
 * @FilePath: \Car_ai\jpsdk\inc\jpsdk.h
 * @Description: 这是默认设置,请设置`customMade`, 打开koroFileHeader查看配置 进行设置: https://github.com/OBKoro1/koro1FileHeader/wiki/%E9%85%8D%E7%BD%AE
 */
#ifndef _JP_SDK_H
#define _JP_SDK_H

#ifdef __cplusplus
extern "C"
{
#endif
#define HI_SDK_SUCESS 0 //成功

#define ERR_NO 0x00000000 //无错误
#define ERR_MALLOC_ERROR 0xc0000000 //内存分配失败
#define ERR_INVALID_HANDLE 0xc0000001 //句柄错误
#define ERR_WAIT_TIMEOUT 0xc0000002 //等待超时
#define ERR_INVALID_ARGUMENT 0xc0000003 //参数错误
#define ERR_NOT_SUPPORT 0xc0000004 //不支持功能
#define ERR_SDK_INITFAIL 0xc0000005 //SDK初始化失败
#define ERR_FILE_READ_ERROR 0xC0000006 //文件读取失败
#define ERR_BUFFER_LEN_SHORT 0xC0000007 //内存大小不够
#define ERR_PACKET_NO_DATA 0xC0000008 //包里没有数据
#define ERR_SYS_BUSY 0xC0000009 //系统忙
#define ERR_ENCODENOTSTART 0xC000000A //未启动录像

#define ERR_SDK_NOINIT 0xFFFFFFFF //SDK没有初始化

typedef enum {
    ENC_CIF_FORMAT = 0,
    ENC_QCIF_FORMAT = 1,
    ENC_2CIF_FORMAT = 2,
    ENC_MD_FORMAT = 3,
    ENC_D1_FORMAT = 4,
    ENC_DCIF_FORMAT = 5,
    ENC_SD1_FORMAT = 6,
    ENC_360P_FORMAT = 7, // 640x360
    ENC_QVGA_FORMAT = 8, //QVGA(320*240)
    ENC_VGA_FORMAT = 9, //VGA(640*480)
    ENC_SVGA_FORMAT = 10, //SVGA(800*600)
    ENC_960H_FORMAT = 11, //(960*576)
    ENC_XVGA_FORMAT = 12, //XVGA(1024*768)
    ENC_HD720_FORMAT = 13, //HD720(1280*720)
    ENC_QuadVGA_FORMAT = 14, //QuadVGA(1280*960)
    ENC_SXGA_FORMAT = 15, //SXGA(1280*1024)
    ENC_UXGA_FORMAT = 16, //UXGA(1600*1200)
    ENC_HD1080_FORMAT = 17, //HD1080(1920*1080)
    ENC_QXGA_FORMAT = 18, //QXGA(2304*1296)
    ENC_UWXGA_FORMAT = 19, //UWXGA(2560×1440)
    ENC_USXGA_FORMAT = 20, //USXGA(2592×1944)
    ENC_FORMAT_NUMS
} PictureFormat_t;

typedef enum {
    PktError = 0x00,
    PktIFrames = 0x01,
    PktPFrames = 0x02,
    PktMJPEG = 0x03,
    PktBPFrames = 0x20,
    PktBBPFrames = 0x04,
    PktAudioFrames = 0x08,
    PktQCIFIFrames = 0x10,
    PktQCIFPFrames = 0x40,
    PktBIFrames = 0x90,
    PktBBIFrames = 0xC0,
    PktSQCIFIFrames = 0x11,
    PktSQCIFPFrames = 0x41,
    PktSysHeader = 0x80,
    PktSysHeaderAux = 0x81,
    PktSysHeaderHDAux = 0x82,
    PktMotionDetection = 0x83,
    PktAimDetection = 0x84,
    PktPrivateData = 0x85,
    PktIAFrames = 0x86,
    PktIAFramesForStream = 0x86 + 0xff, //供设备内部使用，用于区分智能事件帧与智能码流帧
    PktOrigImage = 0x87,
    PktFourthIFrames = 0x42,
    PktFourthPFrames = 0x43,
    PktAdvFaceCapFrames = 0xA0,
    PktAdvFaceRecoFrames = 0xA1,
    PktAdvCarCapFrames = 0xA2,
} FrameType_t;

typedef enum {
    H264 = 0x00,
    H265 = 0x04,
    MJPEG = 0x05
} VideoEncodeType_t;
/*=============*/
typedef enum {
    LtH264Video = 0x00,
    LtH265Video = 0x01,
    LtMjpegVideo = 0x02,
    LtH265PlusVideo = 0x03,
    LtH264PlusVideo = 0x04,
} EncodeType_t;

typedef enum {
    LtG726Audio24Kbps = 0x00,
    LtG711AudioALaw = 0x01,
    LtG711AudioULaw = 0x02,
    LtG729Audio = 0x03,
    LtG723Audio1 = 0x04,
    LtMP3Audio = 0x05,
    LtAdpcmAudioIma = 0x06,
    LtAdpcmAudioDvi4 = 0x07,
    LtAmrAudio = 0x08,
    LtAacAudio = 0x09,
} Audio_LoadType_t;


typedef struct CODEC_PARA_S_t {
    PictureFormat_t PictureFormat; //设置编码格式
    int BitRate; //Bit率
    int FrameRate; //帧率
    int PicQualityLever; //编码图像质量，vbr有效
    int BitRateCtlMode; //码率控制类型：0为VBR，1为CBR
    int KeyFrameInterval; //关键帧间隔
} CODEC_PARA_S;
/*=====================*/

typedef struct
{
    FrameType_t frameType;
    unsigned int channel;
    unsigned char* pDataBuf;
    unsigned int uiDataLen;
    unsigned int PTS;
    unsigned int uPTS;
} FRAME_HOOK_INFO;

typedef enum {
    FR_NORMAL = 0,
    FR_ROTATE_180 = 1, //
    FR_HORI_FLIP = 2, //水平镜像
    FR_VERT_FLIP = 3, //垂直镜像
    FR_ROTATE_90 = 4,
    FR_ROTATE_270 = 5,
    FR_MAX = FR_VERT_FLIP,
} FLIP_ROTATE_MODE_E;

typedef enum {
    eSpeechPlayReady = 0, // 准备播放
    eSpeechPlaying = 1, // 正在播放
} E_SpeechPlayStatus;

typedef struct 
{
    int enable;
    int interval_time;
    unsigned int  start_time;       //分钟
    unsigned int  end_time;
    int mode; //0 : 非跨天 1： 跨天          
}AlarmEventCfg;

typedef struct 
{
    int min_temperature;
    int max_temperature;
}TemperatureCfg;

typedef int (*GasDetectResult_CALLBACK)(void* pDataBuf, int uiDataLen, void* userData);

typedef int (*AlarmEvent_CALLBACK)(int type, void *data, void *userData);

int JPSys_Init(int mode);

int JPSys_DeInit();

/*ch 1 2 */
int JPSys_SetheatEnable(int ch, int enbale);

int JPSys_SetMcuPower(int enbale);

int JPSys_SetIrCameraPower(int enbale);

int JPSys_SetVisCameraPower(int enbale);

/*1 发送 0 接收*/
int JPSys_SetPtzUartSwitchPower(int enbale);
/*1 发送 0 接收*/
int JPSys_SetAlarmUartSwitchPower(int enbale);

int JPSys_SetHeatMode(int mode);

int JPSys_RegisterGasDetectResultCb(GasDetectResult_CALLBACK cb, void *userdata);

int JPSys_PushStream(void *stream);

int JPSys_SetAutoTemperaCfg(TemperatureCfg tem_cfg);


#ifdef __cplusplus
}
#endif


#endif