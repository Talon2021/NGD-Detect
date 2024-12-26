/*
 * @Author: error: error: git config user.name & please set dead value or install git && error: git config user.email & please set dead value or install git & please set dead value or install git
 * @Date: 2024-05-20 14:00:43
 * @LastEditors: error: error: git config user.name & please set dead value or install git && error: git config user.email & please set dead value or install git & please set dead value or install git
 * @LastEditTime: 2024-05-23 14:38:42
 * @FilePath: \panoramic_code\src\sdk_log\sdk_log.h
 * @Description: 这是默认设置,请设置`customMade`, 打开koroFileHeader查看配置 进行设置: https://github.com/OBKoro1/koro1FileHeader/wiki/%E9%85%8D%E7%BD%AE
 */
#ifndef _SDK_LOG_H_
#define _SDK_LOG_H_
#pragma once

#define SDK_LOG_USER         "mid_log"
#define SDK_LOG_LOG         "/root/app/ai/logs/"

// #define DEBUG(format, args...) SPDLOG_LOGGER_DEBUG(spdlog::default_logger_raw(), format, ##args);SPDLOG_LOGGER_DEBUG(spdlog::get(SDK_LOG_USER), format, ##args)
// #define INFO(format, args...) SPDLOG_LOGGER_INFO(spdlog::default_logger_raw(),format, ##args);SPDLOG_LOGGER_INFO(spdlog::get(SDK_LOG_USER), format, ##args)
// #define WARN(format, args...) SPDLOG_LOGGER_WARN(spdlog::default_logger_raw(), format, ##args);SPDLOG_LOGGER_WARN(spdlog::get(SDK_LOG_USER), format, ##args)
// #define ERROR(format, args...) SPDLOG_LOGGER_ERROR(spdlog::default_logger_raw(), format, ##args);SPDLOG_LOGGER_ERROR(spdlog::get(SDK_LOG_USER), format, ##args)

#define DEBUG(format, args...) debug_log("[%s:%d]" format, __FILE__, __LINE__, ##args)
#define INFO(format, args...) info_log("[%s:%d]" format, __FILE__, __LINE__, ##args)
#define WARN(format, args...) warn_log("[%s:%d]" format, __FILE__, __LINE__, ##args)
#define ERROR(format, args...) error_log("[%s:%d]" format, __FILE__, __LINE__, ##args)
int my_spd_log_init();
int my_spd_log_deinit();

void debug_log(const char *format, ...);
void info_log(const char *format, ...);
void warn_log(const char *format, ...);
void error_log(const char *format, ...);
#endif