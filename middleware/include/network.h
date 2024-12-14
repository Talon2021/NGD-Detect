#ifndef __NETWORK_H__
#define __NETWORK_H__

#include "network_common.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef int (*NetworkOperationGetAbilityCb)(NetworkAbilitySupportFunction* );

typedef enum {
    NETWORK_SYSTEM_REBOOT = 1000,   // 重启
    NETWORK_SYSTEM_RESET,           // 恢复出厂
    NETWORK_SYSTEM_FORMAT,          // 格式化
    NETWORK_SYSTEM_DEVICE_INFO,     // 设备信息
    NETWORK_SYSTEM_SET_TIME,        // 设置时间
    NETWORK_SYSTEM_GET_RTSP_URL,    // 获取rtsp_url
}NetworkSystemType;
typedef struct {
    int type;   // NetworkSystemType
    int cam;	// 摄像头

    union {
        int time;								// 设置的UTC时间
    } in;

    union {
        NetworkSystemDeviceInfo device_info;	// 获取的设备信息
        NetworkSystemRtspUrl rtsp_url;			// 获取的rtsp地址
    } out;
}NetworkSystem;
typedef int (*NetworkOperationSystemCb)(NetworkSystem* );

typedef enum {
    NETWORK_CONFIG_SYSTEM_TIME_INFO = 2000,			// 时间信息, NetworkConfigSystemTimeInfo

    NETWORK_CONFIG_CAMERA_IR_IMAGE_INFO,			// 红外图像信息, NetworkConfigCameraIrImageInfos
    NETWORK_CONFIG_CAMERA_IR_IMAGE_ENHANCE,			// 红外图像增强, NetworkConfigCameraIrImageEnhance
    NETWORK_CONFIG_CAMERA_IR_IMAGE_ZOOM,			// 红外图像变倍, NetworkConfigCameraIrImageZooms
    NETWORK_CONFIG_CAMERA_IR_FOCUSING,				// 红外调焦, NetworkConfigCameraIrFocusings
    NETWORK_CONFIG_CAMERA_IR_CALIBRATIONS,			// 红外校正, NetworkConfigCameraIrCalibrations

    NETWORK_CONFIG_CAMERA_VIS_IMAGE_INFO,			// 可见光图像信息, NetworkConfigCameraVisImageInfos
    NETWORK_CONFIG_CAMERA_VIS_IMAGE_ZOOM,			// 可见光图像变倍, NetworkConfigCameraVisImageZooms
    NETWORK_CONFIG_CAMERA_VIS_FOCUSING,				// 可见光图像信息, NetworkConfigCameraVisFocusings

	NETWORK_CONFIG_PTZ_INFO,						// 全景云台信息, NetworkConfigPtzInfo
	NETWORK_CONFIG_PTZ_PRESETS,						// 预置点, NetworkConfigPtzPresets

	NETWORK_CONFIG_ALGORITHM_DETECTION,				// 图像算法，NetworkConfigAlgorithmDetection
	NETWORK_CONFIG_ALGORITHM_AREAS,					// 算法区域，NetworkConfigAlgorithmAreas
	NETWORK_CONFIG_ALGORITHM_IMAGE,					// 图像算法，NetworkConfigAlgorithmImage

	NETWORK_CONFIG_NETWORK_TCP_IP,					// TCP/IP配置，NetworkConfigNetworkTcpIp

	NETWORK_CONFIG_DISPLAY_SCREEN_INFO,				// 设备显示屏信息，NetworkConfigDisplayScreenInfo
	NETWORK_CONFIG_DISPLAY_SCREEN_CALIBRATION_GUN,	// 校抢，NetworkConfigDisplayScreenCalibrationGun
	NETWORK_CONFIG_DISPLAY_SCREEN_RED_DOT_INFO,		// 红点，NetworkConfigDisplayScreenRedDotInfo

	NETWORK_CONFIG_OTHER_INFO,						// 其他信息，NetworkConfigOtherInfo
	NETWORK_CONFIG_OTHER_PREVIEW,					// 预览，NetworkConfigOtherPreview
	NETWORK_CONFIG_OTHER_POSITION,					// 定位，NetworkConfigOtherPosition
}NetworkConfigType;
typedef int (*NetworkOperationGetConfigCb)(int , void* , int );
typedef int (*NetworkOperationSetConfigCb)(int , void* , int );

typedef enum {
    NETWORK_CONTORL_SNAP = 3000,				// 抓拍 NetworkContorlSnap
    NETWORK_CONTORL_RECORD,						// 录像 NetworkContorlRecord
    NETWORK_CONTORL_BAD_PIX,					// 坏点 NetworkContorlBadPix
    NETWORK_CONTORL_SHUTTER_CALIBRATION,		// 快门校正 NetworkContorlShutterCalibration
    NETWORK_CONTORL_LASER_RANGING,				// 测距 NetworkContorlLaserRanging
    NETWORK_CONTORL_PTZ,						// 云台 NetworkContorlPtzCtrl
    NETWORK_CONTORL_TRACKING,					// 跟踪 NetworkContorlTrackingObject
    NETWORK_CONTORL_IR_FOCUSING,	            // 红外调焦 NetworkContorlIrFocusing
}NetworkContorlType;
typedef int (*NetworkOperationControlCb)(int, int, void*, int);     // type, cam, st, st_size

typedef int (*NetworkOperationUpgradeCb)(const char* );

typedef enum {
    NETWORK_TRANSPARENT_TRANSMISSION_PTZ = 4000,     // 云台
}NetworkTransparentTransmissionType;
typedef struct {
    int type;       // enum NetworkTransparentTransmissionType
    char* in;
    char* out;      // 默认1024空间，空间不足可用realloc重载空间
}NetworkTransparentTransmission;
typedef int (*NetworkOperationTransparentTransmissionCb)(NetworkTransparentTransmission*);

typedef enum {
    NETWORK_OPERATION_GET_ABILITY,                  // NetworkOperationGetAbilityCb   
    NETWORK_OPERATION_SYSTEM_REQUEST,               // NetworkOperationSystemCb
    NETWORK_OPERATION_GET_CONFIG,                   // NetworkOperationGetConfigCb
    NETWORK_OPERATION_SET_CONFIG,                   // NetworkOperationSetConfigCb
    NETWORK_OPERATION_CONTORL_REQUEST,              // NetworkOperationControlCb
    NETWORK_OPERATION_UPGRADE,                      // NetworkOperationUpgradeCb
    NETWORK_OPERATION_TRANSPARENT_TRANSMISSION,     // NetworkOperationTransparentTransmissionCb
}NetworkOperationType;
/**
 * @brief 回调注册接口
 */
void NetworkOperationRegister(NetworkOperationType type, void* cb);

/**
 * @brief 网络初始化
 * @return 成功返回 0
 *         失败返回 其他值
*/
int NetworkInit(char* addr);

/**
 * @brief 网络反初始化
*/
void NetworkUnInit();

/**
 * @brief 信息上报函数
 */
int NetworkUploadInfo(NetworkUpload* upload_info);

#ifdef __cplusplus
};
#endif

#endif