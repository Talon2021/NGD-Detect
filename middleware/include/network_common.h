#ifndef __NETWORK_COMMON_H__
#define __NETWORK_COMMON_H__

#include <stdbool.h>
#include <stdint.h>

#define NETWORK_CAMERA_MAX_NUM (4)	// 最大摄像头数量
#define NETWORK_PRESET_NAME_MAX_SIZE (64)

typedef enum {
	NETWORK_PSEUDO_COLOR_WHITE_HOT,			// 白热
	NETWORK_PSEUDO_COLOR_BLACK_HOT,			// 黑热
	NETWORK_PSEUDO_COLOR_IRON_RED,			// 铁红
	NETWORK_PSEUDO_COLOR_DESERT_YELLOW,		// 沙漠黄
	NETWORK_PSEUDO_COLOR_GREEN_HOT,			// 绿热
	NETWORK_PSEUDO_COLOR_RED_HOT,			// 红热
	NETWORK_PSEUDO_COLOR_SKY,				// 天空
	NETWORK_PSEUDO_COLOR_EDGE,				// 边缘
	NETWORK_PSEUDO_COLOR_RAINBOW,			// 彩虹
	NETWORK_PSEUDO_COLOR_IRONBOW_REVERSE,	// 反铁红
	NETWORK_PSEUDO_COLOR_LAVA_FORWARD,		// 熔盐
	NETWORK_PSEUDO_COLOR_LAVA_REVERSE,		// 反熔盐
	NETWORK_PSEUDO_COLOR_RAINBOW_REVERSE,	// 反彩虹
	NETWORK_PSEUDO_COLOR_RAINBOWHC_FORWARD,	// 彩虹高对比度
	NETWORK_PSEUDO_COLOR_RAINBOWHC_REVERSE,	// 反彩虹高对比度
}NetworkPseudoColor;

typedef enum {
	NETWORK_FOCUSING_MODE_AUTO,					// 自动聚焦
	NETWORK_FOCUSING_MODE_ELECTRIC,				// 电动聚焦
	NETWORK_FOCUSING_MODE_ONCE_FOCUSING,		// 一次聚焦
	NETWORK_FOCUSING_MODE_MANUAL_FOCUSING,		// 手动聚焦
	NETWORK_FOCUSING_MODE_SEMI_AUTO_FOCUSING,	// 半自动聚焦
}NetworkFocusingMode;

typedef enum {
	NETWORK_SENSITIVITY_NORMAL,		// 普通
	NETWORK_SENSITIVITY_LOW,		// 低
	NETWORK_SENSITIVITY_MID,		// 中
	NETWORK_SENSITIVITY_HIGH,		// 高
}NetworkSensitivityMode;

typedef enum {
    NETWORK_FUSION_VISIBLE_LIGHT,               // 可见光
    NETWORK_FUSION_INFRARED,                    // 红外
    NETWORK_FUSION_VISIBLE_LIGHT_AND_INFRARED,  // 融合
}NetworkFusionMode;

typedef enum {
    NETWORK_PICTURE_IN_PICTURE_OFF,         // 关闭
    NETWORK_PICTURE_IN_PICTURE_LEFT_UP,     // 左上
    NETWORK_PICTURE_IN_PICTURE_MIDDLE_UP,   // 中上
    NETWORK_PICTURE_IN_PICTURE_REIGHT_UP,   // 右上
}NetworkPictureInPicturePos;

typedef enum {
    NETWORK_LANGUAGE_CHINESE,	// 中
    NETWORK_LANGUAGE_ENGLISH,	// 英
    NETWORK_LANGUAGE_RUSSIAN,   // 俄
}NetworkLanguage;


typedef enum {
    NETWORK_COLGRATICULE_COLOR_WHITE,   // 白
    NETWORK_COLGRATICULE_COLOR_RED,     // 红
    NETWORK_COLGRATICULE_COLOR_YELLOW,  // 黄
    NETWORK_COLGRATICULE_COLOR_BLUE,    // 蓝
}NetworkColgraticuleColor;


/** 获取设备能力 **/
typedef struct {
	int support_state;	// 支持状态
	int min;			// 最小值
	int max;			// 最大值
}NetworkAbilityFunctionSoap;

/** 红外摄像头 **/
typedef struct {
	int brightness_support;					// 亮度
	int contrast_support;					// 对比度
	uint64_t pseudo_color_support_mask;		// 极性/伪彩掩码，【1 << NetworkPseudoColor】
	int sharpening_support;					// 锐度
	int saturation_support;					// 饱和度
}NetworkAbilityCameraIrImageInfoFunction;

typedef struct {
    int central_enhance_support;			// 中心增强
    int detail_enhance_support;				// 细节增强
}NetworkAbilityCameraIrImageEnhanceFunction;

typedef struct {
    int background_calibration_support;						// 背景校正
    int shutter_calibration_support;						// 快门校正
	NetworkAbilityFunctionSoap bad_pix_threshold_support;	// 坏点阈值
}NetworkAbilityCameraIrCalibrationFunction;

typedef struct {
    int electronic_zoom_support;			// 电子变倍，【0：不支持】【1：1/2/4】【2：1-4(±0.1)】【3：1-8(±1)】【4：1/2/4/8】
}NetworkAbilityCameraIrImageZoomFunction;

typedef struct {
    uint64_t focusing_mode_support_mask;			// 变焦模式, 【1 << NetworkFocusingMode】
}NetworkAbilityCameraIrImageFocusingFunction;

typedef struct {
    int support_state;
	NetworkAbilityCameraIrImageInfoFunction camera_ir_image_info;			// 图像信息
	NetworkAbilityCameraIrImageEnhanceFunction camera_ir_image_enhance;		// 图像增强
	NetworkAbilityCameraIrCalibrationFunction camera_ir_calibration;		// 校正
	NetworkAbilityCameraIrImageZoomFunction camera_ir_image_zoom;			// 变倍
	NetworkAbilityCameraIrImageFocusingFunction camera_ir_focusing;			// 调焦
}NetworkAbilityCameraIrFunction;

typedef struct {
    int num;
	NetworkAbilityCameraIrFunction camera_ir[NETWORK_CAMERA_MAX_NUM];
}NetworkAbilityCameraIrFunctions;

/** 可见光摄像头 **/
typedef struct {
	int brightness_support;					// 亮度
	int contrast_support;					// 对比度
	int sharpening_support;					// 锐度
	int saturation_support;					// 饱和度
}NetworkAbilityCameraVisImageInfoFunction;

typedef struct {
    int electronic_zoom_support;			// 电子变倍
}NetworkAbilityCameraVisImageZoomFunction;

typedef struct {
    uint64_t focusing_mode_support_mask;							// 变焦模式, 【1 << NetworkFocusingMode】
	uint64_t sensitivity_support_mask;								// 灵敏度, 【1 << NetworkSensitivityMode】
	int min_focusing_distance_support;								// 最小调焦距离
	NetworkAbilityFunctionSoap focus_distance_coordinates_support;	// 焦距坐标范围
}NetworkAbilityCameraVisImageFocusingFunction;

typedef struct {
    int support_state;
	NetworkAbilityCameraVisImageInfoFunction camera_vis_image_info;				// 图像信息
	NetworkAbilityCameraVisImageZoomFunction camera_vis_image_zoom;				// 变倍
	NetworkAbilityCameraVisImageFocusingFunction camera_vis_focusing;			// 调焦
}NetworkAbilityCameraVisFunction;

typedef struct {
    int num;
	NetworkAbilityCameraVisFunction camera_vis[NETWORK_CAMERA_MAX_NUM];
}NetworkAbilityCameraVisFunctions;

/** 云台 **/
typedef struct {
	int ptz_info_support;	// 全景云台信息
	int preset_support;		// 预置点
}NetworkAbilityPtzFunction;

/** 算法 **/
typedef struct {
	int area_support;	// 报警区域
}NetworkAbilityAlgorithmInfoFunction;

typedef struct {
	int object_detection_support;	// 目标检测
	int tracking_support;			// 追踪
	int action_analyze_support;		// 行为分析
	int fire_point_support;			// 火点检测
	int gas_detection_support;		// 气体检测
}NetworkAbilityAlgorithmDetectionFunction;

typedef struct {
	int gas_display_support;			// 气体显示增强
	int outdoor_mode_support;			// 户外模式
	int splicing_support;				// 图像拼接
	uint64_t fusion_mode_support_mask;	// 融合模式，【1 << NetworkFusionMode】
}NetworkAbilityAlgorithmImageFunction;

typedef struct {
	NetworkAbilityAlgorithmInfoFunction algorithm_info;				// 算法信息
	NetworkAbilityAlgorithmDetectionFunction algorithm_detection;	// 检测算法
	NetworkAbilityAlgorithmImageFunction algorithm_image;			// 图像算法
}NetworkAbilityAlgorithmFunction;

/** 网络 **/
typedef struct {
	int onvif_protocol_support;		// onvif协议
	int ipv6_protocol_support;		// ipv6协议
}NetworkAbilityNetworkFunction;

/** 设备显示屏 **/
typedef struct {
	uint64_t picture_in_picture_support_mask;	// 画中画，【1 << NetworkPictureInPicturePos】
	int electronic_compass_support;				// 电子罗盘
	uint64_t language_support_mask;				// 语言掩码, 【1 << NetworkLanguage】
	int red_dot_support;						// 红点
}NetworkAbilityDisplayScreenInfoFunction;

typedef struct {
	int support_state;
	int gun_type_support;					// 枪型
	uint64_t graticule_style_support_mask;	// 分划样式
	uint64_t color_support_mask;			// 分划颜色, 【1 << NetworkColgraticuleColor】
}NetworkAbilityDisplayScreenCalibrationGunFunction;

typedef struct {
	NetworkAbilityDisplayScreenInfoFunction display_info;					// 显示信息
	NetworkAbilityDisplayScreenCalibrationGunFunction calibration_gun;		// 校抢
}NetworkAbilityDisplayScreenFunction;

/** 杂项 **/
typedef struct {
	int heat_support;			// 加热
	int fan_support;			// 风扇
	int wipers_support;			// 雨刷
	int fill_light_support;		// 补光灯
}NetworkAbilityOtherInfoFunction;

typedef struct {
	int hdmi_support;			// hdmi
	int cvbs_support;			// cvbs
}NetworkAbilityOtherPreviewFunction;

typedef struct {
	NetworkAbilityOtherInfoFunction other_info;		// 其他信息
	NetworkAbilityOtherPreviewFunction preview;		// 预览
}NetworkAbilityOtherFunction;

typedef struct {
	NetworkAbilityCameraIrFunctions camera_ir_funcs;			// 红外摄像头
	NetworkAbilityCameraVisFunctions camera_vis_funcs;			// 可见光摄像头
	NetworkAbilityPtzFunction ptz_func;							// 云台
	NetworkAbilityAlgorithmFunction algorithm_func;				// 算法
	NetworkAbilityNetworkFunction network_func;					// 网络
	NetworkAbilityDisplayScreenFunction display_screen_func;	// 设备显示屏
	NetworkAbilityOtherFunction other_func;						// 杂项
}NetworkAbilitySupportFunction;

/** 系统请求 **/
#define NETWORK_SYSTEM_DEVICE_INFO_MAX (32)		// 设备信息长度
#define NETWORK_SYSTEM_URL_MAX (128)			// 码流地址长度

typedef struct {
	char soft_version[NETWORK_SYSTEM_DEVICE_INFO_MAX];	// 软件版本
	char hard_version[NETWORK_SYSTEM_DEVICE_INFO_MAX];	// 硬件版本
	char serial_number[NETWORK_SYSTEM_DEVICE_INFO_MAX];	// 序列号
}NetworkSystemDeviceInfo;

typedef struct {
    char main_stream[NETWORK_SYSTEM_URL_MAX];	// 主码流
    char sub_stream[NETWORK_SYSTEM_URL_MAX];	// 次码流
    char third_stream[NETWORK_SYSTEM_URL_MAX];	// 第三码流
}NetworkSystemRtspUrl;

/** 配置 **/
typedef struct {
	int x;
	int y;
}NetworkConfigPoint;

typedef struct {
	char time_zone[16];		// 时区
	int time_format;		// 时间格式
	int summer_time;		// 夏令时
}NetworkConfigSystemTimeInfo;


typedef struct {
	int brightness;		// 亮度
	int contrast;		// 对比度
	int sharpening;		// 锐化
	int saturation;		// 饱和度
	int pseudo_color;	// 极性/伪彩，enum NetworkPseudoColor
}NetworkConfigCameraIrImageInfo;

typedef struct {
	int deal_num;	// 处理数组中的某项，【-1：表示全数组】
	int num;		// 数组数
	NetworkConfigCameraIrImageInfo image_info[NETWORK_CAMERA_MAX_NUM];
}NetworkConfigCameraIrImageInfos;

typedef struct {
	int central;	// 中心增强
	int detail;		// 细节增强
}NetworkConfigCameraIrImageEnhance;

typedef struct {
	int deal_num;	// 处理数组中的某项，【-1：表示全数组】
	int num;		// 数组数
	NetworkConfigCameraIrImageEnhance image_enhance[NETWORK_CAMERA_MAX_NUM];
}NetworkConfigCameraIrImageEnhances;

typedef struct {
	float electronic;	// 电子变倍
}NetworkConfigCameraIrImageZoom;

typedef struct {
	int deal_num;	// 处理数组中的某项，【-1：表示全数组】
	int num;		// 数组数
	NetworkConfigCameraIrImageZoom image_zoom[NETWORK_CAMERA_MAX_NUM];
}NetworkConfigCameraIrImageZooms;

typedef struct {
	int focusing_mode;	// 调焦模式, enum NetworkFocusingMode
}NetworkConfigCameraIrFocusing;

typedef struct {
	int deal_num;	// 处理数组中的某项，【-1：表示全数组】
	int num;		// 数组数
	NetworkConfigCameraIrFocusing focusing[NETWORK_CAMERA_MAX_NUM];
}NetworkConfigCameraIrFocusings;

typedef struct {
	int shutter;			// 快门校正
	int background;			// 背景校正
	int bad_pix_threshold;	// 坏点阈值
}NetworkConfigCameraIrCalibration;

typedef struct {
	int deal_num;	// 处理数组中的某项，【-1：表示全数组】
	int num;		// 数组数
	NetworkConfigCameraIrCalibration calibration[NETWORK_CAMERA_MAX_NUM];
}NetworkConfigCameraIrCalibrations;


typedef struct {
	int brightness;		// 亮度
	int contrast;		// 对比度
	int sharpening;		// 锐化
	int saturation;		// 饱和度
}NetworkConfigCameraVisImageInfo;

typedef struct {
	int deal_num;	// 处理数组中的某项，【-1：表示全数组】
	int num;		// 数组数
	NetworkConfigCameraVisImageInfo image_info[NETWORK_CAMERA_MAX_NUM];
}NetworkConfigCameraVisImageInfos;

typedef struct {
	float electronic;	// 电子变倍
}NetworkConfigCameraVisImageZoom;

typedef struct {
	int deal_num;	// 处理数组中的某项，【-1：表示全数组】
	int num;		// 数组数
	NetworkConfigCameraVisImageZoom image_zoom[NETWORK_CAMERA_MAX_NUM];
}NetworkConfigCameraVisImageZooms;

typedef struct {
	int focusing_mode;					// 调焦模式, enum NetworkFocusingMode
	int sensitivity;					// 灵敏度，enum NetworkSensitivityMode
	int min_focusing_distance;			// 最近可聚焦距离
	int focus_distance_coordinates;		// 焦距坐标
}NetworkConfigCameraVisFocusing;

typedef struct {
	int deal_num;	// 处理数组中的某项，【-1：表示全数组】
	int num;		// 数组数
	NetworkConfigCameraVisFocusing focusing[NETWORK_CAMERA_MAX_NUM];
}NetworkConfigCameraVisFocusings;


typedef enum {
    NETWORK_CONFIG_CONSTAN_SCAN_DEFLECTION_ANGLE,  // 角度
    NETWORK_CONFIG_CONSTAN_SCAN_DEFLECTION_PIX,    // 像素点
}NetworkConfigConstanScanDeflectionMode;

typedef struct {
    int deflection_mode;    // 偏转模式，enum NetworkConfigConstanScanDeflectionMode
    double yaw;             // 角度
    NetworkConfigPoint pix;	// 像素点
}NetworkConfigConstantScan;

typedef struct {
    double start_angle;     // 起始角
    double end_angle;       // 终止角
}NetworkConfigFanScanning;

typedef enum {
    NETWORK_CONFIG_PTZ_SCAN_WEEKLY,    // 周扫描
    NETWORK_CONFIG_PTZ_SCAN_FAN,       // 扇扫描
    NETWORK_CONFIG_PTZ_SCAN_CONSTANT,  // 定扫描
}NetworkConfigPtzScanMode;

typedef struct {
    bool motor_enable;          // 电机使能  
    int scan_mode;              // 扫描模式，enum NetworkConfigPtzScanMode
    NetworkConfigConstantScan constant_scan; 
    NetworkConfigFanScanning fan_scanning;
    double pitch;               // 俯仰角
    double step;                // 步长
    int speed;                  // 速度
}NetworkConfigPtzInfo;

#define NETWORK_CONFIG_PRESET_MAX_NUM (256)

typedef struct {
	char name[NETWORK_PRESET_NAME_MAX_SIZE];	// 预置点名
	int num;		// 预置点号
}NetworkConfigPtzPreset;

typedef struct {
	int deal_num;	// 处理数组中的某项，【-1：表示全数组】
	int preset_num;	// 数组数
	NetworkConfigPtzPreset preset[NETWORK_CONFIG_PRESET_MAX_NUM];
}NetworkConfigPtzPresets;

typedef struct {
    int object_detection;	// 目标检测
    int tracking;           // 追踪
    int action_analyze;     // 行为分析
	int gas_detection;		// 气体检测
}NetworkConfigAlgorithmDetection;

#define NETWORK_CONFIG_AREA_NAME_SIAE (128)
#define NETWORK_CONFIG_POINT_NUM_MAX (8)
#define NETWORK_CONFIG_AREA_NUM_MAX (32)

typedef struct {
    char area_name[NETWORK_CONFIG_AREA_NAME_SIAE]; // 区域名
    bool enable;                        // 区域使能
    int point_num_max;                  // 区域点最大个数
    int point_num;                      // 区域点数
    NetworkConfigPoint point[NETWORK_CONFIG_POINT_NUM_MAX];     // 区域点坐标
}NetworkConfigAlgorithmArea;

typedef struct {
	int deal_num;	// 处理数组中的某项，【-1：表示全数组】
    int area_num;
    NetworkConfigAlgorithmArea area[NETWORK_CONFIG_AREA_NUM_MAX];
}NetworkConfigAlgorithmAreas;

typedef struct {
    int gas_display_enhance;	// 气体增强
    int outdoor_mode;           // 户外模式
    int fusion_mode;     		// 融合模式，enum NetworkFusionMode
}NetworkConfigAlgorithmImage;

typedef struct {
    bool dhcp_enable;
    bool ipv4_enable;
    char ipv4_addr[16];
    char ipv4_mask[16];
    char ipv4_gateway[16];
    bool ipv6_enable;
    char ipv6_addr[128];
    char ipv6_gateway[128];
    char mac[18];
    char dns[16];
    char alt_dns[16];
}NetworkConfigNetworkTcpIp;

typedef struct {
	int picture_in_picture;		// 画中画，
	int electronic_compass;		// 电子罗盘
	char language[32];			// 语言
}NetworkConfigDisplayScreenInfo;

typedef struct {
	char gun_type[32];					// 枪型
	int graticule_style;				// 分划样式 
	int color;							// 颜色，enum NetworkColgraticuleColor
	NetworkConfigPoint coordinate;		// 坐标
}NetworkConfigDisplayScreenCalibrationGun;

typedef struct {
    bool enable;					// 使能
    NetworkConfigPoint coordinate;	// 坐标
}NetworkConfigDisplayScreenRedDotInfo;

typedef enum {
	NETWORK_CONFIG_FILL_LIGHT_OFF,		// 关
	NETWORK_CONFIG_FILL_LIGHT_ON,		// 开
	NETWORK_CONFIG_FILL_LIGHT_AUTO,	// 自动
}NetworkConfigFillLightCtrl;

typedef struct {
	int heat;			// 加热
	int fan;			// 风扇
	int wipers;			// 雨刷
	int fill_light;		// 补光灯, enum NetworkConfigFillLightCtrl
}NetworkConfigOtherInfo;

typedef struct {
	int hdmi;			// hdmi
	int cvbs;			// cvbs
}NetworkConfigOtherPreview;

typedef struct {
	int position_mode;			// 定位模式
	int external_position;		// 外部定位
	int target_guidance;		// 目标引导
	int first_and_last_target;	// 首末目标
	int multi_point_position;	// 多点定位
}NetworkConfigOtherPosition;

/** 控制请求 **/
typedef struct {
	char reserve[128];
}NetworkContorlSnap;

typedef enum {
    NETWORK_CONTORL_RECORD_STOP,       // 单次
    NETWORK_CONTORL_RECORD_START,    // 持续
}NetworkContorlRecordState;

typedef struct {
	int state;	// enum NetworkContorlRecordState
	char reserve[128-4];
}NetworkContorlRecord;

typedef enum {
    NETWORK_CONTORL_LASER_RANGING_SINGLE,       // 单次
    NETWORK_CONTORL_LASER_RANGING_CONTINUED,    // 持续
}NetworkContorlLaserRangingMode;

typedef struct {
    int mode;   // enum NetworkLaserRangingMode   
    int state;  // 持续测距，0停止，1开始

	int distence;
	char reserve[128-12];
}NetworkContorlLaserRanging;

typedef enum {
    NETWORK_CONTORL_PTZ_CTRL_UP,
    NETWORK_CONTORL_PTZ_CTRL_DOWN,
    NETWORK_CONTORL_PTZ_CTRL_LEFT,
    NETWORK_CONTORL_PTZ_CTRL_RIGHT,
    NETWORK_CONTORL_PTZ_CTRL_HOME,
    NETWORK_CONTORL_PTZ_CTRL_PRESET, // 转到预置点，回调携带PRESET_NAME
}NetworkContorlPtzCtrlMode;

typedef struct {
    int mode;   // enum NetworkContorlPtzCtrlMode   
    char preset_name[NETWORK_PRESET_NAME_MAX_SIZE];  // 预置点名
	char reserve[128-68];
}NetworkContorlPtzCtrl;

#define NETWORK_CONTORL_TRACKING_OBJECT_MAX_NUM (16)
typedef struct {
    int tracking_num;
    int tracking_object[NETWORK_CONTORL_TRACKING_OBJECT_MAX_NUM];
}NetworkContorlTrackingObject;

typedef enum {
    NETWORK_CONTORL_BAD_PIX_OPERATION_GET,      // 获取
    NETWORK_CONTORL_BAD_PIX_OPERATION_DELETE,   // 清除
    NETWORK_CONTORL_BAD_PIX_OPERATION_RESET,    // 恢复
    NETWORK_CONTORL_BAD_PIX_OPERATION_SAVE,     // 保存
}NetworkContorlBadPixOperationType;

typedef struct {
	int oper;	// enum NetworkContorlBadPixOperationType
	int pix_num;
	char reserve[128-12];
}NetworkContorlBadPix;

typedef struct {
	char reserve[128];
}NetworkContorlShutterCalibration;

typedef enum {
    NETWORK_CONTORL_IR_FOCUSING_STOP,		// 停止
    NETWORK_CONTORL_IR_FOCUSING_LEFT,		// 左转
    NETWORK_CONTORL_IR_FOCUSING_RTGHT,		// 右转
}NetworkContorlIrFocusingMode;

typedef struct {
	int mode;	// enum NetworkContorlIrFocusing
	char reserve[128 - 4];
}NetworkContorlIrFocusing;

/** 设备上报 **/
typedef enum {
    NETWORK_ALARM_TYPE_OVER_BOUNDARY,            // 越界  
    NETWORK_ALARM_TYPE_AREA_INTRUDE,             // 区域入侵
    NETWORK_ALARM_TYPE_ABNORMAL_ACTION,          // 异常行为
    NETWORK_ALARM_TYPE_FIRE_POINT_DETECTION,     // 火点检测
    NETWORK_ALARM_TYPE_TEMPERATURE_ABNORMAL,     // 温度异常
    NETWORK_ALARM_TYPE_VOLTAGE_ABNORMAL,         // 电压异常
    NETWORK_ALARM_TYPE_AMPERE_ABNORMAL,          // 电流异常
}NetworkAlarmType;

typedef enum {
    NETWORK_ALARM_STATE_START,      // 开始
    NETWORK_ALARM_STATE_STOP,       // 停止
    NETWORK_ALARM_STATE_TRIGGER,    // 触发
}NetworkAlarmState;

typedef struct NetworkAlarmInfo {
    int type;       // 报警类型，enum NetworkAlarmType
    int state;      // 报警状态，enum NetworkAlarmState
    uint32_t time;  // 时间
}NetworkAlarmInfo;

typedef struct {
    double yaw;                 // 航偏角
    double pitch;               // 俯仰角
    double motor_temperature;   // 电机温度
    double input_voltage;       // 输入电压
    double working_ampere;      // 工作电流
    double longitude;           // 经度
    double latitude;            // 纬度
}NetworkPeripheralInfo;

#define NETWORK_UPLOAD_DOWNLOADING "downloading"            // 下载中
#define NETWORK_UPLOAD_DOWNLOAD_SUCCESS "download_success"  // 下载成功
#define NETWORK_UPLOAD_DOWNLOAD_FAIL "download_fail"        // 下载失败

#define NETWORK_UPLOAD_UPGRADING "upgrading"                // 升级中
#define NETWORK_UPLOAD_UPGRADE_SUCCESS "upgrade_success"    // 升级成功
#define NETWORK_UPLOAD_UPGRADE_FAIL "upgrade_fail"          // 升级失败

typedef enum {
    NETWORK_UPLOAD_ALARM = 4000,		// 报警
    NETWORK_UPLOAD_PERIPHERAL,			// 外设
    NETWORK_UPLOAD_DOWNLOAD,			// 下载
    NETWORK_UPLOAD_UPGRADE,				// 升级
    NETWORK_UPLOAD_LASER_RANGING,		// 测距
}NetworkUploadType;

typedef struct {
    int type; // enum NetworkUploadType
    union {
        NetworkAlarmInfo alarm_info;
        NetworkPeripheralInfo peripherail_info;
        char download_state[32];  // NETWORK_UPLOAD_DOWNLOADXXX
        char upgrade_state[32];  // NETWORK_UPLOAD_UPGRADXXX
        int distence;
    } data;
}NetworkUpload;

#endif