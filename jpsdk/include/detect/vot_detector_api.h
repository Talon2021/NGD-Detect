/*
 **************************************************************************************************************
 * @Time    : 2024/4/18 11:00
 * @Author  : DaiPuWei
 * @Email   : puwei.dai@Vitalchem.com
 * Copyright (c) 2024 Vitalchem All rights reserved.
 **************************************************************************************************************
 */

#ifndef VOTDETECTIONTRACK_VOT_DETECTOR_API_H
#define VOTDETECTIONTRACK_VOT_DETECTOR_API_H

/**
 * 这是同步检测接口的头文件
 */

#include <memory>
#include "vector"
#include "string"
#include "detection_common.h"

namespace VOT{

    /**
     * @brief 这是设置debug级别日志的函数
     */
    EXPORT_API void enable_debug_msg();

    class EXPORT_API VOTDetectorWrapper{
    public:
        /**
         * 这是同步VOT检测器接口的构造函数
         * @param confidence_threshold 置信度阈值
         * @param iou_threshold iou阈值，默认为0.5
         * @param device_id 设备号，默认为0
         * @param export_time 是否输出时间，默认为0
         */
        VOTDetectorWrapper(float confidence_threshold,float iou_threshold=0.5,int device_id=0,int export_time=0);

        /**
         * @brief 这是同步VOT检测器接口的析构函数
         */
        ~VOTDetectorWrapper();

        /**
         * @brief 这是同步VOT检测器接口检测单张图像的函数
         * @param frame_input 视频帧
         * @return 检测帧的检测结果结构体
         */
        FrameDetectionResult detect(FrameInput frame_input);

        /**
         * @brief 这是同步VOT检测器接口检测批量图像的函数
         * @param frame_inputs 视频帧数组
         * @return 检测结果结构体数组
         */
        std::vector<FrameDetectionResult> detect(std::vector<FrameInput> frame_inputs);

        /**
         * @brief 这是同步VOT检测器接口检测视频的函数
         * @param video_path 视频路径
         * @param result_video_path 检测结果视频路径
         * @param interval 抽帧频率，默认为-1，代表逐帧检测,1代表隔秒检测
         */
        void detect(std::string video_path,std::string result_video_path,float interval);

        /**
         * @brief 这是同步VOT检测器接口获取目标名称数组的函数
         * @return 目标名称数组
         */
        std::vector<std::string> get_class_names();

        /**
         * @brief 这是获取模型各个阶段推理速度的函数
         * @return 各个阶段推理时间数组
         */
        std::vector<double> get_model_speed();

    private:
        class Impl;
        std::shared_ptr<Impl> impl_;
    };
} // namespace VOT

#endif //VOTDETECTIONTRACK_VOT_DETECTOR_API_H
