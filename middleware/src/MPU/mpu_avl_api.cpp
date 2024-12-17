/*
 * @Author: error: error: git config user.name & please set dead value or install git && error: git config user.email & please set dead value or install git & please set dead value or install git
 * @Date: 2024-05-14 14:28:45
 * @LastEditors: error: error: git config user.name & please set dead value or install git && error: git config user.email & please set dead value or install git & please set dead value or install git
 * @LastEditTime: 2024-07-30 14:30:14
 * @FilePath: \panoramic_code\src\MPU\mpu_avl_api.cpp
 * @Description: 这是默认设置,请设置`customMade`, 打开koroFileHeader查看配置 进行设置: https://github.com/OBKoro1/koro1FileHeader/wiki/%E9%85%8D%E7%BD%AE
 */
#include "mpu_avl_api.h"
#include "common.h"
#include "CChannelManager.h"
#include "sdk_log.h"
//调用通道管理器执行动作
#define MPU_AVL_CALL(x)															\
do																				\
{																				\
	int nRet;																		\
	CChannelManager *pMgr = CChannelManager::GetInstanceHandle();					\
	if (NULL != pMgr)																\
	{																			\
		nRet = pMgr->x;															\
																				\
		if (0 != nRet)														\
		{																		\
			ERROR("%s, return %d\n", #x, nRet);									\
		}																		\
	}																			\
	else																			\
	{																			\
		ERROR("Get CChannelManager instance fail!\n");								\
		nRet = -1;							                                    \
	}																			\
																				\
	return nRet;																	\
}																				\
while (0)


int AVL_Init()
{
    MPU_AVL_CALL(Init());
}

int AVL_UnInit()
{
    MPU_AVL_CALL(UnInit());
}

int AVL_SetperiphearaEnable(int enable)
{
    MPU_AVL_CALL(SetperiphearaEnable(enable));
}

int AVL_SetMpuCallback(peripheralInfoCallback callback)
{
    MPU_AVL_CALL(SetMpuCallback(callback));
}

int AVL_InfraredImage_Init(int nch)
{
   MPU_AVL_CALL(InfraredImage_Init(nch));
}

int AVL_InfraredImage_UnInit(int nch)
{
   MPU_AVL_CALL(InfraredImage_UnInit(nch));
}

int AVL_InfraredImage_SetImageBrightness(int nch, int value)
{
    MPU_AVL_CALL(InfraredImage_SetImageBrightness(nch, value));
}

int AVL_InfraredImage_GetImageBrightness(int nch, int *value)
{
    MPU_AVL_CALL(InfraredImage_GetImageBrightness(nch, value));
}

int AVL_InfraredImage_SetImageContrast(int nch, int value)
{
    MPU_AVL_CALL(InfraredImage_SetImageContrast(nch, value));
}

int AVL_InfraredImage_GetImageContrast(int nch, int *value)
{
    MPU_AVL_CALL(InfraredImage_GetImageContrast(nch, value));
}

int AVL_InfraredImage_SetHotspotTracking(int nch, int value)
{
    MPU_AVL_CALL(InfraredImage_SetHotspotTracking(nch, value));
}

int AVL_InfraredImage_GetHotspotTracking(int nch, int *value)
{
    MPU_AVL_CALL(InfraredImage_GetHotspotTracking(nch, value));
}

int AVL_InfraredImage_ManualDefectRemoval(int nch, int opt, int value)
{
    MPU_AVL_CALL(InfraredImage_ManualDefectRemoval(nch, opt, value));
}

int AVL_InfraredImage_GetManualDefectRemoval(int nch, int *opt, int *value)
{
    MPU_AVL_CALL(InfraredImage_GetManualDefectRemoval(nch, opt, value));
}

int AVL_InfraredImage_SetPictureInPictureSwitch(int nch, int value)
{
    MPU_AVL_CALL(InfraredImage_SetPictureInPictureSwitch(nch, value));
}

int AVL_InfraredImage_GetPictureInPictureSwitch(int nch, int *value)
{
    MPU_AVL_CALL(InfraredImage_GetPictureInPictureSwitch(nch, value));
}

int AVL_InfraredImage_SetInfraredImageSharpening(int nch, int ddelv)
{
    MPU_AVL_CALL(InfraredImage_SetInfraredImageSharpening(nch, ddelv));
}

int AVL_InfraredImage_GetInfraredImageSharpening(int nch, int *ddelv)
{
    MPU_AVL_CALL(InfraredImage_GetInfraredImageSharpening(nch, ddelv));
}

int AVL_InfraredImage_SetInfraredImagePolarity(int nch, int value)
{
    MPU_AVL_CALL(InfraredImage_SetInfraredImagePolarity(nch, value));
}

int AVL_InfraredImage_GetInfraredImagePolarity(int nch, int *value)
{
    MPU_AVL_CALL(InfraredImage_GetInfraredImagePolarity(nch, value));
}

int AVL_InfraredImage_InfraredImageSaveCmd(int nch)
{
    MPU_AVL_CALL(InfraredImage_InfraredImageSaveCmd(nch));
}

int AVL_InfraredImage_SetInfraredImagePAL(int nch, int status)
{
    MPU_AVL_CALL(InfraredImage_SetInfraredImagePAL(nch, status));
}

int AVL_InfraredImage_GetInfraredImagePAL(int nch, int *status)
{
    MPU_AVL_CALL(InfraredImage_GetInfraredImagePAL(nch, status));
}

int AVL_InfraredImage_SetInfraredImageElectronicZoom(int nch, float value)
{
    MPU_AVL_CALL(InfraredImage_SetInfraredImageElectronicZoom(nch, value));
}

int AVL_InfraredImage_GetInfraredImageElectronicZoom(int nch, float *value)
{
   MPU_AVL_CALL(InfraredImage_GetInfraredImageElectronicZoom(nch, value));
}

int AVL_InfraredImage_SetInfraredImageFocusMode(int nch, int mode)
{
    MPU_AVL_CALL(InfraredImage_SetInfraredImageFocusMode(nch, mode));
}

int AVL_InfraredImage_GetInfraredImageFocusMode(int nch, int *mode)
{
    MPU_AVL_CALL(InfraredImage_GetInfraredImageFocusMode(nch, mode));
}


int AVL_InfraredImage_SetGasEnhanced(int nch, int enable)
{
    MPU_AVL_CALL(InfraredImage_SetGasEnhanced(nch, enable));
}

int AVL_InfraredImage_GetGasEnhanced(int nch, int *enable)
{
    MPU_AVL_CALL(InfraredImage_GetGasEnhanced(nch, enable));
}

int AVL_InfraredImage_SetElectricFocu(int nch, int action)
{
    MPU_AVL_CALL(InfraredImage_SetElectricFocu(nch, action));
}

int AVL_InfraredImage_SetAutoFocuData(int nch, int type)
{
    MPU_AVL_CALL(InfraredImage_SetAutoFocuData(nch, type));
}

int AVL_InfraredImage_SetImagesaturation(int nch, int value)
{
    MPU_AVL_CALL(InfraredImage_SetImagesaturation(nch, value));
}

int AVL_InfraredImage_GetImagesaturation(int nch, int *value)
{
    MPU_AVL_CALL(InfraredImage_GetImagesaturation(nch, value));
}

int AVL_Alg_SetDetectGasEnable(int nch, int enable)
{
    MPU_AVL_CALL(Alg_SetDetectGasEnable(nch, enable));
}

int AVL_Alg_GetDetectGasEnable(int nch, int *enable)
{
    MPU_AVL_CALL(Alg_GetDetectGasEnable(nch, enable));
}

int AVL_Groy_Init(int nch)
{
    MPU_AVL_CALL(Groy_Init(nch));
}

int AVL_Groy_UnInit(int nch)
{
    MPU_AVL_CALL(Groy_UnInit(nch));
}

int AVL_Groy_GetGyroAngle(int nch, double *picth_angle, double *roll_angle, double *temp)
{
    MPU_AVL_CALL(Groy_GetGyroAngle(nch, picth_angle, roll_angle, temp));
}

int AVL_Groy_SetCalibrationSamples(int nch, int samples)
{
    MPU_AVL_CALL(Groy_SetCalibrationSamples(nch, samples));
}

int AVL_Groy_GetCalibrationSamples(int nch)
{
    MPU_AVL_CALL(Groy_GetCalibrationSamples(nch));
}

int AVL_Groy_SetCalibration(int nch, int flag)
{
    MPU_AVL_CALL(Groy_SetCalibration(nch, flag));
}

int AVL_Groy_GetCalibrationStatus(int nch)
{
    MPU_AVL_CALL(Groy_GetCalibrationStatus(nch));
}

int AVL_Groy_SetGyroAngle(int nch, double picth_angle, double roll_angle, double temp)
{
    MPU_AVL_CALL(Groy_SetGyroAngle(nch, picth_angle, roll_angle, temp));
}

int AVL_Groy_GetTemp(int nch, double *value)
{
    MPU_AVL_CALL(Groy_GetTemp(nch, value));
}

int AVL_Area_Init(int nch)
{
    MPU_AVL_CALL(Area_Init(nch));
}

int AVL_Area_UnInit(int nch)
{
    MPU_AVL_CALL(Area_UnInit(nch));
}

int AVL_Area_SetAreaPoint(int nch, area_information area)
{
    MPU_AVL_CALL(Area_SetAreaPoint(nch, area));
}

int AVL_Area_SetAllAreaPoint(int nch, area_information *area, int num)
{
    MPU_AVL_CALL(Area_SetAllAreaPoint(nch, area, num));
}

int AVL_Area_GetAreaPoint(int nch, area_information *area, int *num)
{
    MPU_AVL_CALL(Area_GetAreaPoint(nch, area, num));
}

int AVL_Area_DeleteAreaPoint(int nch, const char *area_name)
{
    MPU_AVL_CALL(Area_DeleteAreaPoint(nch, area_name));
}


int AVL_Alg_SetDetectEnable(int nch, int enable)
{
    MPU_AVL_CALL(Alg_SetDetectEnable(nch, enable));
}

int AVL_Alg_SetTrackEnable(int nch, int enable)
{
    MPU_AVL_CALL(Alg_SetTrackEnable(nch, enable));
}

int AVL_Alg_SetBehaviorEnable(int nch, int enable)
{
    MPU_AVL_CALL(Alg_SetBehaviorEnable(nch, enable));
}

int AVL_Alg_GetAlgEnable(int nch, int *enable)
{
    MPU_AVL_CALL(Alg_GetAlgEnable(nch, enable));
}

int AVL_Alg_SetDetectTrackId(int nch, std::vector<long> track_id)
{
    MPU_AVL_CALL(Alg_SetDetectTrackId(nch, track_id));
}

int AVL_Alg_GetDetectTrackId(int nch, std::vector<long> track_id)
{
    MPU_AVL_CALL(Alg_GetDetectTrackId(nch,track_id));
}

int AVL_Compass_Init(int nch)
{
    MPU_AVL_CALL(Compass_Init(nch));
}

int AVL_Compass_UnInit(int nch)
{
    MPU_AVL_CALL(Compass_UnInit(nch));
}

int AVL_Compass_GetYawAngle(int nch, double *value)
{
    MPU_AVL_CALL(Compass_GetYawAngle(nch, value));
}

int AVL_Compass_SetCalibrationSamples(int nch, int value)
{
    MPU_AVL_CALL(Compass_SetCalibrationSamples(nch, value));
}

int AVL_Compass_GetCalibrationSamples(int nch, int *value)
{
    MPU_AVL_CALL(Compass_GetCalibrationSamples(nch, value));
}

int AVL_Compass_SetCalibrationFlag(int nch, int flag)
{
    MPU_AVL_CALL(Compass_SetCalibrationFlag(nch, flag));
}

int AVL_Compass_GetCalibrationFlag(int nch, int *flag)
{
    MPU_AVL_CALL(Compass_GetCalibrationFlag(nch, flag));
}




int AVL_Alarm_Init(int nch)
{
    MPU_AVL_CALL(Alarm_Init(nch));
}

int AVL_Alarm_UnInit(int nch)
{
    MPU_AVL_CALL(Alarm_UnInit(nch));
}

int AVL_Alarm_EnableAction(int nch, int Action, int bEnable)
{
    MPU_AVL_CALL(Alarm_EnableAction(nch, Action, bEnable));
}

int AVL_Alarm_SetAlarmActionCallback(int nch, ALARMCALLBACK callback)
{
    MPU_AVL_CALL(Alarm_SetAlarmActionCallback(nch, callback));
}




int AVL_Ptz_Init(int nch)
{
    MPU_AVL_CALL(Ptz_Init(nch));
}

int AVL_Ptz_UnInit(int nch)
{
    MPU_AVL_CALL(Ptz_UnInit(nch));
}

int AVL_Ptz_SetPtzAngle(int nch, double yaw, double pitch)
{
    MPU_AVL_CALL(Ptz_SetPtzAngle(nch, yaw, pitch));
}

int AVL_Ptz_GetPtzAngle(int nch, double *yaw, double *pitch)
{
    MPU_AVL_CALL(Ptz_GetPtzAngle(nch, yaw, pitch));
}

int AVL_Ptz_SetScanMode(int nch, int mode)
{
    MPU_AVL_CALL(Ptz_SetScanMode(nch, mode));
}

int AVL_Ptz_GetScanMode(int nch, int *mode)
{
    MPU_AVL_CALL(Ptz_GetScanMode(nch, mode));
}

int AVL_Ptz_SetStep(int nch, double step)
{
    MPU_AVL_CALL(Ptz_SetStep(nch, step));
}

int AVL_Ptz_GetStep(int nch, double *step)
{
    MPU_AVL_CALL(Ptz_GetStep(nch,step));
}

int AVL_Ptz_SetPtzEnable(int nch, int enable)
{
    MPU_AVL_CALL(Ptz_SetPtzEnable(nch, enable));
}

int AVL_Ptz_GetPtzEnable(int nch, int *enable)
{
    MPU_AVL_CALL(Ptz_GetPtzEnable(nch, enable));
}

int AVL_Ptz_ZeroInit(int nch)
{
    MPU_AVL_CALL(Ptz_ZeroInit(nch));
}

int AVL_Ptz_SetTargetLocation(int nch, target_data data)
{
    MPU_AVL_CALL(Ptz_SetTargetLocation(nch, data));
}

int AVL_Ptz_GetTargetLocation(int nch, target_data *data)
{
    MPU_AVL_CALL(Ptz_GetTargetLocation(nch, data));
}

int AVL_Ptz_SetFanScanAngle(int nch, double startAngle, double endAngle)
{
    MPU_AVL_CALL(Ptz_SetFanScanAngle(nch, startAngle, endAngle));
}

int AVL_Ptz_GetFanScanAngle(int nch, double *startAngle, double *endAngle)
{
    MPU_AVL_CALL(Ptz_GetFanScanAngle(nch, startAngle, endAngle));
}

int AVL_Ptz_SetScanSpeed(int nch, unsigned short speed)
{
    MPU_AVL_CALL(Ptz_SetScanSpeed(nch, speed));
}

int AVL_Ptz_GetScanSpeed(int nch, unsigned short *speed)
{
    MPU_AVL_CALL(Ptz_GetScanSpeed(nch, speed));
}

int AVL_Ptz_SetFanEnable(int nch, int enable)
{
    MPU_AVL_CALL(Ptz_SetFanEnable(nch, enable));
}

int AVL_Ptz_GetFanEnable(int nch)
{
    MPU_AVL_CALL(Ptz_GetFanEnable(nch));
}

int AVL_Ptz_SetHeatingEnbale(int nch, int enable)
{
    MPU_AVL_CALL(Ptz_SetHeatingEnbale(nch, enable));
}

int AVL_Ptz_GetHeatingEnbale(int nch)
{
    MPU_AVL_CALL(Ptz_GetHeatingEnbale(nch));
}

int AVL_Ptz_SetPreset(int nch, char *preset_name)
{
    MPU_AVL_CALL(Ptz_SetPreset(nch, preset_name));
}

int AVL_Ptz_GetPreset(int nch, traget_preset *presetInfo, int *num)
{
    MPU_AVL_CALL(Ptz_GetPreset(nch, presetInfo, num));
}

int AVL_Ptz_DelPreset(int nch, char *preset_name)
{
    MPU_AVL_CALL(Ptz_DelPreset(nch, preset_name));
}

int AVL_Ptz_SetPresetEx(int nch, traget_preset *presetInfo, int num)
{
    MPU_AVL_CALL(Ptz_SetPresetEx(nch, presetInfo, num));
}

int AVL_Ptz_DataTransm(int nch, void *data, int len, void *out)
{
    MPU_AVL_CALL(Ptz_DataTransm(nch, data, len, out));
}

int AVL_Coder_Init(int nch)
{
    MPU_AVL_CALL(Coder_Init(nch));
}

int AVL_Coder_UnInit(int nch)
{
    MPU_AVL_CALL(Coder_UnInit(nch));
}

int AVL_Coder_GetVideoCoderEncParam(int nch, video_codec *Enc)
{
    MPU_AVL_CALL(Coder_GetVideoCoderEncParam(nch, Enc));
}

int AVL_Coder_SetVideoCoderEncParam(int nch, video_codec Enc)
{
    MPU_AVL_CALL(Coder_SetVideoCoderEncParam(nch, Enc));
}

int AVL_Coder_GetAudioCoderEncParam(int nch, audio_codec *Enc)
{
    MPU_AVL_CALL(Coder_GetAudioCoderEncParam(nch, Enc));
}

int AVL_Coder_SetAudioCoderEncParam(int nch, audio_codec Enc)
{
    MPU_AVL_CALL(Coder_SetAudioCoderEncParam(nch, Enc));
}

int AVL_Coder_SetLanguage(int nch, char *language)
{
    MPU_AVL_CALL(Coder_SetLanguage(nch, language));
}

int AVL_Coder_GetLanguage(int nch, char *language)
{
    MPU_AVL_CALL(Coder_GetLanguage(nch, language));
}

int AVL_Coder_SetTimeConfig(int nch, time_cfg cfg)
{
    MPU_AVL_CALL(Coder_SetTimeConfig(nch, cfg));
}

int AVL_Coder_GetTimeConfig(int nch, time_cfg *cfg)
{
    MPU_AVL_CALL(Coder_GetTimeConfig(nch, cfg));
}

int AVL_Coder_SetTime(int nch, unsigned int time)
{
    MPU_AVL_CALL(Coder_SetTime(nch, time));
}

int AVL_Ext_SetWipersEnable(int nch, int enable)
{
    MPU_AVL_CALL(Ext_SetWipersEnable(nch, enable));
}

int AVL_Ext_GetWipersEnable(int nch, int *enable)
{
    MPU_AVL_CALL(Ext_GetWipersEnable(nch, enable));
}

int AVL_Ext_SetAutoLightEnable(int nch, int enable)
{
    MPU_AVL_CALL(Ext_SetAutoLightEnable(nch, enable));
}

int AVL_Ext_GetAutoLightEnable(int nch, int *enable)
{
    MPU_AVL_CALL(Ext_GetAutoLightEnable(nch, enable));
}

int AVL_Ext_SetCvbsEnable(int nch, int enable)
{
    MPU_AVL_CALL(Ext_SetCvbsEnable(nch, enable));
}

int AVL_Ext_GetCvbsEnable(int nch, int *enable)
{
    MPU_AVL_CALL(Ext_GetCvbsEnable(nch, enable));
}

int AVL_Ext_GetDevVersionInfo(int nch, DevInfo_st *info)
{
    MPU_AVL_CALL(Ext_GetDevVersionInfo(nch, info));
}

int AVL_Vis_SetBrightness(int nch, int value)
{
    MPU_AVL_CALL(Vis_SetBrightness(nch, value));
}

int AVL_Vis_GetBrightness(int nch, int *value)
{
    MPU_AVL_CALL(Vis_GetBrightness(nch, value));
}

int AVL_Vis_SetContrast(int nch, int value)
{
    MPU_AVL_CALL(Vis_SetContrast(nch, value));
}

int AVL_Vis_GetContrast(int nch, int *value)
{
    MPU_AVL_CALL(Vis_GetContrast(nch, value));
}

int AVL_Vis_SetFocuMode(int nch, int mode)
{
    MPU_AVL_CALL(Vis_SetFocuMode(nch, mode));
}

int AVL_Vis_GetFocuMode(int nch, int *mode)
{
    MPU_AVL_CALL(Vis_GetFocuMode(nch, mode));
}

int AVL_Vis_SetSaturation(int nch, int value)
{
    MPU_AVL_CALL(Vis_SetSaturation(nch, value));
}

int AVL_Vis_GetSaturation(int nch, int *value)
{
    MPU_AVL_CALL(Vis_GetSaturation(nch, value));
}

int AVL_Vis_SetSharpness(int nch, int value)
{
    MPU_AVL_CALL(Vis_SetSharpness(nch, value));
}

int AVL_Vis_GetSharpness(int nch, int *value)
{
    MPU_AVL_CALL(Vis_GetSharpness(nch, value));
}
