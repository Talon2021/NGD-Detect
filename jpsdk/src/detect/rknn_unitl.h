#ifndef RKNN_UNITL_H
#define RKNN_UNITL_H

#include "rknn_api.h"
#include "image_common.h"
#include "image_utils.h"
typedef struct {
    rknn_context rknn_ctx;
    rknn_input_output_num io_num;
    rknn_tensor_attr* input_attrs;
    rknn_tensor_attr* output_attrs;
    int model_channel;
    int model_width;
    int model_height;
    int input_image_width;
    int input_image_height;
    bool is_quant;
} rknn_app_context_t;


#define OBJ_NAME_MAX_SIZE 64
#define OBJ_NUMB_MAX_SIZE 128
#define OBJ_CLASS_NUM 1
#define NMS_THRESH 0.45
#define BOX_THRESH 0.25
#define PROP_BOX_SIZE (5 + OBJ_CLASS_NUM)

#define PROTO_CHANNEL 32
#define PROTO_HEIGHT 160
#define PROTO_WEIGHT 160

#define N_CLASS_COLORS 20

// class rknn_app_context_t;

typedef struct
{
    image_rect_t box;
    float prop;
    int cls_id;
} object_detect_result;

typedef struct
{
    uint8_t *seg_mask;
} object_segment_result;

typedef struct
{
    int id;
    int count;
    object_detect_result results[OBJ_NUMB_MAX_SIZE];
    object_segment_result results_seg[OBJ_NUMB_MAX_SIZE];
} object_detect_result_list;


int init_post_process();
void deinit_post_process();
char *coco_cls_to_name(int cls_id);
int post_process(rknn_app_context_t *app_ctx, rknn_output *outputs, letterbox_t *letter_box, float conf_threshold, float nms_threshold, object_detect_result_list *od_results);
int clamp(float val, int min, int max);


int init_yolov8_seg_model(const char* model_path, rknn_app_context_t* app_ctx);

int release_yolov8_seg_model(rknn_app_context_t* app_ctx);

int inference_yolov8_seg_model(rknn_app_context_t* app_ctx, image_buffer_t* img, object_detect_result_list* od_results);

#endif