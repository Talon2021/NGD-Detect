#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <libgen.h>
#include "draw_rect.h"

#define draw_rect_err(format, args...)  do{printf("\033[1;31m[DRAW_RECT][%s-%d]\033[m" format, basename(__FILE__), __LINE__, ##args);}while(0)

#define LINE_NUMBER 1
#ifndef ABS
#define ABS(x) (((x) < 0) ? -(x) : (x))
#endif

#ifndef YUV_SWAP
#define YUV_SWAP(a, b) {(a)=(a)+(b); (b)=(a)-(b); (a)=(a)-(b);}
#endif

static int yuv420_draw_point(draw_img_data_t *img_data, draw_yuv_point_t point, draw_yuv_color_t color)
{
    int yuSize = 0;
    int y_offset = 0;
    int u_offset = 0;
    int v_offset = 0;

    if (!img_data || !img_data->data)
    {
        draw_rect_err("null pointer exception.\n");
        return -1;
    }

    if (img_data->width < 0 || img_data->height < 0)
    {
        draw_rect_err("width or height invaild(width=%d,height=%d).\n", 
                       img_data->width, img_data->height);
        return -1;
    }

    if (point.x < 0 || point.x > img_data->width || point.y < 0 || point.y > img_data->height)
    {
        draw_rect_err("point index invaild(x=%d,y=%d,width=%d,height=%d).\n",
                      point.x, point.y, img_data->width, img_data->height);
        return -1;
    }
#if 1 //NV12
    if (point.x % 2)
    {
        point.x --;
    }

    if (point.y % 2)
    {
        point.y --;
    }

    yuSize = img_data->width * img_data->height;

    y_offset = point.y * img_data->width + point.x;
    u_offset = yuSize + ((point.y * img_data->width) >> 1) + point.x;  //gwp mark 20241127 NV12
    v_offset = yuSize + ((point.y * img_data->width) >> 1) + point.x + 1;
    // u_offset = yuSize + (point.y * img_data->width) + point.x;  //gwp mark 20241127 此处已经变成了NV16的格式 可以只改这里便可实现NV16画点
    // v_offset = yuSize + (point.y * img_data->width) + point.x + 1;

    y_offset = y_offset > (img_data->size - 1) ? (img_data->size - 1) : y_offset;
    u_offset = u_offset > (img_data->size - 1) ? (img_data->size - 1) : u_offset;
    v_offset = v_offset > (img_data->size - 1) ? (img_data->size - 1) : v_offset;

    img_data->data[y_offset] = color.Y;
    img_data->data[u_offset] = color.U;
    img_data->data[v_offset] = color.V;
#else //NV16
    yuSize = img_data->width * img_data->height;

    // Calculate Y offset (no change here)
    y_offset = point.y * img_data->width + point.x;

    if (point.x % 2)
    {
        point.x--;
    }
    // Calculate U and V offsets for NV16 format
    u_offset = yuSize + (point.y * img_data->width) + (point.x & ~1);
    v_offset = u_offset + 1;

    // Boundary checks
    y_offset = y_offset > (img_data->size - 1) ? (img_data->size - 1) : y_offset;
    u_offset = u_offset > (img_data->size - 2) ? (img_data->size - 2) : u_offset;  // Ensure u_offset and v_offset are within bounds
    v_offset = u_offset + 1;  // v_offset should be immediately after u_offset

    // Set pixel color
    img_data->data[y_offset] = color.Y;
    img_data->data[u_offset] = color.U;
    img_data->data[v_offset] = color.V;

    //printf("y_offset = %d, u_offset = %d, v_offset = %d\n", y_offset, u_offset, v_offset);
#endif
    return 0;
}

static int yuv420_draw_vline(draw_img_data_t *img_data, draw_yuv_point_t start_point, draw_yuv_point_t end_point, draw_yuv_color_t color)
{
    int ret = 0;
    int x0 = 0, y0 = 0, y1 = 0;
    draw_yuv_point_t point = {0};

    if (!img_data || !img_data->data)
    {
        draw_rect_err("null pointer exception.\n");
        return -1;
    }

    x0 = start_point.x;
    y0 = start_point.y;
    y1 = end_point.y;

    if (y0 > y1)
    {
        YUV_SWAP(y0, y1);
    }

    point.x = x0;
    point.y = y0;

    while (point.y < y1)
    {
        point.y ++;
        ret = yuv420_draw_point(img_data, point, color);
        if (ret)
        {
            return -1;
        }
    }

    return 0;
}

static int yuv420_draw_hline(draw_img_data_t *img_data, draw_yuv_point_t start_point,  draw_yuv_point_t end_point,  draw_yuv_color_t color)
{
    int ret = 0;
    int x0 = 0, y0 = 0, x1 = 0;
    draw_yuv_point_t point = {0};

    if (!img_data || !img_data->data)
    {
        draw_rect_err("null pointer exception.\n");
        return -1;
    }

    x0 = start_point.x;
    y0 = start_point.y;
    x1 = end_point.x;

    if (x0 > x1)
    {
        YUV_SWAP(x0, x1);
    }

    point.x = x0;
    point.y = y0;

    while (point.x < x1)
    {
        point.x ++;
        ret = yuv420_draw_point(img_data, point, color);
        if (ret)
        {
            draw_rect_err("Fail to called yuv420_draw_point.\n");
            return -1;
        }
    }

    return 0;
}

static int yuv420_draw_tline(draw_img_data_t *img_data, draw_yuv_point_t startpoint, draw_yuv_point_t endpoint, draw_yuv_color_t color)
{
    int x0 = 0, y0 = 0, x1 = 0, y1 = 0;
    int dx = 0, dy = 0, error = 0;
    draw_yuv_point_t point = {0};

    if (!img_data || !img_data->data)
    {
        draw_rect_err("null pointer exception.\n");
        return -1;
    }

    x0 = startpoint.x;
    y0 = startpoint.y;
    x1 = endpoint.x;
    y1 = endpoint.y;

    if (x0 > x1)
    {
        YUV_SWAP(x0, x1);
        YUV_SWAP(y0, y1);
    }

    dx = x1 - x0;
    dy = y1 - y0;
    point.x = x0;
    point.y = y0;

    if (0 == dx)
    {
        yuv420_draw_vline(img_data, startpoint, endpoint, color);
        return 0;
    }

    if (0 == dy)
    {
        yuv420_draw_hline(img_data, startpoint, endpoint, color);
        return 0;
    }

    yuv420_draw_point(img_data, point, color);

    if ((dx >= dy) && (dy > 0)) /* 0 < k <= 1 */
    {
        error = (dy << 1) - dx;
        while (point.x < x1)
        {
            if (error > 0)
            {
                error += ((dy - dx) << 1);
                point.x ++;
                point.y ++;
            }
            else
            {
                error += (dy << 1);
                point.x ++;
            }
            yuv420_draw_point(img_data, point, color);
        }
        return 0;
    }

    if ((dy > dx) && (dy > 0)) /* k>1 */
    {
        error = dy - (dx << 1);
        while (point.y < y1)
        {
            if (error < 0)
            {
                error += ((dy - dx) << 1);
                point.x ++;
                point.y ++;
            }
            else
            {
                error += ((-dx) << 1);
                point.y ++;
            }
            yuv420_draw_point(img_data, point, color);
        }
        return 0;
    }

    if ((dx >= ABS(dy)) && (dy < 0)) /* -1=<k<0 */
    {
        error = (dy << 1) + dx;
        while (point.x < x1)
        {
            if (error < 0)
            {
                error += ((dy + dx) << 1);
                point.x ++;
                point.y --;
            }
            else
            {
                error += (dy << 1);
                point.x++;
            }
            yuv420_draw_point(img_data, point, color);
        }
        return 0;
    }

    if ((ABS(dy) > dx) && (dy < 0)) /* k<-1 */
    {
        error = dy + (dx << 1);
        while (point.y > y1)
        {
            if (error > 0)
            {
                error += ((dy + dx) << 1);
                point.x ++;
                point.y --;
            }
            else
            {
                error += (dx << 1);
                point.y--;
            }
            yuv420_draw_point(img_data, point, color);
        }
        return 0;
    }

    return 0;
}


static int converRechToLine8Seg(draw_yuv_rect_t *rects, int region_count, draw_line_t *draw_line, 
                                int *line_count, int line_thick, int line_color_rgb)
{
    int i = 0;
    draw_yuv_point_t rect_point[4] = {0};
    int rect_width = 0;
    int rect_height = 0;
    int line_width = 0; 
    int line_height = 0;

    if (!rects || !draw_line || !line_count)
    {
        draw_rect_err("null pointer exception.\n");
        return -1;
    }
    
    *line_count = 0;

    for(i=0; i<region_count; i++)
    {
        rect_point[0].x = rects[i].x; 
        rect_point[0].y = rects[i].y;
        rect_point[1].x = rects[i].x + rects[i].width - line_thick; 
        rect_point[1].y = rects[i].y;
        rect_point[2].x = rects[i].x; 
        rect_point[2].y = rects[i].y + rects[i].height - line_thick;
        rect_point[3].x = rects[i].x + rects[i].width - line_thick; 
        rect_point[3].y = rects[i].y + rects[i].height - line_thick;

        rect_width = rects[i].width;
        rect_height = rects[i].height;
        line_width = (rect_width >> 2);
        line_height = (rect_height >> 2);
        line_width = (line_width + 1) & (~0x1);
        line_height = (line_height + 1) & (~0x1);

        draw_line[i * 8].start_point.x = rect_point[0].x;
        draw_line[i * 8].start_point.y = rect_point[0].y;
        draw_line[i * 8].end_point.x = rect_point[0].x + line_width;
        draw_line[i * 8].end_point.y = rect_point[0].y;
        draw_line[i * 8].thick = line_thick;
        draw_line[i * 8].color = line_color_rgb;
        draw_line[i * 8 + 1].start_point.x = rect_point[1].x - line_width;
        draw_line[i * 8 + 1].start_point.y = rect_point[1].y;
        draw_line[i * 8 + 1].end_point.x = rect_point[1].x;
        draw_line[i * 8 + 1].end_point.y = rect_point[1].y;
        draw_line[i * 8 + 1].thick = line_thick;
        draw_line[i * 8 + 1].color = line_color_rgb;
        draw_line[i * 8 + 2].start_point.x = rect_point[2].x;
        draw_line[i * 8 + 2].start_point.y = rect_point[2].y;
        draw_line[i * 8 + 2].end_point.x = rect_point[2].x + line_width;
        draw_line[i * 8 + 2].end_point.y = rect_point[2].y;
        draw_line[i * 8 + 2].thick = line_thick;
        draw_line[i * 8 + 2].color = line_color_rgb;
        draw_line[i * 8 + 3].start_point.x = rect_point[3].x - line_width;
        draw_line[i * 8 + 3].start_point.y = rect_point[3].y;
        draw_line[i * 8 + 3].end_point.x = rect_point[3].x;
        draw_line[i * 8 + 3].end_point.y = rect_point[3].y;
        draw_line[i * 8 + 3].thick = line_thick;
        draw_line[i * 8 + 3].color = line_color_rgb;

        draw_line[i * 8 + 4].start_point.x = rect_point[0].x;
        draw_line[i * 8 + 4].start_point.y = rect_point[0].y;
        draw_line[i * 8 + 4].end_point.x = rect_point[0].x;
        draw_line[i * 8 + 4].end_point.y = rect_point[0].y + line_height;
        draw_line[i * 8 + 4].thick = line_thick;
        draw_line[i * 8 + 4].color = line_color_rgb;
        draw_line[i * 8 + 5].start_point.x = rect_point[2].x;
        draw_line[i * 8 + 5].start_point.y = rect_point[2].y - line_height;
        draw_line[i * 8 + 5].end_point.x = rect_point[2].x;
        draw_line[i * 8 + 5].end_point.y = rect_point[2].y;
        draw_line[i * 8 + 5].thick = line_thick;
        draw_line[i * 8 + 5].color = line_color_rgb;
        draw_line[i * 8 + 6].start_point.x = rect_point[1].x;
        draw_line[i * 8 + 6].start_point.y = rect_point[1].y;
        draw_line[i * 8 + 6].end_point.x = rect_point[1].x;
        draw_line[i * 8 + 6].end_point.y = rect_point[1].y + line_height;
        draw_line[i * 8 + 6].thick = line_thick;
        draw_line[i * 8 + 6].color = line_color_rgb;
        draw_line[i * 8 + 7].start_point.x = rect_point[3].x;
        draw_line[i * 8 + 7].start_point.y = rect_point[3].y - line_height;
        draw_line[i * 8 + 7].end_point.x = rect_point[3].x;
        draw_line[i * 8 + 7].end_point.y = rect_point[3].y;
        draw_line[i * 8 + 7].thick = line_thick;
        draw_line[i * 8 + 7].color = line_color_rgb;
        *line_count = *line_count + 8;
    }

    return 0;
}


static int converRechToLine8SegBak(draw_rect_t *draw_region, int region_count, draw_line_t *draw_line, 
                                int *line_count, int line_thick, int line_color_rgb)
{
    int i = 0;
    draw_yuv_point_t rect_point[4] = {0};
    int rect_width = 0;
    int rect_height = 0;
    int line_width = 0; 
    int line_height = 0; 

    if (!draw_region || !draw_line || !line_count)
    {
        draw_rect_err("null pointer exception.\n");
        return -1;
    }
    
    *line_count = 0;

    for(i=0; i<region_count; i++)
    {
        rect_point[0].x = draw_region[i].left;
        rect_point[0].y = draw_region[i].top;
        rect_point[1].x = draw_region[i].right;
        rect_point[1].y = draw_region[i].top;
        rect_point[2].x = draw_region[i].left;
        rect_point[2].y = draw_region[i].bottom;
        rect_point[3].x = draw_region[i].right;
        rect_point[3].y = draw_region[i].bottom;
        rect_width = draw_region[i].right - draw_region[i].left;
        rect_height = draw_region[i].bottom - draw_region[i].top;
        line_width = (rect_width >> 2);
        line_height = (rect_height >> 2);
        line_width = (line_width + 1) & (~0x1);
        line_height = (line_height + 1) & (~0x1);

        draw_line[i * 8].start_point.x = rect_point[0].x;
        draw_line[i * 8].start_point.y = rect_point[0].y;
        draw_line[i * 8].end_point.x = rect_point[0].x + line_width;
        draw_line[i * 8].end_point.y = rect_point[0].y;
        draw_line[i * 8].thick = line_thick;
        draw_line[i * 8].color = line_color_rgb;
        draw_line[i * 8 + 1].start_point.x = rect_point[1].x - line_width;
        draw_line[i * 8 + 1].start_point.y = rect_point[1].y;
        draw_line[i * 8 + 1].end_point.x = rect_point[1].x;
        draw_line[i * 8 + 1].end_point.y = rect_point[1].y;
        draw_line[i * 8 + 1].thick = line_thick;
        draw_line[i * 8 + 1].color = line_color_rgb;
        draw_line[i * 8 + 2].start_point.x = rect_point[2].x;
        draw_line[i * 8 + 2].start_point.y = rect_point[2].y;
        draw_line[i * 8 + 2].end_point.x = rect_point[2].x + line_width;
        draw_line[i * 8 + 2].end_point.y = rect_point[2].y;
        draw_line[i * 8 + 2].thick = line_thick;
        draw_line[i * 8 + 2].color = line_color_rgb;
        draw_line[i * 8 + 3].start_point.x = rect_point[3].x - line_width;
        draw_line[i * 8 + 3].start_point.y = rect_point[3].y;
        draw_line[i * 8 + 3].end_point.x = rect_point[3].x;
        draw_line[i * 8 + 3].end_point.y = rect_point[3].y;
        draw_line[i * 8 + 3].thick = line_thick;
        draw_line[i * 8 + 3].color = line_color_rgb;

        draw_line[i * 8 + 4].start_point.x = rect_point[0].x;
        draw_line[i * 8 + 4].start_point.y = rect_point[0].y;
        draw_line[i * 8 + 4].end_point.x = rect_point[0].x;
        draw_line[i * 8 + 4].end_point.y = rect_point[0].y + line_height;
        draw_line[i * 8 + 4].thick = line_thick;
        draw_line[i * 8 + 4].color = line_color_rgb;
        draw_line[i * 8 + 5].start_point.x = rect_point[2].x;
        draw_line[i * 8 + 5].start_point.y = rect_point[2].y - line_height;
        draw_line[i * 8 + 5].end_point.x = rect_point[2].x;
        draw_line[i * 8 + 5].end_point.y = rect_point[2].y;
        draw_line[i * 8 + 5].thick = line_thick;
        draw_line[i * 8 + 5].color = line_color_rgb;
        draw_line[i * 8 + 6].start_point.x = rect_point[1].x;
        draw_line[i * 8 + 6].start_point.y = rect_point[1].y;
        draw_line[i * 8 + 6].end_point.x = rect_point[1].x;
        draw_line[i * 8 + 6].end_point.y = rect_point[1].y + line_height;
        draw_line[i * 8 + 6].thick = line_thick;
        draw_line[i * 8 + 6].color = line_color_rgb;
        draw_line[i * 8 + 7].start_point.x = rect_point[3].x;
        draw_line[i * 8 + 7].start_point.y = rect_point[3].y - line_height;
        draw_line[i * 8 + 7].end_point.x = rect_point[3].x;
        draw_line[i * 8 + 7].end_point.y = rect_point[3].y;
        draw_line[i * 8 + 7].thick = line_thick;
        draw_line[i * 8 + 7].color = line_color_rgb;
        *line_count = *line_count + 8;
    }

    return 0;
}


static int converRechToLine4Seg(draw_yuv_rect_t *rects, int rectCount, draw_line_t *draw_line, 
                                int *line_count, int line_thick, int line_color_rgb)
{
    int i = 0;
    draw_yuv_point_t rect_point[4] = {0};

    if (!rects || !draw_line || !line_count)
    {
        draw_rect_err("null pointer exception.\n");
        return -1;
    }

    *line_count = 0;

    for(i=0; i<rectCount; i++)
    {
        rect_point[0].x = rects[i].x; 
        rect_point[0].y = rects[i].y;
        rect_point[1].x = rects[i].x + rects[i].width - line_thick; 
        rect_point[1].y = rects[i].y;
        rect_point[2].x = rects[i].x; 
        rect_point[2].y = rects[i].y + rects[i].height - line_thick;
        rect_point[3].x = rects[i].x + rects[i].width - line_thick;
        rect_point[3].y = rects[i].y + rects[i].height - line_thick;

        rect_point[0].x = rect_point[0].x < 0 ? 0 : rect_point[0].x;
        rect_point[0].y = rect_point[0].y < 0 ? 0 : rect_point[0].y;
        rect_point[1].x = rect_point[1].x < 0 ? 0 : rect_point[1].x;
        rect_point[1].y = rect_point[1].y < 0 ? 0 : rect_point[1].y;
        rect_point[2].x = rect_point[2].x < 0 ? 0 : rect_point[2].x;
        rect_point[2].y = rect_point[2].y < 0 ? 0 : rect_point[2].y;
        rect_point[3].x = rect_point[3].x < 0 ? 0 : rect_point[3].x;
        rect_point[3].y = rect_point[3].y < 0 ? 0 : rect_point[3].y;


        draw_line[i * 4].start_point.x = rect_point[0].x; 
        draw_line[i * 4].start_point.y = rect_point[0].y;
        draw_line[i * 4].end_point.x = rect_point[1].x;
        draw_line[i * 4].end_point.y = rect_point[1].y;
        draw_line[i * 4].thick = line_thick;
        draw_line[i * 4].color = line_color_rgb;
        draw_line[i * 4 + 1].start_point.x = rect_point[2].x; 
        draw_line[i * 4 + 1].start_point.y = rect_point[2].y;
        draw_line[i * 4 + 1].end_point.x = rect_point[3].x;
        draw_line[i * 4 + 1].end_point.y = rect_point[3].y;
        draw_line[i * 4 + 1].thick = line_thick;
        draw_line[i * 4 + 1].color = line_color_rgb;


        draw_line[i * 4 + 2].start_point.x = rect_point[0].x;
        draw_line[i * 4 + 2].start_point.y = rect_point[0].y;
        draw_line[i * 4 + 2].end_point.x = rect_point[2].x;
        draw_line[i * 4 + 2].end_point.y = rect_point[2].y;
        draw_line[i * 4 + 2].thick = line_thick;
        draw_line[i * 4 + 2].color = line_color_rgb;
        draw_line[i * 4 + 3].start_point.x = rect_point[1].x;
        draw_line[i * 4 + 3].start_point.y = rect_point[1].y;
        draw_line[i * 4 + 3].end_point.x = rect_point[3].x;
        draw_line[i * 4 + 3].end_point.y = rect_point[3].y;
        draw_line[i * 4 + 3].thick = line_thick;
        draw_line[i * 4 + 3].color = line_color_rgb;
        *line_count = *line_count + 4;
    }

    return 0;
}

static void RGB2YUV(int    R, int G, int B, int *Y, int *U, int *V)
{
    float Red;
    float Green;
    float Blue;
    float Sum;
    
    //float datasetMemory = float(sampledDataset_.rows * sampledDataset_.cols * sizeof(float));

    Red = (float)0.299 * R;
    Green = (float)0.587 * G;
    Blue = (float)0.144 * B;
    Sum = Red + Green + Blue;
    *Y = Sum;

    Red = (float)-0.1684 * R;
    Green = (float)- 0.3316 * G;
    Blue = (float)0.5 * B;
    Sum = Red + Green + Blue;
    *U = Sum + 128;

    Red = (float)0.5 * R;
    Green = (float)-0.4187 * G;
    Blue = (float)-0.0813 * B;
    Sum = Red + Green + Blue;
    *V = Sum + 128;
}

/*
rgb��yuv��ת����ʽΪ
Y = 0.2990R + 0.5870G + 0.1140B
U = -0.1684R - 0.3316G + 0.5B + 128;
V = 0.5R - 0.4187G - 0.0813B + 128
*/
draw_yuv_color_t set_yuv_color(color_type color_type)
{
    draw_yuv_color_t color = {0};

    int nColor_Y = 0;
    int nColor_U = 0;
    int nColor_V = 0;    

    switch (color_type)
    {
        case COLOR_Y:
        {
            color.Y = 226;
            color.U = 0;
            color.V = 149;
            break;
        }

        case COLOR_R:
        {
            color.Y = 76;
            color.U = 85;
            color.V = 255;
            break;
        }

        case COLOR_G:
        {
            color.Y = 150;
            color.U = 44;
            color.V = 21;
            break;
        }

        case COLOR_B:
        {
            color.Y = 29;
            color.U = 255;
            color.V = 107;
            break;
        }

        case COLOR_BK:
        {
            color.Y = 16;
            color.U = 128;
            color.V = 128;
            break;
        }

        case COLOR_OR:
        {
            RGB2YUV(253, 130, 46, &nColor_Y, &nColor_U, &nColor_V);
            
            color.Y = nColor_Y;
            color.U = nColor_U;
            color.V = nColor_V;
            break;
        }

        case COLOR_PLATE:
        {
            RGB2YUV(81, 201, 238, &nColor_Y, &nColor_U, &nColor_V);

            color.Y = nColor_Y;
            color.U = nColor_U;
            color.V = nColor_V;
            break;
        }
        
        case COLOR_PINK:
        {
            RGB2YUV(184, 61, 186, &nColor_Y, &nColor_U, &nColor_V);

            color.Y = nColor_Y;
            color.U = nColor_U;
            color.V = nColor_V;
            break;
        }

        default:
        {
            color.Y = 128;
            color.U = 128;
            color.V = 128;
            break;
        }
    }

    return color;
}

int yuv420_draw_line(draw_img_data_t* img_data, draw_yuv_point_t startpoint, draw_yuv_point_t endpoint, int thick, draw_yuv_color_t color, int line_num)
{
    if (!img_data || !img_data->data)
    {
        draw_rect_err("null pointer exception.\n");
        return -1;
    }
    
    int i = 0;
    draw_yuv_point_t start_point = {0};
    draw_yuv_point_t end_point = {0};

    thick = thick <= 0 ? 2 : thick;
    line_num = line_num <= 0 ? 0 : line_num;
#if 1  //NV12
    start_point.x = startpoint.x - ((line_num >> 1) * thick);
    start_point.y = startpoint.y - ((line_num >> 1) * thick);
    end_point.x = endpoint.x - ((line_num >> 1) * thick);
    end_point.y = endpoint.y - ((line_num >> 1) * thick);

    for(i = 0; i < line_num; i++)
    {
        start_point.x = (start_point.x < 0 ? 0 : start_point.x) + (i * thick);
        start_point.y = (start_point.y < 0 ? 0 : start_point.y) + (i * thick);
        end_point.x = (end_point.x < 0 ? 0 : end_point.x) + (i * thick);
        end_point.y = (end_point.y < 0 ? 0 : end_point.y) + (i * thick);

        // edge limit
        start_point.x = start_point.x > (img_data->width - 1) ? (img_data->width - 1) : start_point.x;
        start_point.y = start_point.y > (img_data->height - 1) ? (img_data->height - 1) : start_point.y;
        end_point.x = end_point.x > (img_data->width - 1) ? (img_data->width - 1) : end_point.x;
        end_point.y = end_point.y > (img_data->height - 1) ? (img_data->height - 1) : end_point.y;

        yuv420_draw_tline(img_data, start_point, end_point, color);
    }
#else  //NV16
    start_point.x = startpoint.x - ((line_num >> 1) * thick);
    start_point.y = startpoint.y - ((line_num >> 1) * thick);
    end_point.x = endpoint.x - ((line_num >> 1) * thick);
    end_point.y = endpoint.y - ((line_num >> 1) * thick);

    for(i = 0; i < line_num; i++)
    {
        start_point.x = (start_point.x < 0 ? 0 : start_point.x) + (i * thick);
        start_point.y = (start_point.y < 0 ? 0 : start_point.y) + (i * thick);
        end_point.x = (end_point.x < 0 ? 0 : end_point.x) + (i * thick);
        end_point.y = (end_point.y < 0 ? 0 : end_point.y) + (i * thick);

        // edge limit
        start_point.x = start_point.x > (img_data->width - 1) ? (img_data->width - 1) : start_point.x;
        start_point.y = start_point.y > (img_data->height - 1) ? (img_data->height - 1) : start_point.y;
        end_point.x = end_point.x > (img_data->width - 1) ? (img_data->width - 1) : end_point.x;
        end_point.y = end_point.y > (img_data->height - 1) ? (img_data->height - 1) : end_point.y;

        yuv420_draw_tline(img_data, start_point, end_point, color);
    }
#endif

#if 0
    start_point.x = start_point.x < 0 ? 0 : start_point.x;
    start_point.y = start_point.y < 0 ? 0 : start_point.y;
    end_point.x = end_point.x < 0 ? 0 : end_point.x;
    end_point.y = end_point.y < 0 ? 0 : end_point.y;

    yuv420_draw_tline(img_data, start_point, end_point, color);

    start_point.x = startpoint.x;
    start_point.y = startpoint.y;
    end_point.x = endpoint.x;
    end_point.y = endpoint.y;
    yuv420_draw_tline(img_data, start_point, end_point, color);

    start_point.x = startpoint.x + thick;
    start_point.y = startpoint.y + thick;
    end_point.x = endpoint.x + thick;
    end_point.y = endpoint.y + thick;
    yuv420_draw_tline(img_data, start_point, end_point, color);
#endif
    return 0;
}

int yuv420_draw_rectangle(draw_img_data_t* img_data, draw_yuv_rect_t rect, int thick, draw_yuv_color_t color)
{
    if (!img_data || !img_data->data)
    {    
        draw_rect_err("null pointer exception.\n");
        return -1;
    }

    int x = rect.x;
    int y = rect.y;
    draw_yuv_point_t point[4] = {0};

    point[0].x = x;
    point[0].y = y;
    point[1].x = x + rect.width - thick;
    point[1].y = y;
    point[2].x = x + rect.width - thick;
    point[2].y = y + rect.height - thick;
    point[3].x = x;
    point[3].y = y + rect.height - thick;

    yuv420_draw_line(img_data, point[0], point[1], thick, color, LINE_NUMBER);
    yuv420_draw_line(img_data, point[1], point[2], thick, color, LINE_NUMBER);
    yuv420_draw_line(img_data, point[3], point[2], thick, color, LINE_NUMBER);
    yuv420_draw_line(img_data, point[0], point[3], thick, color, LINE_NUMBER);

    return 0;
}

int yuv420_draw_rectangles_dotted(draw_img_data_t* img_data, draw_yuv_rect_t *rects, int rect_num, int thick, draw_yuv_color_t color, int thick_num)
{
    if (!img_data || !img_data->data)
    {    
        draw_rect_err("null pointer exception.\n");
        return -1;
    }

    if (0 == rect_num)
    {
        return 0;
    }

    int i = 0;
    int real_lines = 0;
    int line_num = (rect_num << 3);
    draw_line_t *draw_lines = (draw_line_t *)calloc(line_num, sizeof(draw_line_t));
    if (NULL == draw_lines)
    {
        draw_rect_err("Fail to calloc draw_lines.\n");
        return -1;
    }

    converRechToLine8Seg(rects, rect_num, draw_lines, &real_lines, thick, 0xffffff);

    for (i=0; i<real_lines; i++)
    {
        yuv420_draw_line(img_data, draw_lines[i].start_point, draw_lines[i].end_point, thick, color, thick_num);
    }

    free(draw_lines);
    draw_lines = NULL;

    return 0;
}


int yuv420_draw_rectangles_solid(draw_img_data_t* img_data, draw_yuv_rect_t *rects, int rect_num, int thick, draw_yuv_color_t color, int thick_num)
{
    if (!img_data || !img_data->data)
    {    
        draw_rect_err("null pointer exception.\n");
        return -1;
    }

    if (0 == rect_num)
    {
        return 0;
    }

    int i = 0;
    int real_lines = 0;
    int line_num = (rect_num << 2);
    draw_line_t *draw_lines = (draw_line_t *)calloc(line_num, sizeof(draw_line_t));
    if (NULL == draw_lines)
    {
        draw_rect_err("Fail to calloc draw_lines.\n");
        return -1;
    }

    converRechToLine4Seg(rects, rect_num, draw_lines, &real_lines, thick, 0xffffff);

    for (i=0; i<real_lines; i++)
    {
        yuv420_draw_line(img_data, draw_lines[i].start_point, draw_lines[i].end_point, thick, color, thick_num);
    }

    free(draw_lines);
    draw_lines = NULL;

    return 0;
}

int yuv420_draw_rectangle_8_segment(draw_img_data_t* img_data, draw_rect_t *rect, int thick, draw_yuv_color_t color)
{
    if (!img_data || !img_data->data)
    {
        draw_rect_err("null pointer exception.\n");
        return -1;
    }

    int line_count = 0;
    draw_rect_t regions[1] = {0};
    draw_line_t draw_line[8] = {0};

    regions[0].left = (rect->left + 1) & (~0x1);
    regions[0].right = (rect->right + 1) & (~0x1);
    regions[0].top = (rect->top + 1) & (~0x1);
    regions[0].bottom = (rect->bottom + 1) & (~0x1);
    converRechToLine8SegBak(regions, 1, draw_line, &line_count, thick, 0xffffff);

    yuv420_draw_line(img_data, draw_line[0].start_point, draw_line[0].end_point, thick, color, LINE_NUMBER);
    yuv420_draw_line(img_data, draw_line[1].start_point, draw_line[1].end_point, thick, color, LINE_NUMBER);
    yuv420_draw_line(img_data, draw_line[2].start_point, draw_line[2].end_point, thick, color, LINE_NUMBER);
    yuv420_draw_line(img_data, draw_line[3].start_point, draw_line[3].end_point, thick, color, LINE_NUMBER);
    yuv420_draw_line(img_data, draw_line[4].start_point, draw_line[4].end_point, thick, color, LINE_NUMBER);
    yuv420_draw_line(img_data, draw_line[5].start_point, draw_line[5].end_point, thick, color, LINE_NUMBER);
    yuv420_draw_line(img_data, draw_line[6].start_point, draw_line[6].end_point, thick, color, LINE_NUMBER);
    yuv420_draw_line(img_data, draw_line[7].start_point, draw_line[7].end_point, thick, color, LINE_NUMBER);

    return 0;
}

int yuv420_draw_rectangles_8_segment(draw_img_data_t* img_data, draw_rect_t* rects, int rect_num, int thick, draw_yuv_color_t color)
{
    if (!img_data || !img_data->data || !rects)
    {
        draw_rect_err("null pointer exception.\n");
        return -1;
    }

    int i = 0;
    int real_lines = 0;
    int line_num = (rect_num << 3);
    draw_line_t *draw_lines = (draw_line_t *)calloc(line_num, sizeof(draw_line_t));
    if (NULL == draw_lines)
    {
        draw_rect_err("Fail to calloc draw_lines.\n");
        return -1;
    }

    for (i=0; i<rect_num; i++)
    {
        rects[0].left = (rects[0].left + 1) & (~0x1);
        rects[0].right = (rects[0].right + 1) & (~0x1);
        rects[0].top = (rects[0].top + 1) & (~0x1);
        rects[0].bottom = (rects[0].bottom + 1) & (~0x1);
    }
    converRechToLine8SegBak(rects, rect_num, draw_lines, &real_lines, thick, 0xffffff);

    for (i=0; i<real_lines; i++)
    {
        yuv420_draw_line(img_data, draw_lines[0].start_point, draw_lines[0].end_point, thick, color, LINE_NUMBER);
    }

    free(draw_lines);
    draw_lines = NULL;

    return 0;
}

int yuv420_draw_polygon(draw_img_data_t* img_data, draw_yuv_point_t *points, int point_num, int thick, draw_yuv_color_t color, int thick_num)
{
    if (!img_data || !img_data->data || !points)
    {
        draw_rect_err("null pointer exception.\n");
        return -1;
    }

    int i = 0;
    for (i=0; i<(point_num-1); i++)
    {
        yuv420_draw_line(img_data, points[i], points[i+1], thick, color, thick_num);
    }

    yuv420_draw_line(img_data, points[i], points[0], thick, color, thick_num);

    return 0;
}


static void rgb_to_yuv(uint8_t r, uint8_t g, uint8_t b, uint8_t *y, uint8_t *u, uint8_t *v) {
    *y = (0.257 * r) + (0.504 * g) + (0.098 * b) + 16;
    *u = -(0.148 * r) - (0.291 * g) + (0.439 * b) + 128;
    *v = (0.439 * r) - (0.368 * g) - (0.071 * b) + 128;
}


int draw_bitmap_on_nv12(uint8_t *nv12_data, int yuv_width, int yuv_height, 
                        const uint8_t *bitmap_data, int bmp_width, int bmp_height, 
                        int start_x, int start_y) {
    for (int j = 0; j < bmp_height; j++) {
        for (int i = 0; i < bmp_width; i++) {
            int yuv_x = start_x + i;
            int yuv_y = start_y + j;
            if (yuv_x >= 0 && yuv_x < yuv_width && yuv_y >= 0 && yuv_y < yuv_height) {
                uint8_t r = bitmap_data[(j * bmp_width + i) * 3 + 0];
                uint8_t g = bitmap_data[(j * bmp_width + i) * 3 + 1];
                uint8_t b = bitmap_data[(j * bmp_width + i) * 3 + 2];

                uint8_t y, u, v;
                rgb_to_yuv(r, g, b, &y, &u, &v);

                nv12_data[yuv_y * yuv_width + yuv_x] = y;

                if (yuv_x % 2 == 0 && yuv_y % 2 == 0) {
                    int uv_index = yuv_width * yuv_height + (yuv_y / 2) * yuv_width + yuv_x;
                    nv12_data[uv_index] = u; 
                    nv12_data[uv_index + 1] = v; 
                }
            }
        }
    }
    return 0;
}

// gwp add below 20241119
void yuv_setdata(
	uInt8* YBuff,
	uInt8* UVBuff,
	enYuvType yuvType,
	uInt16 width,
	uInt16 height,
	stPoint draw_point,
	enYuvColorIdx clrIdx)
{
	switch(yuvType)
	{
		case TYPE_YUV422I_UYVY:
		case TYPE_YUV422I_YUYV:
		{
			/*
				UYVY UYVY UYVY UYVY
			*/
			uInt32 tmp = draw_point.y * width * 2;
			uInt32 y_offset = 0, u_offset = 0, v_offset = 0;
			if(yuvType == TYPE_YUV422I_UYVY) {
				u_offset = tmp + draw_point.x / 2 * 4;
				v_offset = u_offset + 2;
				y_offset = u_offset + 1;
			}
			else {
				y_offset = tmp + draw_point.x / 2 * 4;
				u_offset = y_offset + 1;
				v_offset = u_offset + 2;
			}
			YBuff[y_offset] = s_color_table[clrIdx].Y;
			YBuff[y_offset + 2] = s_color_table[clrIdx].Y;
			YBuff[u_offset] = s_color_table[clrIdx].U;
			YBuff[v_offset] = s_color_table[clrIdx].V;
		}break;
		case TYPE_YUV420SP_NV12:
		case TYPE_YUV420SP_NV21:
		{
			/*
				YY YY
				YY YY
				UV UV
			*/
			uInt32 y_offset = draw_point.y * width + draw_point.x;
			uInt32 u_offset = 0, v_offset = 0;
			YBuff[y_offset] = s_color_table[clrIdx].Y;
			#if 0
			Int32 x_flag = 1, y_flag = 1;
			if(draw_point.y % 2 == 0) {
				YBuff[y_offset + width] = s_color_table[clrIdx].Y;
				y_flag = 1;
			}
			else {
				YBuff[y_offset - width] = s_color_table[clrIdx].Y;
				y_flag = -1;
			}
 
			if(draw_point.x % 2 == 0) {
				YBuff[y_offset + 1] = s_color_table[clrIdx].Y;
				x_flag = 1;
			}
			else {
				YBuff[y_offset - 1] = s_color_table[clrIdx].Y;
				x_flag = -1;
			}
			YBuff[y_offset + width * y_flag + 1 * x_flag] = s_color_table[clrIdx].Y;
			#endif
			
			if(yuvType == TYPE_YUV420SP_NV12) {
				u_offset = (draw_point.y / 2) * width + draw_point.x / 2 * 2;
				v_offset = u_offset + 1;
			}
			else {
				v_offset = (draw_point.y / 2) * width + draw_point.x / 2 * 2;
				u_offset = v_offset + 1;
			}
			UVBuff[u_offset] = s_color_table[clrIdx].U;
			UVBuff[v_offset] = s_color_table[clrIdx].V;
			//printf("[%d, %d]: y_offset = %d, u_offset = %d, v_offset = %d\n",
			//	draw_point.x, draw_point.y, y_offset, u_offset, v_offset);
		}break;
		case TYPE_YUV422SP_NV16:
        case TYPE_YUV422SP_NV61:
		{
			/*
				YY YY
				YY YY
				UV UV
			*/
			uInt32 y_offset = draw_point.y * width + draw_point.x;
			uInt32 u_offset = 0, v_offset = 0;
			YBuff[y_offset] = s_color_table[clrIdx].Y;
			#if 0
			Int32 x_flag = 1, y_flag = 1;
			if(draw_point.y % 2 == 0) {
				YBuff[y_offset + width] = s_color_table[clrIdx].Y;
				y_flag = 1;
			}
			else {
				YBuff[y_offset - width] = s_color_table[clrIdx].Y;
				y_flag = -1;
			}
 
			if(draw_point.x % 2 == 0) {
				YBuff[y_offset + 1] = s_color_table[clrIdx].Y;
				x_flag = 1;
			}
			else {
				YBuff[y_offset - 1] = s_color_table[clrIdx].Y;
				x_flag = -1;
			}
			YBuff[y_offset + width * y_flag + 1 * x_flag] = s_color_table[clrIdx].Y;
			#endif
			
			if(yuvType == TYPE_YUV422SP_NV16) {
				u_offset = (draw_point.y) * width + draw_point.x / 2 * 2;
				v_offset = u_offset + 1;
			}
			else {
				v_offset = (draw_point.y) * width + draw_point.x / 2 * 2;
				u_offset = v_offset + 1;
			}
			UVBuff[u_offset] = s_color_table[clrIdx].U;
			UVBuff[v_offset] = s_color_table[clrIdx].V;
			//printf("[%d, %d]: y_offset = %d, u_offset = %d, v_offset = %d\n",
			//	draw_point.x, draw_point.y, y_offset, u_offset, v_offset);
		}break;
		case TYPE_YUV444P:
		{
			/*
				YYYYYYYY
				UUUUUUUU
				VVVVVVVV
			*/
			uInt32 y_offset = 0, u_offset = 0, v_offset = 0;
			uInt32 plane_size = width * height;
			y_offset = draw_point.y * width + draw_point.x;
			u_offset = y_offset;
			v_offset = plane_size + u_offset;
			YBuff[y_offset] = s_color_table[clrIdx].Y;
			UVBuff[u_offset] = s_color_table[clrIdx].U;
			UVBuff[v_offset] = s_color_table[clrIdx].V;
		}break;
		case TYPE_YUV444I:
		{
			/*
				YUV YUV YUV YUV YUV YUV YUV YUV
			*/
			uInt32 y_offset = 0, u_offset = 0, v_offset = 0;
			y_offset = draw_point.y * width * 3 + draw_point.x * 3;
			u_offset = y_offset + 1;
			v_offset = u_offset + 1;
			YBuff[y_offset] = s_color_table[clrIdx].Y;
			YBuff[u_offset] = s_color_table[clrIdx].U;
			YBuff[v_offset] = s_color_table[clrIdx].V;
		}break;
		case TYPE_YUV422P:
		{
			/*
				YYYYYYYY
				UUUU
				VVVV
			*/
			uInt32 y_offset = 0, u_offset = 0, v_offset = 0;
			uInt32 plane_size = width * height / 2;
			y_offset = draw_point.y * width + draw_point.x;
			u_offset = y_offset / 2;
			v_offset = plane_size + y_offset / 2;
			YBuff[y_offset] = s_color_table[clrIdx].Y;
			UVBuff[u_offset] = s_color_table[clrIdx].U;
			UVBuff[v_offset] = s_color_table[clrIdx].V;
		}break;
		case TYPE_YUV420I:
		{
			/*
				YYYYYYYY
				UU
				VV
			*/
			uInt32 y_offset = 0, u_offset = 0, v_offset = 0;
			uInt32 plane_size = width * height / 4;
			y_offset = draw_point.y * width + draw_point.x;
			u_offset = draw_point.y / 2 * width / 2 + draw_point.x/2;
			v_offset = u_offset + plane_size;
			YBuff[y_offset] = s_color_table[clrIdx].Y;
			UVBuff[u_offset] = s_color_table[clrIdx].U;
			UVBuff[v_offset] = s_color_table[clrIdx].V;
		}break;
	}
}

void yuv_drawline(const stYuvBuffInfo *pYuvBuffInfo, stDrawLineInfo *pDrawLineInfo)
{
	if(!pYuvBuffInfo || !pYuvBuffInfo->pYuvBuff) return;
 
	uInt8 *YBuff = NULL, *UVBuff = NULL;
	if(pDrawLineInfo->lineWidth == 0) pDrawLineInfo->lineWidth = 1;
	
	uInt16 i = 0;
	for(i = 0; i < pDrawLineInfo->lineWidth; i++) {
		uInt16 x0 = pDrawLineInfo->startPoint.x, y0 = pDrawLineInfo->startPoint.y;
		uInt16 x1 = pDrawLineInfo->endPoint.x, y1 = pDrawLineInfo->endPoint.y;
    #if 0 //gwp delete 20241114    
		x0 = (x0 >= pYuvBuffInfo->width) ? (x0 - pDrawLineInfo->lineWidth) : x0;
		x1 = (x1 >= pYuvBuffInfo->width) ? (x1 - pDrawLineInfo->lineWidth) : x1;
		y0 = (y0 >= pYuvBuffInfo->height) ? (y0 - pDrawLineInfo->lineWidth) : y0;
		y1 = (y1 >= pYuvBuffInfo->height) ? (y1 - pDrawLineInfo->lineWidth) : y1;
    #else
        x0 = (x0 >= pYuvBuffInfo->width- pDrawLineInfo->lineWidth) ? (x0 - pDrawLineInfo->lineWidth) : x0;
		x1 = (x1 >= pYuvBuffInfo->width- pDrawLineInfo->lineWidth) ? (x1 - pDrawLineInfo->lineWidth) : x1;
		y0 = (y0 >= pYuvBuffInfo->height- pDrawLineInfo->lineWidth) ? (y0 - pDrawLineInfo->lineWidth) : y0;
		y1 = (y1 >= pYuvBuffInfo->height- pDrawLineInfo->lineWidth) ? (y1 - pDrawLineInfo->lineWidth) : y1;
    #endif
		
		uInt16 dx = (x0 > x1) ? (x0 - x1) : (x1 - x0);
		uInt16 dy = (y0 > y1) ? (y0 - y1) : (y1 - y0);
 
		if(dx <= dy) {
			x0 += i;
			x1 += i;
		}
		else {
			y0 += i;
			y1 += i;
		}
 
		Int16 xstep = (x0 < x1) ? 1 : -1;
		Int16 ystep = (y0 < y1) ? 1 : -1;
		Int16 nstep = 0, eps = 0;
 
		stPoint draw_point;
		draw_point.x = x0;
		draw_point.y = y0;
 
		switch(pYuvBuffInfo->yuvType)
		{
			case TYPE_YUV422I_UYVY:
			case TYPE_YUV422I_YUYV:
			case TYPE_YUV444I:
			{
				YBuff = pYuvBuffInfo->pYuvBuff;
				UVBuff = NULL;
			}break;
			case TYPE_YUV420SP_NV12:
			case TYPE_YUV420SP_NV21:
			case TYPE_YUV422SP_NV16:
			case TYPE_YUV422SP_NV61:
			case TYPE_YUV444P:
			case TYPE_YUV422P:
			case TYPE_YUV420I:
			{
				YBuff = pYuvBuffInfo->pYuvBuff;
				UVBuff = pYuvBuffInfo->pYuvBuff + pYuvBuffInfo->width * pYuvBuffInfo->height;
			}break;
			default:
				return;
		}
 
		// 布雷森汉姆算法画线
		if(dx > dy){
			while(nstep <= dx) {
				yuv_setdata(YBuff, UVBuff, pYuvBuffInfo->yuvType, pYuvBuffInfo->width, pYuvBuffInfo->height, draw_point, pDrawLineInfo->clrIdx);
				eps += dy;
				if( (eps << 1) >= dx ) {
					draw_point.y += ystep;
					eps -= dx;
				}
				draw_point.x += xstep;
				nstep++;
			}
		}else {
			while(nstep <= dy){	
				yuv_setdata(YBuff, UVBuff, pYuvBuffInfo->yuvType, pYuvBuffInfo->width, pYuvBuffInfo->height, draw_point, pDrawLineInfo->clrIdx);
				eps += dx;
				if( (eps << 1) >= dy ) {
					draw_point.x += xstep;
					eps -= dy;
				}
				draw_point.y += ystep;
				nstep++;
			}
		}
	}
}

void yuv_draw_rectline(const stYuvBuffInfo *pYuvBuffInfo, stDrawLineInfo *pDrawLineInfo, enRectLinePos RectLinePos)
{
	if(!pYuvBuffInfo || !pYuvBuffInfo->pYuvBuff) return;
 
	uInt8 *YBuff = NULL, *UVBuff = NULL;
	if(pDrawLineInfo->lineWidth == 0) pDrawLineInfo->lineWidth = 1;
	
    // 校验线宽大于1的情况下，是否x0>x1,y0>y1
    if (pDrawLineInfo->lineWidth > 1)
    {
        switch(RectLinePos)
		{
			case POS_H_TOP:
            case POS_H_BOTTOM:
            {
                if (pDrawLineInfo->startPoint.x + (pDrawLineInfo->lineWidth -1)*2 >=  pDrawLineInfo->endPoint.x 
                        || pDrawLineInfo->startPoint.y != pDrawLineInfo->endPoint.y)
                {
                    printf("draw rect H line(%d) input error!(x0,y0)=(%d,%d),(x1,y1)=(%d,%d)\n",RectLinePos,
                    pDrawLineInfo->startPoint.x,
                    pDrawLineInfo->startPoint.y,
                    pDrawLineInfo->endPoint.x,
                    pDrawLineInfo->endPoint.y);
                    return;
                }
                break;
            }
			case POS_V_LEFT:
            case POS_V_RIGHT:
			{
                if (pDrawLineInfo->startPoint.y + (pDrawLineInfo->lineWidth -1)*2 >=  pDrawLineInfo->endPoint.y 
                        || pDrawLineInfo->startPoint.x != pDrawLineInfo->endPoint.x)
                {
                    printf("draw rect V line(%d) input error!(x0,y0)=(%d,%d),(x1,y1)=(%d,%d)\n",RectLinePos,
                    pDrawLineInfo->startPoint.x,
                    pDrawLineInfo->startPoint.y,
                    pDrawLineInfo->endPoint.x,
                    pDrawLineInfo->endPoint.y);
                    return;
                }
                break;
            }
			default:
            {
                printf("draw rect line input error!\n");
                return;
            }	
		}
    }
    
	uInt16 i = 0;
	for(i = 0; i < pDrawLineInfo->lineWidth; i++) {
		uInt16 x0 = pDrawLineInfo->startPoint.x, y0 = pDrawLineInfo->startPoint.y;
		uInt16 x1 = pDrawLineInfo->endPoint.x, y1 = pDrawLineInfo->endPoint.y;
    #if 0 //gwp delete 20241114    
		x0 = (x0 >= pYuvBuffInfo->width) ? (x0 - pDrawLineInfo->lineWidth) : x0;
		x1 = (x1 >= pYuvBuffInfo->width) ? (x1 - pDrawLineInfo->lineWidth) : x1;
		y0 = (y0 >= pYuvBuffInfo->height) ? (y0 - pDrawLineInfo->lineWidth) : y0;
		y1 = (y1 >= pYuvBuffInfo->height) ? (y1 - pDrawLineInfo->lineWidth) : y1;
    #else
        x0 = (x0 >= pYuvBuffInfo->width- pDrawLineInfo->lineWidth) ? (x0 - pDrawLineInfo->lineWidth) : x0;
		x1 = (x1 >= pYuvBuffInfo->width- pDrawLineInfo->lineWidth) ? (x1 - pDrawLineInfo->lineWidth) : x1;
		y0 = (y0 >= pYuvBuffInfo->height- pDrawLineInfo->lineWidth) ? (y0 - pDrawLineInfo->lineWidth) : y0;
		y1 = (y1 >= pYuvBuffInfo->height- pDrawLineInfo->lineWidth) ? (y1 - pDrawLineInfo->lineWidth) : y1;
    #endif
		

        uInt16 dx,dy;
        // printf("(x0,y0)=(%d,%d),(x1,y1)=(%d,%d)\n",x0,y0,x1,y1);
        switch(RectLinePos)
		{
			case POS_H_TOP:
            {
                y0 += i;
                y1 += i;
                x0 += i;
                x1 -= i;
                break;
            }
			case POS_H_BOTTOM:
            {
                y0 -= i;
                y1 -= i;
                x0 += i;
                x1 -= i;
                break;
            }
			case POS_V_LEFT:
			{
                x0 += i;
                x1 += i;
                y0 += i;
                y1 -= i;
                break;
            }
			case POS_V_RIGHT:
            {
                x0 -= i;
                x1 -= i;
                y0 += i;
                y1 -= i;
                break;
            }
			default:
            {
                printf("draw rect line input error!\n");
                return;
            }
				
		}
        dx = (x0 > x1) ? (x0 - x1) : (x1 - x0);
		dy = (y0 > y1) ? (y0 - y1) : (y1 - y0);
        // printf("new (x0,y0)=(%d,%d),(x1,y1)=(%d,%d),(dx,dy)=(%d,%d)\n",x0,y0,x1,y1,dx,dy);
            
		Int16 xstep = (x0 < x1) ? 1 : -1;
		Int16 ystep = (y0 < y1) ? 1 : -1;
		Int16 nstep = 0, eps = 0;
 
		stPoint draw_point;
		draw_point.x = x0;
		draw_point.y = y0;
 
		switch(pYuvBuffInfo->yuvType)
		{
			case TYPE_YUV422I_UYVY:
			case TYPE_YUV422I_YUYV:
			case TYPE_YUV444I:
			{
				YBuff = pYuvBuffInfo->pYuvBuff;
				UVBuff = NULL;
			}break;
			case TYPE_YUV420SP_NV12:
			case TYPE_YUV420SP_NV21:
			case TYPE_YUV422SP_NV16:
			case TYPE_YUV422SP_NV61:
			case TYPE_YUV444P:
			case TYPE_YUV422P:
			case TYPE_YUV420I:
			{
				YBuff = pYuvBuffInfo->pYuvBuff;
				UVBuff = pYuvBuffInfo->pYuvBuff + pYuvBuffInfo->width * pYuvBuffInfo->height;
			}break;
			default:
				return;
		}
 
		// 布雷森汉姆算法画线
		if(dx > dy){
			while(nstep <= dx) {
				yuv_setdata(YBuff, UVBuff, pYuvBuffInfo->yuvType, pYuvBuffInfo->width, pYuvBuffInfo->height, draw_point, pDrawLineInfo->clrIdx);
				eps += dy;
				if( (eps << 1) >= dx ) {
					draw_point.y += ystep;
					eps -= dx;
				}
				draw_point.x += xstep;
				nstep++;
			}
		}else {
			while(nstep <= dy){	
				yuv_setdata(YBuff, UVBuff, pYuvBuffInfo->yuvType, pYuvBuffInfo->width, pYuvBuffInfo->height, draw_point, pDrawLineInfo->clrIdx);
				eps += dx;
				if( (eps << 1) >= dy ) {
					draw_point.x += xstep;
					eps -= dy;
				}
				draw_point.y += ystep;
				nstep++;
			}
		}
	}
}
