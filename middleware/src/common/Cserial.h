/*
 * @Author: error: error: git config user.name & please set dead value or install git && error: git config user.email & please set dead value or install git & please set dead value or install git
 * @Date: 2024-05-14 09:46:30
 * @LastEditors: error: error: git config user.name & please set dead value or install git && error: git config user.email & please set dead value or install git & please set dead value or install git
 * @LastEditTime: 2024-06-28 17:24:43
 * @FilePath: \panoramic_code\src\uart\Cserial.h
 * @Description: 这是默认设置,请设置`customMade`, 打开koroFileHeader查看配置 进行设置: https://github.com/OBKoro1/koro1FileHeader/wiki/%E9%85%8D%E7%BD%AE
 */
#ifndef __CSERIAL_H_
#define __CSERIAL_H_


#include "common.h"
#include <pthread.h>

class Cserial
{
    private:
    char m_file_path[256];
    int m_fd;
    unsigned long m_Baudrate;
    pthread_mutex_t m_lock;
    public:
    Cserial(const char *path, unsigned long Baudrate);
    ~Cserial();
    int init();
    int UartRead(char *buf, int size, unsigned long time_millisecond);
    int UartRead(unsigned char *buf, int size, unsigned long time_millisecond);
    int UartWrite(const char *buf, int size);
    int UartWrite(const unsigned char *buf, int size);
    int ConvBaudrate(unsigned long dwBaudrate);
};


#endif