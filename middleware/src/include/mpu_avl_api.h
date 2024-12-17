/*
 * @Author: error: error: git config user.name & please set dead value or install git && error: git config user.email & please set dead value or install git & please set dead value or install git
 * @Date: 2024-05-14 14:57:49
 * @LastEditors: error: error: git config user.name & please set dead value or install git && error: git config user.email & please set dead value or install git & please set dead value or install git
 * @LastEditTime: 2024-07-29 15:43:28
 * @FilePath: \panoramic_code\src\include\mpu\mpu_avl_api.h
 * @Description: 这是默认设置,请设置`customMade`, 打开koroFileHeader查看配置 进行设置: https://github.com/OBKoro1/koro1FileHeader/wiki/%E9%85%8D%E7%BD%AE
 */
#ifndef _MPU_AVL_API_H_
#define _MPU_AVL_API_H_

#include "CChannelManager.h"
int AVL_Init();

int AVL_UnInit();

int AVL_SetperiphearaEnable(int enable);

int AVL_SetMpuCallback(peripheralInfoCallback callback);

int AVL_InfraredImage_Init(int nch);

int AVL_InfraredImage_UnInit(int nch);

int AVL_InfraredImage_SetImageBrightness(int nch, int value);

int AVL_InfraredImage_GetImageBrightness(int nch, int *value);

int AVL_InfraredImage_SetImageContrast(int nch, int value);

int AVL_InfraredImage_GetImageContrast(int nch, int *value);

int AVL_InfraredImage_SetHotspotTracking(int nch, int value);

int AVL_InfraredImage_GetHotspotTracking(int nch, int *value);

int AVL_InfraredImage_ManualDefectRemoval(int nch, int opt, int value);

int AVL_InfraredImage_GetManualDefectRemoval(int nch, int *opt, int *value);

int AVL_InfraredImage_SetPictureInPictureSwitch(int nch, int value);

int AVL_InfraredImage_GetPictureInPictureSwitch(int nch, int *value);

int AVL_InfraredImage_SetInfraredImageSharpening(int nch, int ddelv);

int AVL_InfraredImage_GetInfraredImageSharpening(int nch, int *ddelv);

int AVL_InfraredImage_SetInfraredImagePolarity(int nch, int value);

int AVL_InfraredImage_GetInfraredImagePolarity(int nch, int *value);

int AVL_InfraredImage_InfraredImageSaveCmd(int nch);

int AVL_InfraredImage_SetInfraredImagePAL(int nch, int status);

int AVL_InfraredImage_GetInfraredImagePAL(int nch, int *status);

int AVL_InfraredImage_SetInfraredImageElectronicZoom(int nch, float value);

int AVL_InfraredImage_GetInfraredImageElectronicZoom(int nch, float *value);

int AVL_InfraredImage_SetInfraredImageFocusMode(int nch, int mode);

int AVL_InfraredImage_GetInfraredImageFocusMode(int nch, int *mode);

int AVL_InfraredImage_SetGasEnhanced(int nch, int enable);

int AVL_InfraredImage_GetGasEnhanced(int nch, int *enable);

int AVL_InfraredImage_SetElectricFocu(int nch, int action);

int AVL_InfraredImage_SetAutoFocuData(int nch, int type);


int AVL_InfraredImage_SetImagesaturation(int nch, int value);

int AVL_InfraredImage_GetImagesaturation(int nch, int *value);

int AVL_Alg_SetDetectGasEnable(int nch, int enable);

int AVL_Alg_GetDetectGasEnable(int nch, int *enable);


int AVL_Groy_Init(int nch);

int AVL_Groy_UnInit(int nch);

int AVL_Groy_GetGyroAngle(int nch, double *picth_angle, double *roll_angle, double *temp);

int AVL_Groy_SetCalibrationSamples(int nch, int samples);

int AVL_Groy_GetCalibrationSamples(int nch);

int AVL_Groy_SetCalibration(int nch, int flag);

int AVL_Groy_GetCalibrationStatus(int nch);

int AVL_Groy_SetGyroAngle(int nch,double picth_angle, double roll_angle, double temp);

int AVL_Groy_GetTemp(int nch, double *value);


int AVL_Area_Init(int nch);

int AVL_Area_UnInit(int nch);

int AVL_Area_SetAreaPoint(int nch, area_information area);

int AVL_Area_SetAllAreaPoint(int nch, area_information *area, int num);

int AVL_Area_GetAreaPoint(int nch, area_information *area, int *num);

int AVL_Area_DeleteAreaPoint(int nch, const char *area_name);

int AVL_Alg_SetDetectEnable(int nch, int enable);

int AVL_Alg_SetTrackEnable(int nch, int enable);

int AVL_Alg_SetBehaviorEnable(int nch, int enable);

int AVL_Alg_GetAlgEnable(int nch, int *enable);

int AVL_Alg_SetDetectTrackId(int nch, std::vector<long> track_id);

int AVL_Alg_GetDetectTrackId(int nch, std::vector<long> track_id);

int AVL_Compass_Init(int nch);

int AVL_Compass_UnInit(int nch);

int AVL_Compass_GetYawAngle(int nch, double *value);

int AVL_Compass_SetCalibrationSamples(int nch, int value);

int AVL_Compass_GetCalibrationSamples(int nch, int *value);

int AVL_Compass_SetCalibrationFlag(int nch, int flag);

int AVL_Compass_GetCalibrationFlag(int nch, int *flag);


int AVL_Alarm_Init(int nch);

int AVL_Alarm_UnInit(int nch);

int AVL_Alarm_EnableAction(int nch, int Action, int bEnable);

int AVL_Alarm_SetAlarmActionCallback(int nch, ALARMCALLBACK callback);




int AVL_Ptz_Init(int nch);

int AVL_Ptz_UnInit(int nch);

int AVL_Ptz_SetPtzAngle(int nch, double yaw, double pitch);

int AVL_Ptz_GetPtzAngle(int nch, double *yaw, double *pitch);

int AVL_Ptz_SetScanMode(int nch, int mode);

int AVL_Ptz_GetScanMode(int nch, int *mode);

int AVL_Ptz_SetStep(int nch, double step);

int AVL_Ptz_GetStep(int nch, double *step);

int AVL_Ptz_SetPtzEnable(int nch, int enable);

int AVL_Ptz_GetPtzEnable(int nch, int *enable);

int AVL_Ptz_ZeroInit(int nch);

int AVL_Ptz_SetTargetLocation(int nch, target_data data);

int AVL_Ptz_GetTargetLocation(int nch, target_data *data);

int AVL_Ptz_SetFanScanAngle(int nch, double startAngle, double endAngle);

int AVL_Ptz_GetFanScanAngle(int nch, double *startAngle, double *endAngle);

int AVL_Ptz_SetScanSpeed(int nch, unsigned short speed);

int AVL_Ptz_GetScanSpeed(int nch, unsigned short *speed);

int AVL_Ptz_SetFanEnable(int nch, int enable);

int AVL_Ptz_GetFanEnable(int nch);

int AVL_Ptz_SetHeatingEnbale(int nch, int enable);

int AVL_Ptz_GetHeatingEnbale(int nch);

int AVL_Ptz_SetPreset(int nch, char *preset_name);

int AVL_Ptz_GetPreset(int nch, traget_preset *presetInfo, int *num);

int AVL_Ptz_DelPreset(int nch, char *preset_name);

int AVL_Ptz_SetPresetEx(int nch, traget_preset *presetInfo, int num);

int AVL_Ptz_DataTransm(int nch, void *data, int len, void *out);

int AVL_Coder_Init(int nch);

int AVL_Coder_UnInit(int nch);

int AVL_Coder_GetVideoCoderEncParam(int nch, video_codec *Enc);

int AVL_Coder_SetVideoCoderEncParam(int nch, video_codec Enc);

int AVL_Coder_GetAudioCoderEncParam(int nch, audio_codec *Enc);

int AVL_Coder_SetAudioCoderEncParam(int nch, audio_codec Enc);

int AVL_Coder_SetLanguage(int nch, char *language);

int AVL_Coder_GetLanguage(int nch, char *language);

int AVL_Coder_SetTimeConfig(int nch, time_cfg cfg);

int AVL_Coder_GetTimeConfig(int nch, time_cfg *cfg);

int AVL_Coder_SetTime(int nch, unsigned int time);

int AVL_Ext_SetWipersEnable(int nch, int enable);

int AVL_Ext_GetWipersEnable(int nch, int *enable);

int AVL_Ext_SetAutoLightEnable(int nch, int enable);

int AVL_Ext_GetAutoLightEnable(int nch, int *enable);

int AVL_Ext_SetCvbsEnable(int nch, int enable);

int AVL_Ext_GetCvbsEnable(int nch, int *enable);

int AVL_Ext_GetDevVersionInfo(int nch, DevInfo_st *info);


int AVL_Vis_SetBrightness(int nch, int value);

int AVL_Vis_GetBrightness(int nch, int *value);

int AVL_Vis_SetContrast(int nch, int value);

int AVL_Vis_GetContrast(int nch, int *value);

int AVL_Vis_SetFocuMode(int nch, int mode);

int AVL_Vis_GetFocuMode(int nch, int *mode);

int AVL_Vis_SetSaturation(int nch, int value);

int AVL_Vis_GetSaturation(int nch, int *value);

int AVL_Vis_SetSharpness(int nch, int value);

int AVL_Vis_GetSharpness(int nch, int *value);

#endif