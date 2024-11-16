#include "Cserial.h"
#include <stdio.h>
#include <string.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include <stdlib.h>
#include <errno.h>
#include <termios.h>
#include <sys/time.h>
#include <pthread.h>
#include <sys/prctl.h>
#include "common.h"
#include "sdk_log.h"
Cserial::Cserial(const char *path, unsigned long Baudrate)
{
    memset(m_file_path,0,sizeof(m_file_path));
    memcpy(m_file_path, path, strlen(path));
    m_fd = 0;
    m_Baudrate = Baudrate;
    pthread_mutex_init(&m_lock, NULL);
}

Cserial::~Cserial()
{
    if(m_fd > 0){
        close(m_fd);
        m_fd = 0;
    }
    memset(m_file_path,0,sizeof(m_file_path));
    pthread_mutex_destroy(&m_lock);
    
}

int Cserial::init()
{
    int ret = 0;
    struct termios oldtio;
    struct termios opt;
    int dwSpeed = B115200;
    if(access(m_file_path,0)){
        ERROR("init is err\n");
        return -1;
    }
    m_fd = open(m_file_path, O_RDWR | O_NOCTTY | O_NONBLOCK);
    if(m_fd < 0){
        ERROR("open uart dev fail\n");
        ret = -1;
        goto EXIT;
    }
    if (tcgetattr(m_fd, &oldtio) != 0){
        ERROR("tcgetattr uart dev fail\n");
        ret = -1;
        goto EXIT;
    }
    bzero(&opt, sizeof(opt));
    opt.c_cflag |= CLOCAL; // 忽略 modem 控制线。
    opt.c_cflag |= CREAD;  // 打开接受者。
    opt.c_cflag &= ~CSIZE;
    opt.c_cflag |= CS8; // 传送或接收字元时用8bits
    opt.c_cflag &= ~PARENB;

   
    dwSpeed = ConvBaudrate(m_Baudrate);

    cfsetispeed(&opt, dwSpeed);
    cfsetospeed(&opt, dwSpeed);
    opt.c_cflag &= ~CSTOPB;
    tcflush(m_fd, TCIOFLUSH);
    if ((tcsetattr(m_fd, TCSANOW, &opt)) != 0){
        fprintf(stderr,"tcsetattr uart dev fail\n");
        ret = -1;
        goto EXIT;
    }
    return ret;
EXIT:
    if(m_fd > 0){
        close(m_fd);
        m_fd = 0;
    }
    return ret;
}

int Cserial::ConvBaudrate(unsigned long dwBaudrate)
{
    switch (dwBaudrate)
	{
		case 1200:
			return B1200;
			break;
		case 2400:
			return B2400;
			break;
		case 4800:
			return B4800;
			break;
		case 9600:
			return B9600;
			break;
		case 19200:
			return B19200;
			break;
		case 38400:
			return B38400;
			break;
		case 57600:
			return B57600;
			break;
		case 115200:
			return B115200;
			break;
		default:
			return B115200;
			break;
	}
}

int Cserial::UartRead(char *buf, int size, unsigned long time_millisecond)
{
    
    int ret;
    int maxfd = m_fd;
    struct timeval TimeoutVal;
    TimeoutVal.tv_sec = 0;
    TimeoutVal.tv_usec = time_millisecond * 1000;
    fd_set read_fds;

    FD_ZERO(&read_fds);
    FD_SET(maxfd, &read_fds);

    ret = select(maxfd + 1, &read_fds, NULL, NULL, &TimeoutVal);
    if(ret < 0){
        perror("select fail\n");
        return -1;
    }
    else if(ret == 0){
        return 0;
    }
    if(FD_ISSET(m_fd, &read_fds)){
        pthread_mutex_lock(&m_lock);
        ret = read(m_fd, buf, size);
        // printf("read code:");
        // for(int i = 0 ; i < size; i++)
        // {
        //     printf("0x%x ",buf[i]);
        // }
        // printf("\n");
        pthread_mutex_unlock(&m_lock);
    }
    return ret;
}

int Cserial::UartRead(unsigned char *buf, int size, unsigned long time_millisecond)
{
    
    int ret;
    int maxfd = m_fd;
    struct timeval TimeoutVal;
    TimeoutVal.tv_sec = 0;
    TimeoutVal.tv_usec = time_millisecond * 1000;
    fd_set read_fds;

    FD_ZERO(&read_fds);
    FD_SET(maxfd, &read_fds);

    ret = select(maxfd + 1, &read_fds, NULL, NULL, &TimeoutVal);
    if(ret < 0){
        perror("select fail\n");
        return -1;
    }
    else if(ret == 0){
        return 0;
    }
    if(FD_ISSET(m_fd, &read_fds)){
        pthread_mutex_lock(&m_lock);
        ret = read(m_fd, buf, size);
        // printf("read code:");
        // for(int i = 0 ; i < size; i++)
        // {
        //     printf("0x%x ",buf[i]);
        // }
        // printf("\n");
        pthread_mutex_unlock(&m_lock);
    }
    return ret;
}

int Cserial::UartWrite(const char *buf, int size)
{
    int ret;
    pthread_mutex_lock(&m_lock);
    ret = write(m_fd, buf, size);
    // printf("write code:");
    // for(int i = 0 ; i < size; i++)
    // {
    //     printf("0x%x ",buf[i]);
    // }
    pthread_mutex_unlock(&m_lock);
    if(ret < 0){
        ERROR("write is err ret = %d\n",ret);
        return -1;
    }
    return 0;
}

int Cserial::UartWrite(const unsigned char *buf, int size)
{
    int ret;
    pthread_mutex_lock(&m_lock);
    ret = write(m_fd, buf, size);
    //printf("write code:");
    // for(int i = 0 ; i < size; i++)
    // {
    //     printf("0x%x ",buf[i]);
    // }
    // printf("\n");
    pthread_mutex_unlock(&m_lock);
    if(ret < 0){
        ERROR("write is err ret = %d\n",ret);
        return -1;
    }
    return 0;
}