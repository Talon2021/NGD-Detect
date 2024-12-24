/*
 * @Author: error: error: git config user.name & please set dead value or install git && error: git config user.email & please set dead value or install git & please set dead value or install git
 * @Date: 2024-07-03 11:35:34
 * @LastEditors: error: error: git config user.name & please set dead value or install git && error: git config user.email & please set dead value or install git & please set dead value or install git
 * @LastEditTime: 2024-08-08 15:38:23
 * @FilePath: \Car_ai\jpsdk\src\common\meida_common.cpp
 * @Description: 这是默认设置,请设置`customMade`, 打开koroFileHeader查看配置 进行设置: https://github.com/OBKoro1/koro1FileHeader/wiki/%E9%85%8D%E7%BD%AE
 */
#include "meida_common.h"
#include <pthread.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/time.h>
#include <semaphore.h>
#include <dlfcn.h>
/* 获取系统启动的毫秒数 */

/* SDK接口资源 */
typedef struct
{
    HY_RWLodk_t m_Lock; //SDK出口和其它接口互斥
    int m_bSDKInit; //SDK是否初始化
    BOOL m_bSDKInitLock; //SDK入口与出口函数互斥
    enSdkMode m_enSdkModeCur; //SDK当前模式
} Local_SDK_ResInfo_t;


void *g_pSdkLogHandler = NULL;
log_param_t g_log_param = {
    LOG_DEV_FILE, LOG_PREFIX_STR, LOG_SHOW_LEVEL, LOG_PRT_ENABLE, 10000,
    LOG_MAX_CHAR_COUNT, LOG_FLUSH_TIME_SEC, LOG_IO_BUGGER_SIZE
};

static Local_SDK_ResInfo_t g_SDKResInfo = { 0 };
uint64_t GetSystemBootDuratingMSecond(void)
{
    struct timespec curTime = {0};
    clock_gettime(CLOCK_MONOTONIC, &curTime);
    return ((uint64_t)curTime.tv_sec * 1000 + curTime.tv_nsec / 1000000);
}

int mysystem(const char *cmdstring)
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

void HY_Res_SDK_Usleep(unsigned int ntime)
{
    usleep(ntime);
}

void HY_RES_SDK_init()
{
	pthread_mutex_init(&g_SDKResInfo.m_Lock.m_Lock, NULL);
}

int HY_Res_SDK_Lock()
{
    // if (g_SDKResInfo.m_bSDKInit != SDK_INIT_MAGIC)
    // return -1;
    // if (0 == g_sysinfo.status)
    //     return -1;

    while (1)
    {
        pthread_mutex_lock(&g_SDKResInfo.m_Lock.m_Lock);

        /* 有写锁需求尚未满足或者正在写锁状态，不允许加锁 */
        if ((g_SDKResInfo.m_Lock.m_WriteLockNum > 0)
            || (g_SDKResInfo.m_Lock.m_LockState == HY_WRLOCK))
        {
            //SDK_DBG("[SDK] Zview_RD_Lock wait: WLock = %d, RLock = %d, LockState = %d\n", g_SDKResInfo.m_Lock.m_WriteLockNum, g_SDKResInfo.m_Lock.m_ReadLockNum, g_SDKResInfo.m_Lock.m_LockState);
            pthread_mutex_unlock(&g_SDKResInfo.m_Lock.m_Lock);
            HY_Res_SDK_Usleep(80000);
            continue;
        }

        g_SDKResInfo.m_Lock.m_LockState = HY_RDLOCK;
        g_SDKResInfo.m_Lock.m_ReadLockNum++;
        pthread_mutex_unlock(&g_SDKResInfo.m_Lock.m_Lock);
        break;
    }
    return 0;
}

int HY_Res_SDK_UnLock()
{
    pthread_mutex_lock(&g_SDKResInfo.m_Lock.m_Lock);
    if (g_SDKResInfo.m_Lock.m_ReadLockNum > 0)
        g_SDKResInfo.m_Lock.m_ReadLockNum--;
    else
        //SDK_DBG("[SDK] HY_RWLodk_t RDLockNum Err\n");
    if (g_SDKResInfo.m_Lock.m_ReadLockNum == 0)
        g_SDKResInfo.m_Lock.m_LockState = HY_UNLOCK;
    pthread_mutex_unlock(&g_SDKResInfo.m_Lock.m_Lock);
    return 0;
}


int Comm_CreateThread(ThreadInfo_t* pThInfo, int Pri, void* (*start_routine)(void*))
{
    pthread_attr_t attr = { 0 };
    struct sched_param param = { 0 };
    int RetValue = -1;

    if (Pri == 0)
    {
        pthread_attr_init(&attr);
        pthread_attr_setschedpolicy(&attr, SCHED_OTHER);
        pthread_attr_getschedparam(&attr, &param);
        param.sched_priority = 0;
        pthread_attr_setschedparam(&attr, &param);
        RetValue = pthread_create(&pThInfo->m_ThreadId, &attr, start_routine, pThInfo);
        pthread_attr_destroy(&attr);
    }
    else
    {
        pthread_attr_init(&attr);
        pthread_attr_setschedpolicy(&attr, SCHED_RR);
        pthread_attr_getschedparam(&attr, &param);
        param.sched_priority = Pri;
        pthread_attr_setschedparam(&attr, &param);
        RetValue = pthread_create(&pThInfo->m_ThreadId, &attr, start_routine, pThInfo);
        pthread_attr_destroy(&attr);
    }

    if (RetValue != 0)
    {
        pThInfo->m_ThreadId = 0;
    }

    pThInfo->m_Exit = 0;
    return RetValue;
}

int Comm_DestroyThread(ThreadInfo_t* pThInfo)
{
    if (pThInfo->m_ThreadId != 0)
    {
        pThInfo->m_Exit = 1;
        pthread_join(pThInfo->m_ThreadId, NULL);
        pThInfo->m_ThreadId = 0;
    }
    return 0;
}
