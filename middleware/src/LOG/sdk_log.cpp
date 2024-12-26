/*
 * @Author: error: error: git config user.name & please set dead value or install git && error: git config user.email & please set dead value or install git & please set dead value or install git
 * @Date: 2024-03-25 16:22:40
 * @LastEditors: error: error: git config user.name & please set dead value or install git && error: git config user.email & please set dead value or install git & please set dead value or install git
 * @LastEditTime: 2024-08-19 14:58:45
 * @FilePath: \src\log\my_spd_log.cpp
 * @Description: 这是默认设置,请设置`customMade`, 打开koroFileHeader查看配置 进行设置: https://github.com/OBKoro1/koro1FileHeader/wiki/%E9%85%8D%E7%BD%AE
 */
#include<stdio.h>
#include "sdk_log.h"
#include <spdlog/spdlog.h>
#include <spdlog/async_logger.h>
#include <spdlog/sinks/stdout_color_sinks.h>
#include <spdlog/sinks/basic_file_sink.h>
#include <spdlog/sinks/rotating_file_sink.h>
#include <spdlog/details/thread_pool.h>
#include <spdlog/details/thread_pool-inl.h>
#include <spdlog/sinks/daily_file_sink.h>
#include <spdlog/async.h>
#include <sys/types.h>
#include <unistd.h>
#include <sys/wait.h>
#include <stdarg.h>

#define LOG_LEVEL_TRACE spdlog::level::trace
#define LOG_LEVEL_DEBUG spdlog::level::debug
#define LOG_LEVEL_INFO spdlog::level::info
#define LOG_LEVEL_WARN spdlog::level::warn
#define LOG_LEVEL_ERROR spdlog::level::err
#define LOG_LEVEL_CRITICAL spdlog::level::critical
#define LOG_LEVEL_OFF spdlog::level::off
#define max_file_size       5 * ((1) << (20))
#define max_file_count          (1)

static int mysystem(const char *cmdstring)
{
	pid_t pid;

	int status;

	if (cmdstring == NULL)
		return 1;
	if ((pid = vfork()) < 0)
		status = -1;
	else if (pid == 0)
	{
		execl("/bin/sh", "sh", "-c", cmdstring, (char *) 0);
		_exit(127);
	}
	else
	{
		while (waitpid(pid, &status, 0) < 0)
			if (errno != EINTR)
			{
				status = -1;
				break;
			}
	}
	return status;
}

int my_spd_log_init()
{
    char buf[256]={0};
	sprintf(buf,"mkdir -pm 777 %s",SDK_LOG_LOG);
    if(access(SDK_LOG_LOG,0))
    {
        mysystem(buf);
		
    }
	sprintf(buf,"%s/sdk.log",SDK_LOG_LOG);
    spdlog::init_thread_pool(8192, 1);
    auto rotating_sink = std::make_shared<spdlog::sinks::rotating_file_sink_mt>(buf, max_file_size, max_file_count);
	auto console_sink = std::make_shared<spdlog::sinks::stdout_color_sink_mt>();
	console_sink->set_level(LOG_LEVEL_DEBUG);
    std::vector<spdlog::sink_ptr> sinks {rotating_sink, console_sink};

    auto logger = std::make_shared<spdlog::async_logger>(SDK_LOG_USER, sinks.begin(), sinks.end(), spdlog::thread_pool(), spdlog::async_overflow_policy::block);
    spdlog::register_logger(logger);
    logger->set_level(LOG_LEVEL_DEBUG);
    logger->set_pattern("[%Y-%m-%d %H:%M:%S] [%l] %v");
	logger->flush_on(LOG_LEVEL_DEBUG);
    spdlog::flush_every(std::chrono::seconds(1));
    return 0;
 }

 int my_spd_log_deinit()
 {
    spdlog::drop_all();
    return 0;
 }

 void debug_log(const char *format, ...)
 {
	va_list args;
    va_start(args, format);

	char buffer[1024];
	vsnprintf(buffer, sizeof(buffer), format, args);

	//spdlog::default_logger_raw()->debug(buffer);

	auto logger = spdlog::get(SDK_LOG_USER);
    if (logger) {
        logger->debug(buffer);
    }
	va_end(args);
 }

 void info_log(const char *format, ...)
 {
	va_list args;
    va_start(args, format);

	char buffer[1024];
	vsnprintf(buffer, sizeof(buffer), format, args);
	//spdlog::default_logger_raw()->info(buffer);

	auto logger = spdlog::get(SDK_LOG_USER);
    if (logger) {
        logger->info(buffer);
		logger->flush();
    }
	va_end(args);
 }

 void warn_log(const char *format, ...)
 {
	va_list args;
    va_start(args, format);

	char buffer[1024];
	vsnprintf(buffer, sizeof(buffer), format, args);
	//spdlog::default_logger_raw()->warn(buffer);

	auto logger = spdlog::get(SDK_LOG_USER);
    if (logger) {
        logger->warn(buffer);
    }
	va_end(args);
 }

 void error_log(const char *format, ...)
 {
	va_list args;
    va_start(args, format);

	char buffer[1024];
	vsnprintf(buffer, sizeof(buffer), format, args);
	//spdlog::default_logger_raw()->error(buffer);

	auto logger = spdlog::get(SDK_LOG_USER);
    if (logger) {
        logger->error(buffer);
    }
	va_end(args);
 }