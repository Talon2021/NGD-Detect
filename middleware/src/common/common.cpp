#include "common.h"


#include <errno.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <libgen.h>
#include <semaphore.h>
#include <unistd.h>
#include <sys/time.h>
#include <sys/select.h>
#include <sys/types.h> 
#include <sys/wait.h>
#include <sys/ioctl.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <net/if.h>
#include <arpa/inet.h>
#include <ctype.h>
#include "sdk_log.h"

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


int get_bit(int *data, int index) {
    return (*data & (0x01 << index)) != 0 ? 1 : 0;
}

void set_bit(int *data, int index, bool ch)
{
    if(ch)
    {
        *data |=(0x01<<(index));
    }
    else
    {
        *data &=~(0x01<<(index));
    }
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

unsigned long long get_ms()
{
    struct timeval time_v = { 0 };
    gettimeofday(&time_v, NULL);
    return (unsigned long long)time_v.tv_sec * 1000 + time_v.tv_usec / 1000;   
}

int get_localip(const char * eth_name, char *local_ip_addr)
{
	int ret = -1;
    register int fd;
    struct ifreq ifr;
 
	if (local_ip_addr == NULL || eth_name == NULL)
	{
		return ret;
	}
	if ((fd=socket(AF_INET, SOCK_DGRAM, 0)) > 0)
	{
		strcpy(ifr.ifr_name, eth_name);
		if (!(ioctl(fd, SIOCGIFADDR, &ifr)))
		{
			ret = 0;
			strcpy(local_ip_addr, inet_ntoa(((struct sockaddr_in *)&ifr.ifr_addr)->sin_addr));
		}
	}
	if (fd > 0)
	{
		close(fd);
	}
    return ret;
}

int get_localmac(const char * eth_name, char *mac)
{
	int ret = -1;
    register int fd;
    struct ifreq ifr;
 
	if (mac == NULL || eth_name == NULL)
	{
		return ret;
	}
	if ((fd=socket(AF_INET, SOCK_STREAM, 0)) > 0)
	{
		strcpy(ifr.ifr_name, eth_name);
		if (!(ioctl(fd, SIOCGIFADDR, &ifr)))
		{
			ret = 0;
            snprintf(mac, 18, "%02x:%02x:%02x:%02x:%02x:%02x",
                                (unsigned char)ifr.ifr_hwaddr.sa_data[0],
                                (unsigned char)ifr.ifr_hwaddr.sa_data[1],
                                (unsigned char)ifr.ifr_hwaddr.sa_data[2],
                                (unsigned char)ifr.ifr_hwaddr.sa_data[3],
                                (unsigned char)ifr.ifr_hwaddr.sa_data[4],
                                (unsigned char)ifr.ifr_hwaddr.sa_data[5]);
		}
	}
	if (fd > 0)
	{
		close(fd);
	}
    return ret;
}

int get_localmask(const char * eth_name, char *msak)
{
	int ret = -1;
    register int fd;
    struct ifreq ifr;
 
	if (msak == NULL || eth_name == NULL)
	{
		return ret;
	}
	if ((fd=socket(AF_INET, SOCK_DGRAM, 0)) > 0)
	{
		strcpy(ifr.ifr_name, eth_name);
		if (!(ioctl(fd, SIOCGIFNETMASK, &ifr)))
		{
			ret = 0;
            strcpy(msak, inet_ntoa(((struct sockaddr_in *)&ifr.ifr_addr)->sin_addr));
		}
	}

	if (fd > 0)
	{
		close(fd);
	}
    return ret;
}

int get_localgetway(const char * eth_name, char *netway)
{
	 FILE *fp;  
    char buf[512];  
    char cmd[128];   
    char *tmp;  
  
    strcpy(cmd, "ip route");  
    fp = popen(cmd, "r");  
    if(NULL == fp)  
    {  
        perror("popen error");  
        return false;  
    }  
    while(fgets(buf, sizeof(buf), fp) != NULL)  
    {  
        tmp =buf;  
        while(*tmp && isspace(*tmp))  
            ++tmp;  
        if(strncmp(tmp, "default", strlen("default")) == 0)  
            break;  
    }  
    sscanf(buf, "%*s%*s%s", netway);          
    pclose(fp);  

}