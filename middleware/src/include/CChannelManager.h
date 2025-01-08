/*
 * @Author: error: error: git config user.name & please set dead value or install git && error: git config user.email & please set dead value or install git & please set dead value or install git
 * @Date: 2024-05-13 16:01:48
 * @LastEditors: error: error: git config user.name & please set dead value or install git && error: git config user.email & please set dead value or install git & please set dead value or install git
 * @LastEditTime: 2024-07-29 15:42:47
 * @FilePath: \panoramic_code\src\peripaherls\CChannelManager.h
 * @Description: 这是默认设置,请设置`customMade`, 打开koroFileHeader查看配置 进行设置: https://github.com/OBKoro1/koro1FileHeader/wiki/%E9%85%8D%E7%BD%AE
 */
#ifndef _CCHANNEL_MANAGER_H
#define _CCHANNEL_MANAGER_H
#include "CConfig.h"
#include "CCInfraredImage.h"
#include <pthread.h>
#include "CCGyro.h"
#include "CCAreaInvasion.h"
#include "CCompass.h"
#include "CActionAlarm.h"
#include "CPtzCtrl.h"
#include "CExtctrl.h"
#include "jpsdk.h"
#include "CCoder.h"
#include "CVisLightImage.h"
#define MAX_CH_NUM                      (1)

typedef struct _peripheral_info
{
    int vaild;
    double yaw;
    double pitch;
    double roll;
    double temp;
    double vol;
    double ampere;
    double longitude;
    double latitude;
}peripheral_info_st;

typedef int (*peripheralInfoCallback)(peripheral_info_st p_info);


class CChannelManager
{
private:
    CCInfraredImage *m_infraredImage[MAX_CH_NUM];
    CCGyro *m_ccgyro[MAX_CH_NUM];
    CCAreaInvasion *m_areahannle[MAX_CH_NUM];
    CCompass *m_compasshannel[MAX_CH_NUM];
    CActionAlarm *m_Actionhannel[MAX_CH_NUM];
    CPtzCtrl *m_Ptzhannel[MAX_CH_NUM];
    CCoder *m_coderhannel[MAX_CH_NUM];
    CExtctrl *m_ExtCtrl[MAX_CH_NUM];
    CVisLightImage *m_VisLightImage[MAX_CH_NUM];        
    static CChannelManager *pCChannelManger;
    int m_iEncChannelCnt;
    int m_bInit;

    pthread_t m_PeripheralReportThread;
    int m_reportflag;
    int m_reportExit;
    void *m_han;

    int (*m_FnxAvCallBack) (int nCh, unsigned char *Buff, int nBuffLen);
public:

    peripheralInfoCallback m_callback;

    CChannelManager();
    ~CChannelManager();

    int Init();

    int UnInit();

    int SetAVCallBack(int (*AvCallBack) (int nCh, unsigned char *Buff, int nBuffLen));

    int RVAvReadStream(FRAME_HOOK_INFO *param);
    
    static int FrameHook_FNX(FRAME_HOOK_INFO *param);

    static CChannelManager *GetInstanceHandle();

    static void Release();

    int GetperipheralInfo(peripheral_info_st *info);

    int SetperiphearaEnable(int enable);

    int SetMpuCallback(peripheralInfoCallback callback);
    //int AvCallBack_FNX()

    int InfraredImage_Init(int nch);

    int InfraredImage_UnInit(int nch);

    int InfraredImage_SetImageBrightness(int nch, int value);

    int InfraredImage_GetImageBrightness(int nch, int *value);

    int InfraredImage_SetImageContrast(int nch, int value);

    int InfraredImage_GetImageContrast(int nch, int *value);

    int InfraredImage_SetHotspotTracking(int nch, int value);

    int InfraredImage_GetHotspotTracking(int nch, int *value);

    int InfraredImage_ManualDefectRemoval(int nch, int opt, int value);

    int InfraredImage_GetManualDefectRemoval(int nch, int *opt, int *value);

    int InfraredImage_SetPictureInPictureSwitch(int nch, int value);

    int InfraredImage_GetPictureInPictureSwitch(int nch, int *value);

    int InfraredImage_SetInfraredImageSharpening(int nch, int ddelv);

    int InfraredImage_GetInfraredImageSharpening(int nch, int *ddelv);

    int InfraredImage_SetInfraredImagePolarity(int nch, int value);
    
    int InfraredImage_GetInfraredImagePolarity(int nch, int *value);

    int InfraredImage_InfraredImageSaveCmd(int nch);

    int InfraredImage_SetInfraredImagePAL(int nch, int status);
    
    int InfraredImage_GetInfraredImagePAL(int nch, int *status);

    int InfraredImage_SetInfraredImageElectronicZoom(int nch, float value);

    int InfraredImage_GetInfraredImageElectronicZoom(int nch, float *value);

    int InfraredImage_SetInfraredImageFocusMode(int nch, int mode);

    int InfraredImage_GetInfraredImageFocusMode(int nch, int *mode);


    int InfraredImage_SetGasEnhanced(int nch, int enable);

    int InfraredImage_GetGasEnhanced(int nch, int *enable);

    int InfraredImage_SetElectricFocu(int nch, int action);

    int InfraredImage_SetAutoFocuData(int nch, int type);

    int InfraredImage_SetImagesaturation(int nch, int value);

    int InfraredImage_GetImagesaturation(int nch, int *value);

    int InfraredImage_RegisterIrCtrlCb(int nch, IRControlFunctions cb);

    /*陀螺仪*/
    int Groy_Init(int nch);

    int Groy_UnInit(int nch);

    int Groy_GetGyroAngle(int nch, double *picth_angle, double *roll_angle, double *temp);

    int Groy_SetCalibrationSamples(int nch, int samples);

    int Groy_GetCalibrationSamples(int nch);

    int Groy_SetCalibration(int nch, int flag);

    int Groy_GetCalibrationStatus(int nch);
    
    int Groy_SetGyroAngle(int nch,double picth_angle, double roll_angle, double temp);

    int Groy_GetTemp(int nch, double *value);

    /*区域入侵*/

    int Area_Init(int nch);

    int Area_UnInit(int nch);

    int Area_SetAreaPoint(int nch, area_information area);

    int Area_SetAllAreaPoint(int nch, area_information *area, int num);

    int Area_GetAreaPoint(int nch, area_information *area, int *num);
    
    int Area_DeleteAreaPoint(int nch, const char *area_name);

    int Alg_SetDetectEnable(int nch, int enable);

    int Alg_SetTrackEnable(int nch, int enable);

    int Alg_SetBehaviorEnable(int nch, int enable);

    int Alg_GetAlgEnable(int nch, int *enable);

    int Alg_SetDetectTrackId(int nch, std::vector<long> track_id);
    
    int Alg_GetDetectTrackId(int nch, std::vector<long> track_id);

    int Alg_SetDetectGasEnable(int nch, int enable);

    int Alg_GetDetectGasEnable(int nch, int *enable);

    int Alg_RegisterGasResultCb(int nch, DetectResult_CALLBACK cb);

    /*电子罗盘*/
    int Compass_Init(int nch);

    int Compass_UnInit(int nch);

    int Compass_GetYawAngle(int nch, double *value);

    int Compass_SetCalibrationSamples(int nch, int value);

    int Compass_GetCalibrationSamples(int nch, int *value);

    int Compass_SetCalibrationFlag(int nch, int flag);

    int Compass_GetCalibrationFlag(int nch, int *flag);

    /*报警信息*/
    int Alarm_Init(int nch);

    int Alarm_UnInit(int nch);

    int Alarm_EnableAction(int nch, int Action, int bEnable);

    int Alarm_SetAlarmActionCallback(int nch, ALARMCALLBACK callback);
    
    /*ptz*/
    int Ptz_Init(int nch);

    int Ptz_UnInit(int nch);

    int Ptz_SetPtzAngle(int nch, double yaw, double pitch);

    int Ptz_GetPtzAngle(int nch, double *yaw, double *pitch);

    int Ptz_SetScanMode(int nch, int mode);

    int Ptz_GetScanMode(int nch, int *mode);

    int Ptz_SetStep(int nch, double step);

    int Ptz_GetStep(int nch, double *step);

    int Ptz_SetPtzEnable(int nch, int enable);
    
    int Ptz_GetPtzEnable(int nch, int *enable);

    int Ptz_ZeroInit(int nch);

    int Ptz_SetTargetLocation(int nch, target_data data);

    int Ptz_GetTargetLocation(int nch, target_data *data);

    int Ptz_SetFanScanAngle(int nch, double startAngle, double endAngle);

    int Ptz_GetFanScanAngle(int nch, double *startAngle, double *endAngle);

    int Ptz_SetScanSpeed(int nch, unsigned short speed);

    int Ptz_GetScanSpeed(int nch, unsigned short *speed);

    int Ptz_SetFanEnable(int nch, int enable);

    int Ptz_GetFanEnable(int nch);

    int Ptz_SetHeatingEnbale(int nch, int enable);

    int Ptz_GetHeatingEnbale(int nch);

    int Ptz_SetPreset(int nch, char *preset_name);

    int Ptz_GetPreset(int nch, traget_preset *presetInfo, int *num);
    
    int Ptz_DelPreset(int nch, char *preset_name);

    int Ptz_SetPresetEx(int nch, traget_preset *presetInfo, int num);

    int Ptz_DataTransm(int nch, void *data, int len, void *out);

    int Coder_Init(int nch);

    int Coder_UnInit(int nch);

    int Coder_GetVideoCoderEncParam(int nch, video_codec *Enc);

    int Coder_SetVideoCoderEncParam(int nch, video_codec Enc);

    int Coder_GetAudioCoderEncParam(int nch, audio_codec *Enc);

    int Coder_SetAudioCoderEncParam(int nch, audio_codec Enc);

    int Coder_SetLanguage(int nch, char *language);

    int Coder_GetLanguage(int nch, char *language);

    int Coder_SetTimeConfig(int nch, time_cfg cfg);

    int Coder_GetTimeConfig(int nch, time_cfg *cfg);

    int Coder_SetTime(int nch, unsigned int time);
    
    int Ext_SetWipersEnable(int nch, int enable);

    int Ext_GetWipersEnable(int nch, int *enable);

    int Ext_SetAutoLightEnable(int nch, int enable);

    int Ext_GetAutoLightEnable(int nch, int *enable);

    int Ext_SetCvbsEnable(int nch, int enable);

    int Ext_GetCvbsEnable(int nch, int *enable);

    int Ext_GetDevVersionInfo(int nch, DevInfo_st *info);

    int Ext_SetTemperatureMode(int nch, int mode);

    int Ext_GetTemperaTureMode(int nch, int *mode);

    int Vis_SetBrightness(int nch, int value);

    int Vis_GetBrightness(int nch, int *value);

    int Vis_SetContrast(int nch, int value);

    int Vis_GetContrast(int nch, int *value);

    int Vis_SetFocuMode(int nch, int mode);

    int Vis_GetFocuMode(int nch, int *mode);

    int Vis_SetSaturation(int nch, int value);

    int Vis_GetSaturation(int nch, int *value);

    int Vis_SetSharpness(int nch, int value);
    
    int Vis_GetSharpness(int nch, int *value);

    int Vis_SetDigitalZoom(int nch, float value);

    int Vis_GetDigitalZoom(int nch, float *value);

    int Vis_RegisterVisCtrlCb(int nch, VisControlFunctions cb);

    int Vis_SetAutoFocuData(int nch, int mode);
};







#endif