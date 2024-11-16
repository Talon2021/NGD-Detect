#ifndef __NETWORK_H__
#define __NETWORK_H__

#include "network_common.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef int (*NetworkOperationGetAbilityCb)(NetworkSupportFunction* );

typedef enum {
    NETWORK_SYSTEM_REBOOT = 100,    // 重启
    NETWORK_SYSTEM_RESET,           // 恢复出厂
    NETWORK_SYSTEM_FORMAT,          // 格式化
    NETWORK_SYSTEM_DEVICE_INFO,     // 设备信息
    NETWORK_SYSTEM_SET_TIME,        // 设置时间
    NETWORK_SYSTEM_GET_RTSP_URL,    // 获取rtsp_url
}NetworkSystemType;
typedef struct {
    int type;
    union {
        int time;                   // 设置的UTC时间
    } in;

    union {
        NetworkDeviceInfo device_info;     // 获取的设备信息
        char rtsp_url[128];         // 获取的rtsp地址
    } out;
}NetworkSystem;
typedef int (*NetworkOperationSystemCb)(NetworkSystem* );

typedef enum {
    NETWORK_CONFIG_SYSTEM_INFO = 200,   // 系统信息, NetworkSystemInfo
    NETWORK_CONFIG_CAMERA_CHIP_INFO,    // 机芯信息, NetworkCameraChipInfo
    NETWORK_CONFIG_POSITION,            // 定位, NetworkPosition
    NETWORK_CONFIG_CALIBRATION_GUN,     // 校枪, NetworkCalibrationGun
    NETWORK_CONFIG_PTZ_INFO,            // 云台, NetworkPtzInfo
    NETWORK_CONFIG_AREAS,               // 区域, NetworkAreas
    NETWORK_CONFIG_OTHER_INFO,          // 其他信息, NetworkOtherConfig
    NETWORK_CONFIG_NETWORK,             // 网络信息, NetworkNetworkInfo
    NETWORK_CONFIG_ALGORITHM,           // 算法, NetworkAlgorithm
    NETWORK_CONFIG_PRESETS,             // 预置点, NetworkPresets
}NetworkConfigType;
typedef int (*NetworkOperationGetConfigCb)(NetworkConfigType , void* , int );
typedef int (*NetworkOperationSetConfigCb)(NetworkConfigType , void* , int );

typedef enum {
    NETWORK_CONTORL_SNAP = 300,             // 抓拍
    NETWORK_CONTORL_RECORD,                 // 录像
    NETWORK_CONTORL_BAD_PIX,                // 坏点
    NETWORK_CONTORL_SHUTTER_CALIBRATION,    // 快门校正
    NETWORK_CONTORL_LASER_RANGING,          // 测距
    NETWORK_CONTORL_PTZ,                    // 云台(预留)
    NETWORK_CONTORL_TRACKING,               // 跟踪
    NETWORK_CONTORL_INFRARED_ELECTRIC_FOCUS,// 红外电动调焦
}NetworkContorlType;

typedef struct {
    int type;   // enum NetworkContorlType
    union {
        int recode_state;           // 录像状态，0结束，1开始
        int bad_pix_operation;      // enum NetworkBadPixOperationType
        int infrared_focus_mode;    // 红外电动调焦, 0停止，1左转，2右转
        NetworkLaserRanging laser_ranging;
        NetworkPtzCtrl ptz_ctrl;
        NetworkTrackingObject tracking_object;
    } in;
    
    union {
        int bad_pix_num;    // 获取坏点数
        int distence;       // 单次测距
    } out;
}NetworkContorl;
typedef int (*NetworkOperationControlCb)(NetworkContorl* );

typedef int (*NetworkOperationUpgradeCb)(const char* );

typedef enum {
    NETWORK_TRANSPARENT_TRANSMISSION_PTZ = 400,     // 云台
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