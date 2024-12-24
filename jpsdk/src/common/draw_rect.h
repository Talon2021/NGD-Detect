#ifndef __DRAW_RECT_H__
#define __DRAW_RECT_H__

#include <stdint.h>

#ifdef __cplusplus
extern "C"
{
#endif

// gwp add below 20241119
#define READ_MAX (1024)
#define DETECT_DISTANCE_VERIFY_OSD   //测距模块调试不同距离对应不同画框颜色，需要在框上打OSD。
 
typedef unsigned char  uInt8;
typedef unsigned short uInt16;
typedef unsigned int uInt32;
typedef char Int8;
typedef short Int16;
typedef int Int32;
 
typedef enum
{
	TYPE_YUV422I_UYVY,
	TYPE_YUV422I_YUYV,
	TYPE_YUV420SP_NV12,
	TYPE_YUV420SP_NV21,
    TYPE_YUV422SP_NV16,
    TYPE_YUV422SP_NV61,
	TYPE_YUV422P,
	TYPE_YUV444I,
	TYPE_YUV444P,
	TYPE_YUV420I,
}enYuvType;
 
typedef enum
{
	YUV_GREEN,
	YUV_RED,
	YUV_BLUE,
	YUV_PURPLE,
	YUV_DARK_GREEN,
	YUV_YELLOW,
	YUV_LIGHT_BLUE,
	YUV_LIGHT_PURPLE,
	YUV_DARK_BLACK,
	YUV_GRAY,
	YUV_WHITE,
	YUV_COLOR_MAX,
}enYuvColorIdx;

typedef enum
{
	POS_H_TOP,
	POS_H_BOTTOM,
    POS_V_LEFT,
    POS_V_RIGHT,
}enRectLinePos;
 
typedef struct
{
	uInt8 Y;
	uInt8 U;
	uInt8 V;
}stYuvColor;
 
typedef struct
{
	uInt16 x;
	uInt16 y;
}stPoint;
 
typedef struct
{
	stPoint startPoint;
	stPoint endPoint;
	uInt16 lineWidth;
	enYuvColorIdx clrIdx;
}stDrawLineInfo;
 
typedef struct
{
	enYuvType yuvType;
	uInt8 *pYuvBuff;
	uInt16 width;
	uInt16 height;
}stYuvBuffInfo;
 
#pragma pack (1)
typedef struct
{
	uInt16 x : 14;
	uInt16 y : 14;
	uInt8 color_idx : 4; // 2^4 可以指定16种颜色
}stYuvPixMap;
#pragma pack ()
 
typedef struct
{
	uInt32 pixNums;
	stYuvPixMap *pPixMapBuff;
}stPixMapInfo;

typedef struct
{
	uInt32 startX;
	uInt32 startY;
	uInt32 endX;
	uInt32 endY;
    uInt16 lineWidth;
	enYuvColorIdx clrIdx;
	#ifdef DETECT_DISTANCE_VERIFY_OSD
	Int32  distance;
	Int32  type; //AI type human=0 car=1 animal=2
	#endif
}stRectInfo;
 
static stYuvColor s_color_table[YUV_COLOR_MAX] = {
	{0x00, 0x00, 0x00}, // green
	{0x00, 0x00, 0xff}, // red
	{0x00, 0xff, 0x00},	// blue
	{0x00, 0xff, 0xff},	// purple
	{0xff, 0x00, 0x00}, // dark green
	{0xff, 0x00, 0xff}, // yellow
	{0xff, 0xff, 0x00}, // light blue
	{0xff, 0xff, 0xff}, // light purple
	{0x00, 0x80, 0x80}, // dark black
	{0x80, 0x80, 0x80}, // gray
	{0xff, 0x80, 0x80}, // white
};

int draw_rect_on_NV16_by_AI_result(stYuvBuffInfo *yuvBuffInfo , stRectInfo *rects, int rect_num, unsigned short lineWidth);
// gwp add above 20241119

typedef enum {
    COLOR_Y,
    COLOR_R,
    COLOR_G,
    COLOR_B,
    COLOR_BK,
    COLOR_W,
    COLOR_OR,  //��ɫ
    COLOR_PLATE,
    COLOR_PINK,
} color_type;

typedef struct draw_yuv_rect_s{
    int x;
    int y;
    int width;
    int height;
} draw_yuv_rect_t;

typedef struct draw_yuv_point_s{
    int x;
    int y;
} draw_yuv_point_t;

typedef struct draw_yuv_color_s{
    int Y;
    int U;
    int V;
} draw_yuv_color_t;

typedef struct  draw_line_s{
    draw_yuv_point_t start_point;
    draw_yuv_point_t end_point;
    int thick;
    int color;
} draw_line_t;

typedef struct draw_rect_s{
    int left;
    int top;
    int right;
    int bottom;
} draw_rect_t;

typedef struct draw_img_data_s{
    int width;
    int height;
    int size;
    uint8_t* data;
} draw_img_data_t;

draw_yuv_color_t set_yuv_color(color_type color_type);

int yuv420_draw_line(draw_img_data_t* img_data, draw_yuv_point_t startpoint, draw_yuv_point_t endpoint, int thick, draw_yuv_color_t color, int line_num);

int yuv420_draw_rectangle(draw_img_data_t* img_data, draw_yuv_rect_t rect, int thick, draw_yuv_color_t color);

int yuv420_draw_rectangles_dotted(draw_img_data_t* img_data, draw_yuv_rect_t* rects, int rect_num, int thick, draw_yuv_color_t color,int thick_num);

int yuv420_draw_rectangles_solid(draw_img_data_t* img_data, draw_yuv_rect_t* rects, int rect_num, int thick, draw_yuv_color_t color,int thick_num);

int yuv420_draw_rectangle_8_segment(draw_img_data_t* img_data, draw_rect_t *rect, int thick, draw_yuv_color_t color);

int yuv420_draw_rectangles_8_segment(draw_img_data_t* img_data, draw_rect_t* rects, int rect_num, int thick, draw_yuv_color_t color);

int yuv420_draw_polygon(draw_img_data_t* img_data, draw_yuv_point_t *points, int point_num, int thick, draw_yuv_color_t color, int thick_num);

int draw_bitmap_on_nv12(uint8_t *nv12_data, int yuv_width, int yuv_height, 
                        const uint8_t *bitmap_data, int bmp_width, int bmp_height, 
                        int start_x, int start_y);
#ifdef __cplusplus
}
#endif

#endif

