/*
 * @Author: error: error: git config user.name & please set dead value or install git && error: git config user.email & please set dead value or install git & please set dead value or install git
 * @Date: 2024-05-14 14:55:26
 * @LastEditors: error: error: git config user.name & please set dead value or install git && error: git config user.email & please set dead value or install git & please set dead value or install git
 * @LastEditTime: 2024-08-29 14:02:02
 * @FilePath: \panoramic_code\my_test.cpp
 * @Description: 这是默认设置,请设置`customMade`, 打开koroFileHeader查看配置 进行设置: https://github.com/OBKoro1/koro1FileHeader/wiki/%E9%85%8D%E7%BD%AE
 */
#include "common.h"
#include "mpu_init.h"
#include "mpu_avl_api.h"
#include "CChannelManager.h"
#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include "sdk_log.h"
#include "CConfig.h"

static void sigstop(int signo)
{
    AVL_Ptz_SetPtzEnable(0, 0);
    exit(1);
}
int main(void)
{
    printf("start\n");
    // signal(SIGINT, sigstop);
    // signal(SIGQUIT, sigstop);
    // signal(SIGTERM, sigstop);


    my_spd_log_init();
    int ret;
    ret = mpu_init();
    while (1)
    {
        sleep(3);
    }
    
    
    return 0;
}