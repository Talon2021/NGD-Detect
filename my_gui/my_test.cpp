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
#include "MessageManager.h"

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
    MessageManager *msg = MessageManager::getInstance();
    while (1)
    {
        sleep(10);
    }
    
    
    return 0;
}