
#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include "IR_GasApi.h"

typedef struct
{
    int vaild;
    GAS_DETECT_RESULT result;
}gas_result_cache;

#define SRC_PIC_WIDTH   1024
#define SRC_PIC_HEIGHT   1024
#define N_CLASS_COLORS 20
static pthread_mutex_t result_mutex = PTHREAD_MUTEX_INITIALIZER;
static gas_result_cache result_cache = {0};

static int clamp(float val, int min, int max)
{
    return val > min ? (val < max ? val : max) : min;
}
static unsigned char class_colors[][3] = {
        {255, 56, 56},   // 'FF3838'
        {255, 157, 151}, // 'FF9D97'
        {255, 112, 31},  // 'FF701F'
        {255, 178, 29},  // 'FFB21D'
        {207, 210, 49},  // 'CFD231'
        {72, 249, 10},   // '48F90A'
        {146, 204, 23},  // '92CC17'
        {61, 219, 134},  // '3DDB86'
        {26, 147, 52},   // '1A9334'
        {0, 212, 187},   // '00D4BB'
        {44, 153, 168},  // '2C99A8'
        {0, 194, 255},   // '00C2FF'
        {52, 69, 147},   // '344593'
        {100, 115, 255}, // '6473FF'
        {0, 24, 236},    // '0018EC'
        {132, 56, 255},  // '8438FF'
        {82, 0, 133},    // '520085'
        {203, 56, 255},  // 'CB38FF'
        {255, 149, 200}, // 'FF95C8'
        {255, 55, 199}   // 'FF37C7'
    };

static int AlgoResultCallback(GAS_DETECT_RESULT *result, int len)
{
    pthread_mutex_lock(&result_mutex);
    fprintf(stderr, "result frameid = %u \n", result->frame_id);
    memcpy(result_cache.result.stGasAttr, result->stGasAttr, result->u8GasNum);
    result_cache.result.u8GasNum = result->u8GasNum;
    if(result_cache.result.u8GasNum >= 1)
    {
        memcpy(result_cache.result.gas_mask, result->gas_mask, SRC_PIC_WIDTH * SRC_PIC_HEIGHT);
    }
    result_cache.vaild = 1;
    pthread_mutex_unlock(&result_mutex);
    return 0;
}

static int WriteOutImage(uint8_t *buff, int width, int height)
{
    static int no_result_frame = 0;
    char *ori_img = (char *)buff;
    pthread_mutex_lock(&result_mutex);
    if(result_cache.vaild == 1)
    {
        no_result_frame = 0;
        float alpha = 0.5f;
        uint8_t *seg_mask = result_cache.result.gas_mask;
        if(result_cache.result.u8GasNum >= 1)
        {
            for (int j = 0; j < height; j++)
            {
                for (int k = 0; k < width; k++)
                {
                    int pixel_offset = 3 * (j * width + k);
                    if (seg_mask[j * width + k] != 0)
                    {
                        ori_img[pixel_offset + 0] = (unsigned char)clamp(class_colors[seg_mask[j * width + k] % N_CLASS_COLORS][0] * (1 - alpha) + ori_img[pixel_offset + 0] * alpha, 0, 255); // r
                        ori_img[pixel_offset + 1] = (unsigned char)clamp(class_colors[seg_mask[j * width + k] % N_CLASS_COLORS][1] * (1 - alpha) + ori_img[pixel_offset + 1] * alpha, 0, 255); // g
                        ori_img[pixel_offset + 2] = (unsigned char)clamp(class_colors[seg_mask[j * width + k] % N_CLASS_COLORS][2] * (1 - alpha) + ori_img[pixel_offset + 2] * alpha, 0, 255); // b
                    }
                }
            }
        }
    }
    else
    {
        if(no_result_frame < 3)
        {
            float alpha = 0.5f;
            uint8_t *seg_mask = result_cache.result.gas_mask;
            if(result_cache.result.u8GasNum >= 1)
            {
                for (int j = 0; j < height; j++)
                {
                    for (int k = 0; k < width; k++)
                    {
                        int pixel_offset = 3 * (j * width + k);
                        if (seg_mask[j * width + k] != 0)
                        {
                            ori_img[pixel_offset + 0] = (unsigned char)clamp(class_colors[seg_mask[j * width + k] % N_CLASS_COLORS][0] * (1 - alpha) + ori_img[pixel_offset + 0] * alpha, 0, 255); // r
                            ori_img[pixel_offset + 1] = (unsigned char)clamp(class_colors[seg_mask[j * width + k] % N_CLASS_COLORS][1] * (1 - alpha) + ori_img[pixel_offset + 1] * alpha, 0, 255); // g
                            ori_img[pixel_offset + 2] = (unsigned char)clamp(class_colors[seg_mask[j * width + k] % N_CLASS_COLORS][2] * (1 - alpha) + ori_img[pixel_offset + 2] * alpha, 0, 255); // b
                        }
                    }
                }
            }
        }
        no_result_frame++;
    }
    pthread_mutex_unlock(&result_mutex);
    return 0;
}
static void yuv420_to_rgb(const uint8_t* yuv_data, uint8_t* rgb_data, int width, int height) {
    int frame_size = width * height;
    for (int i = 0; i < frame_size; ++i) {
        uint8_t y = yuv_data[i];  

        rgb_data[3 * i] = y;     // R
        rgb_data[3 * i + 1] = y; // G
        rgb_data[3 * i + 2] = y; // B
    }
}

int main(void)
{
    printf("start\n");
    int ret = 0;
    static uint64_t frame_id = 0;
    ret = IR_Sys_Init();
    if(ret < 0)
    {
        fprintf(stderr, "init sys is err \n");
        return -1;
    }
    IR_Sys_RegisterResultCAllBack(AlgoResultCallback);

    result_cache.result.gas_mask = (uint8_t *)malloc(SRC_PIC_WIDTH * SRC_PIC_HEIGHT);

    int size = SRC_PIC_WIDTH * SRC_PIC_HEIGHT * 3 / 2 ;

    int out_size = SRC_PIC_WIDTH * SRC_PIC_HEIGHT * 3; 

    unsigned char *frame_buff = (unsigned char *)calloc(1, size);

    unsigned char *rgb_frame = (unsigned char *)calloc(1, out_size);
    FILE *file_input = fopen("input_1024.yuv", "r");
    if(file_input == NULL)
    {
        perror("Error opening file");
        return -1;
    }

    FILE *out = fopen("./output.yuv", "w+");
    while(1)
    {
        int bytes_read = fread(frame_buff, 1, size , file_input);
        if (bytes_read != size)
        {
            if (feof(file_input)) 
            {
               break;
            }
        }
        img_data data;
        data.bpp = 12;
        data.height = SRC_PIC_HEIGHT;
        data.widht = SRC_PIC_WIDTH;
        data.img = frame_buff;
        data.size = size;
        data.frame_id = frame_id++;
        IR_Sys_AlgoSendFrame(&data);
        yuv420_to_rgb(frame_buff, rgb_frame, SRC_PIC_WIDTH, SRC_PIC_HEIGHT);

        WriteOutImage(rgb_frame, SRC_PIC_WIDTH, SRC_PIC_HEIGHT);
        fwrite(rgb_frame, 1, out_size, out);
        usleep(40 *1000);

    }
    fclose(file_input);
    fclose(out);
    printf("read file is end \n");
    while (1)
    {
        sleep(10);
    }
    
    
    return 0;
}