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




typedef int (*DetectResult_CALLBACK)(GAS_DETECT_RESULT* pDataBuf, int uiDataLen);



#endif