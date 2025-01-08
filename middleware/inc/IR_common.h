#ifndef IR_COMMON_H
#define IR_COMMON_H

#include <stdint.h>

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

typedef struct
{
    uint64_t frame_id;
    uint8_t *img;
    uint32_t size;
    int format;
    int bpp;
    uint32_t widht;
    uint32_t height;
}__attribute__((packed)) img_data;

typedef enum {
	PSEUDO_COLOR_WHITE_HOT,			// 白热
	PSEUDO_COLOR_BLACK_HOT,			// 黑热
	PSEUDO_COLOR_IRON_RED,			// 铁红
	PSEUDO_COLOR_DESERT_YELLOW,		// 沙漠黄
	PSEUDO_COLOR_GREEN_HOT,			// 绿热
	PSEUDO_COLOR_RED_HOT,			// 红热
	PSEUDO_COLOR_SKY,				// 天空
	PSEUDO_COLOR_EDGE,				// 边缘
	PSEUDO_COLOR_RAINBOW,			// 彩虹
	PSEUDO_COLOR_IRONBOW_REVERSE,	// 反铁红
	PSEUDO_COLOR_LAVA_FORWARD,		// 熔盐
	PSEUDO_COLOR_LAVA_REVERSE,		// 反熔盐
	PSEUDO_COLOR_RAINBOW_REVERSE,	// 反彩虹
	PSEUDO_COLOR_RAINBOWHC_FORWARD,	// 彩虹高对比度
	PSEUDO_COLOR_RAINBOWHC_REVERSE,	// 反彩虹高对比度
}IrPseudoColor;

typedef struct {
    int (*IR_SetPicPreMode)(int mode);             //切换伪彩 IrPseudoColor
    int (*IR_SetAfocesMode)(int mode);             //设置调焦模式  0: 自动调焦 1:电动调焦
    int (*IR_SetAfocesStatus)(int status);         //调焦使能   自动模式：0：单次 1：连续； 电动模式，参数无意义   
    int (*IR_SetPicDigitalZoom)(int value);        //红外数字变倍 *1 *2 *4 *8 
    int (*IR_SetPicbrightness)(int value);         //红外亮度     0~10
    int (*IR_SetPicContrast)(int value);           //红外对比度   0~10
    int (*IR_SetGasEnhanced)(int enable);          //红外气体增强  0：关闭 1：打开 
} IRControlFunctions;

typedef struct {         
    int (*Vis_SetAfocesMode)(int mode);          //设置调焦模式     0: 自动调焦 1:电动调焦
    int (*Vis_SetAfocesStatus)(int status);      //调焦使能         自动模式：0：单次 1：连续； 电动模式，参数无意义  
    int (*Vis_SetPicDigitalZoom)(int value);     //可见光数字变倍   *1 *2 *4 *8 
} VisControlFunctions;


typedef int (*DetectResult_CALLBACK)(GAS_DETECT_RESULT* pDataBuf, int uiDataLen);



#endif