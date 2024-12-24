/*
 **************************************************************************************************************
 * @Time    : 2024/3/29 11:09
 * @Author  : DaiPuWei
 * @Email   : puwei.dai@Vitalchem.com
 * Copyright (c) 2024 Vitalchem All rights reserved.
 **************************************************************************************************************
 */

#ifndef DETECTION_CXX_DETECTION_COMMON_H
#define DETECTION_CXX_DETECTION_COMMON_H

#include <string>
#include <vector>

#ifndef EXPORT_API
#ifdef _WIN64
#define EXPORT_API __declspec(dllexport)
#else
#define EXPORT_API __attribute__ ((visibility("default")))
#endif
#endif


// 自定义检测结果结构体
typedef struct DetectionResult{
    float x1,y1,x2,y2;              // 检测框左上右下坐标
    float score;                    // 检测框得分
    int cls_id;                     // 分类id号
    std::string cls_name;           // 分类名称

    /**
     * @brief 这是检测结果结构体构造函数
     * @param x1 检测框左上点x坐标
     * @param y1 检测框左上点y坐标
     * @param x2 检测框右下点x坐标
     * @param y2 检测框右下点y坐标
     * @param score 检测框得分
     * @param cls_id 检测框分类id号
     * @param cls_name 检测框分类名称
     */
    DetectionResult(float x1, float y1, float x2, float y2, float score, int cls_id, std::string cls_name) {
        this->x1 = x1;
        this->y1 = y1;
        this->x2 = x2;
        this->y2 = y2;
        this->score=score;
        this->cls_id = cls_id;
        this->cls_name = cls_name;
    }
    float xmin() { return this->x1; }
    float ymin() { return this->y1; }
    float xmax() { return this->x2; }
    float ymax() { return this->y2; }
    float xcenter() { return (this->x1+this->x2)/2; }
    float ycenter() { return (this->y1+this->y2)/2; }
    float width() { return this->x2-this->x1; }
    float height() { return this->y2-this->y1; }
}DetectionResult;

// 检测API输入结构体
typedef struct FrameInput{
    unsigned char* frame_image_data;            // cv::Mat指针
    int width;                                  // 图像宽度
    int height;                                 // 图像高度
    int channel;                                // 图像通道数
    uint64_t frame_id;                          // 帧号
    void *user_data;                            // 用户数据指针
    int data_format;                            // 数据格式
}FrameInput;

// 检测API检测结果数组结构体
typedef struct FrameDetectionResult{
    std::vector<DetectionResult> detection_results;         // 图像检测结果数组
    void* user_data;                                        // 用户数组指针
    uint64_t frame_id;                                      // 帧号
}FrameDetectionResult;


#endif //DETECTION_CXX_DETECTION_COMMON_H
