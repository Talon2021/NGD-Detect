/*
 * @Author: error: error: git config user.name & please set dead value or install git && error: git config user.email & please set dead value or install git & please set dead value or install git
 * @Date: 2024-05-13 15:58:20
 * @LastEditors: error: error: git config user.name & please set dead value or install git && error: git config user.email & please set dead value or install git & please set dead value or install git
 * @LastEditTime: 2024-05-21 11:17:04
 * @FilePath: \panoramic_code\src\include\avl\CCGpsPosition.h
 * @Description: 这是默认设置,请设置`customMade`, 打开koroFileHeader查看配置 进行设置: https://github.com/OBKoro1/koro1FileHeader/wiki/%E9%85%8D%E7%BD%AE
 */
#ifndef __CCGPSPOSITION_H_
#define __CCGPSPOSITION_H_

#include "Cserial.h"
#include "common.h"
#include  <pthread.h>

typedef struct GPS_GNGG_DATA
{
    int vaild;
    double longitude;
    char lon_dir;
    double latitude;
    char lat_dir;
    int satellite_num;
    int azimuth;
}gps_gngga_data;

typedef struct GPS_DATA
{
    gps_gngga_data gga_data;
    int signal_strength;    //0 1 2
    
    
}g_data;

class CCGpsPosition
{
private:
   
    int m_init;
    pthread_t m_pid;
    
    g_data m_data;
    void *m_han;
public:
    Cserial *m_serial;
    int m_gpsThreadExit;
    int m_timeZone;
    pthread_mutex_t m_lock;
    CCGpsPosition(void *handle, int ch);
    ~CCGpsPosition();
    int Init();
    int UnInit();
    int SetTimeZone(int timeZone);
    int GetGpsdata(g_data *value);
    int SetGpsdata(g_data value);
};


#endif