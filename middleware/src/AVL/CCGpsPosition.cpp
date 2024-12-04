/*
 * @Author: error: error: git config user.name & please set dead value or install git && error: git config user.email & please set dead value or install git & please set dead value or install git
 * @Date: 2024-05-13 15:58:04
 * @LastEditors: error: error: git config user.name & please set dead value or install git && error: git config user.email & please set dead value or install git & please set dead value or install git
 * @LastEditTime: 2024-06-20 15:19:49
 * @FilePath: \panoramic_code\src\AVL\CCGpsPosition.cpp
 * @Description: 这是默认设置,请设置`customMade`, 打开koroFileHeader查看配置 进行设置: https://github.com/OBKoro1/koro1FileHeader/wiki/%E9%85%8D%E7%BD%AE
 */
#include "CCGpsPosition.h"

#include "sdk_log.h"

#include <string>
#include <sstream>
#include <vector>
#include "Cserial.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include "CConfig.h"
#include "common.h"
#include "sdk_log.h"

#define G_UART_DEV3 "/dev/ttyS3"
#define GPS_BUFFER_LEN  2048
#define UPDATE_TIME     ((5*3600)*5)

typedef struct GNRMCData {
    std::string time;
    char status;
    double latitude;
    char lat_dir;
    double longitude;
    char lon_dir;
    double speed;
    double course;
    std::string date;
    double magnetic_variation;
    char mag_var_dir;
}gngmc_d;

typedef struct GNGGAData {
    std::string time;
    double latitude;
    char lat_dir;
    double longitude;
    char lon_dir;
    int quality;
    int num_satellites;
    double hdop;
    double altitude;
    char altitude_units;
    double geoidal_separation;
    char geoidal_units;
    double age_of_diff_corr;
    std::string diff_ref_station_id;
}gngga_d;

static std::vector<std::string>split(const std::string& s, char delimiter) {
    std::vector<std::string> tokens;
    std::string token;
    std::istringstream tokenStream(s);
    while (std::getline(tokenStream, token, delimiter)) {
        tokens.push_back(token);
    }
    return tokens;
}

static bool parseGNRMC(const std::string& sentence, gngmc_d& data)
{
    auto fields = split(sentence, ',');

    if (fields.size() < 12 || fields[0].substr(3) != "RMC") {
        return false;
    }
    data.time = fields[1];  // UTC时间
    data.status = fields[2][0];  // 状态
    if (data.status != 'A') {
        return false;  // 定位无效
    }
    data.latitude = std::stod(fields[3]);  // 纬度
    data.lat_dir = fields[4][0];  // 纬度方向
    data.longitude = std::stod(fields[5]);  // 经度
    data.lon_dir = fields[6][0];  // 经度方向
    data.speed = std::stod(fields[7]);  // 地面速率
    data.course = std::stod(fields[8]);  // 航向
    data.date = fields[9];  // 日期
    data.magnetic_variation = fields[10].empty() ? 0.0 : std::stod(fields[10]);  // 磁偏角
    data.mag_var_dir = fields[11].empty() ? ' ' : fields[11][0];  // 磁偏角方向

    return true;
}


static bool parseGNGGA(const std::string& sentence, gngga_d& data)
{
    auto fields = split(sentence, ',');

    if (fields.size() < 15 || fields[0].substr(3) != "GGA") {
        return false;
    }

    data.time = fields[1];  // UTC时间
    data.latitude = std::stod(fields[2]);  // 纬度
    data.lat_dir = fields[3][0];  // 纬度方向
    data.longitude = std::stod(fields[4]);  // 经度
    data.lon_dir = fields[5][0];  // 经度方向
    data.quality = std::stoi(fields[6]);  // GPS质量指示
     if (data.quality == 0) {
        return false;  // 定位无效
    }
    data.num_satellites = std::stoi(fields[7]);  // 跟踪卫星数量
    data.hdop = std::stod(fields[8]);  // 水平精度因子
    data.altitude = std::stod(fields[9]);  // 海拔高度
    data.altitude_units = fields[10][0];  // 海拔高度单位
    data.geoidal_separation = std::stod(fields[11]);  // 大地水准面高度
    data.geoidal_units = fields[12][0];  // 大地水准面高度单位
    data.age_of_diff_corr = fields[13].empty() ? 0.0 : std::stod(fields[13]);  // 差分修正时间
    data.diff_ref_station_id = fields[14];  // 差分参考基站ID

    return true;
}


static void *GpsThread(void *arg)
{
    int ret;
    static int last_timeZone = 0;
    int update_time = UPDATE_TIME;
    char buf[GPS_BUFFER_LEN] = {0};
    int get_gpsfail_conunt = 0;
    g_data tmpdata = {0};
    CCGpsPosition *handle = (CCGpsPosition *)arg;
    while (!handle->m_gpsThreadExit)
    {
        memset(&tmpdata, 0, sizeof(g_data));
        memset(buf, 0, GPS_BUFFER_LEN);
        ret = handle->m_serial->UartRead(buf, GPS_BUFFER_LEN, 100);
        if(ret > 0)
        {
            get_gpsfail_conunt = 0;
            gngga_d gga_data;
            gngmc_d gmc_data;
            //std::string gps_data(buf);
            auto lines = split(buf, '\n');
            for(const auto& line : lines)
            {
                if (line.find("$GNRMC") != std::string::npos) {
                    if(parseGNRMC(line, gmc_data) == true)
                    {
                        if(last_timeZone != handle->m_timeZone || update_time < 0)
                        {
                            char formattedDateTime[64] = {0};
                            snprintf(formattedDateTime, sizeof(formattedDateTime), "date -s %04d-%02d-%02dT%02d:%02d:%02d",\
                                                                            (2000 + std::stoi(gmc_data.date.substr(4, 2))), std::stoi(gmc_data.date.substr(2, 2)), \
                                                                            std::stoi(gmc_data.date.substr(0, 2)), (std::stoi(gmc_data.time.substr(0, 2)) + handle->m_timeZone), \
                                                                            std::stoi(gmc_data.time.substr(2, 2)), std::stoi(gmc_data.time.substr(4, 2)));
                            mysystem(formattedDateTime);
                            update_time = UPDATE_TIME;
                           
                        }
                    }
                    
                }
                else if (line.find("$GNGGA") != std::string::npos) {
                    if(parseGNGGA(line, gga_data) == true)
                    {
                        tmpdata.gga_data.vaild = 1;
                        tmpdata.gga_data.longitude = gga_data.longitude;
                        tmpdata.gga_data.lon_dir = gga_data.lon_dir;
                        tmpdata.gga_data.latitude = gga_data.latitude;
                        tmpdata.gga_data.lat_dir = gga_data.lat_dir;
                        tmpdata.gga_data.satellite_num = gga_data.num_satellites;
                    }
                }
            }
            if(gmc_data.status == 'A')
                tmpdata.signal_strength = 1;
            if(gga_data.quality > 0)
                tmpdata.signal_strength = 2;

            handle->SetGpsdata(tmpdata);
        }
        else
        {
            get_gpsfail_conunt++;
            if(get_gpsfail_conunt > 50)
            {
                handle->SetGpsdata(tmpdata);
            }
        }
        update_time--;
        usleep(200 *1000);
    }
    return NULL;
}

CCGpsPosition::CCGpsPosition(void *handle, int ch)
{
    m_init = 0;
    m_serial = NULL;
    m_pid = 0;
    m_gpsThreadExit = 0;
    m_timeZone = 0;
    memset(&m_data, 0, sizeof(g_data));
    m_han = handle;
}

CCGpsPosition::~CCGpsPosition()
{
    UnInit();
}

int CCGpsPosition::Init()
{
    if(m_init == 1)
    {
        return -1;
    }
    int ret;
    pthread_mutex_init(&m_lock,NULL);
    m_gpsThreadExit = 0;
    m_serial = new Cserial(G_UART_DEV3, 115200);
    m_serial->init();
    m_timeZone = 8;
    ret = pthread_create(&(m_pid), NULL, GpsThread, (void *)this);
    if(ret < 0 )
    {
        ERROR("creat thread Gps is err\n");
    }
    m_init = 1;

    return 0;
}

int CCGpsPosition::UnInit()
{
    m_gpsThreadExit = 1;

    pthread_cancel(m_pid);
    pthread_join(m_pid, NULL);
    m_pid = 0;
    pthread_mutex_destroy(&m_lock);
    if(m_serial)
    {
        delete m_serial;
        m_serial = NULL;
    }
    m_init = 0;
    return 0;
}

int CCGpsPosition::SetTimeZone(int timeZone)
{
    pthread_mutex_lock(&m_lock);
    m_timeZone = timeZone;
    pthread_mutex_unlock(&m_lock);
    return 0;
}

int CCGpsPosition::GetGpsdata(g_data *value)
{
    if(value == NULL)
    {
        ERROR("prame is NULL\n");
        return -1;
    }
    pthread_mutex_lock(&m_lock);
    if( m_data.gga_data.vaild ==  0)
    {
        value->signal_strength = m_data.signal_strength;
        pthread_mutex_unlock(&m_lock);
        return 0;
    }
    memcpy(value,&m_data, sizeof(g_data));
    m_data.gga_data.vaild = 0;
    pthread_mutex_unlock(&m_lock);
    return 0;
}

int CCGpsPosition::SetGpsdata(g_data value)
{
    pthread_mutex_lock(&m_lock);
    memcpy(&m_data,&value, sizeof(g_data));
    pthread_mutex_unlock(&m_lock);
    return 0;
}
