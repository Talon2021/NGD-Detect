/*
 * @Author: error: error: git config user.name & please set dead value or install git && error: git config user.email & please set dead value or install git & please set dead value or install git
 * @Date: 2024-05-14 17:31:15
 * @LastEditors: error: error: git config user.name & please set dead value or install git && error: git config user.email & please set dead value or install git & please set dead value or install git
 * @LastEditTime: 2024-07-16 16:23:14
 * @FilePath: \panoramic_code\src\AVL\CCAreaInvasion.cpp
 * @Description: 这是默认设置,请设置`customMade`, 打开koroFileHeader查看配置 进行设置: https://github.com/OBKoro1/koro1FileHeader/wiki/%E9%85%8D%E7%BD%AE
 */
#include "CCAreaInvasion.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include "CConfig.h"
#include "common.h"
#include "sdk_log.h"
#include "connect.h"
CCAreaInvasion::CCAreaInvasion(void *handle, int ch)
{
    memset(&m_area, 0, sizeof(area_information) * AREA_COUNT_MAX);
    m_area_count = 0;
    m_really_count = 0;
    m_init = 0;
    m_han = handle;
}

CCAreaInvasion::~CCAreaInvasion()
{
    UnInit();
}

int CCAreaInvasion::Init()
{
    if(m_init == 1)
        return -1;
    int i = 0;
    int j = 0;
    int ret = 0;
    int enable;
    char buffer[AREA_NAME_BUFF_MAX] = {0};
    char key[64] = {0}; 
    AlgorithmSwitch alg_switch;
    memset(&alg_switch,0,sizeof(AlgorithmSwitch));
    std::vector<std::vector<AreaPoint>> polygon;

    m_cconfig = CConfig::GetInstance();
    m_area_count = m_cconfig->GetValue(AREA_ALL_AREA,"all_count",(long)0);
    for( i = 0; i < m_area_count; i++)
    {
        std::vector<AreaPoint> tmp;
        AreaPoint tmp_point;
        snprintf(buffer, sizeof(buffer), "%s_%02d", AREA_SECTION, i);
        m_area[i].point_num = m_cconfig->GetValue(buffer,"area_num",(long)0);
        for(j = 0; j < m_area[i].point_num;j++)
        {
            snprintf(key, sizeof(key), "areaX_%02d", j);
            m_area[i].point[j].x = m_cconfig->GetValue(buffer,key,(long)0);
            snprintf(key, sizeof(key), "areaY_%02d", j);
            m_area[i].point[j].y = m_cconfig->GetValue(buffer,key,(long)0);

            tmp_point.x = m_area[i].point[j].x;
            tmp_point.y = m_area[i].point[j].y;
            tmp.emplace_back(tmp_point);
        }
        m_cconfig->GetValue(buffer, "area_name", NULL, m_area[i].area_name,AREA_NAME_BUFF_MAX);
        m_area[i].enable = m_cconfig->GetValue(buffer,"area_enable",(long)0);
        if(m_area[i].enable > 0)
        {
            m_really_count++;
            polygon.emplace_back(tmp);
        }
    }
    m_cconfig->SetValue(AREA_ALL_AREA,"really_count",(long)(m_really_count));
    m_cconfig->SetValue(AREA_ALL_AREA,"all_count",(long)(m_area_count));
#ifdef ALG_SDK
    ret = SetMonitorArea(m_han, polygon);
    if(ret != 0)
    {
        ERROR("Set algarea is err\n");
        return -1;
    }
#endif
    m_alg_enable = m_cconfig->GetValue(ALG_SECTION,"alg_enable",(long)0);
    if(get_bit(&m_alg_enable,0))
    {
        alg_switch.TargetDetectionSwitch = true;
    }
    if(get_bit(&m_alg_enable,1))
    {
        alg_switch.TargetTrackSwitch = true;
    }
    if(get_bit(&m_alg_enable,2))
    {
        alg_switch.ActivityAnalysisSwitch = true;
    }
#ifdef ALG_SDK
    ret = SetAlgorithmSwitch(m_han, alg_switch);
    if(ret != 0)
    {
        ERROR("Set alg is err\n");
        return -1;
    }
#endif
    m_init = 1;
    enable =  m_cconfig->GetValue(ALG_SECTION,"gas_detect_enable",(long)(1));
    SetDetectGasEnable(enable);

    return 0;
}

int CCAreaInvasion::UnInit()
{
    memset(&m_area, 0, sizeof(area_information) * AREA_COUNT_MAX);
    m_area_count = 0;
    m_really_count = 0;
    m_init = 0;
    return 0;
}

int CCAreaInvasion::SetAreaPoint(area_information area)
{
    if(!m_init)
    {
        return -1;
    }
    int i = 0,j = 0;
    char buffer[AREA_NAME_BUFF_MAX] = {0};
    char key[64] = {0};
    for( i = 0; i < m_area_count; i++)
    {
        if(strcmp(m_area[i].area_name,area.area_name) == 0)
        {
            snprintf(buffer, sizeof(buffer), "%s_%02d", AREA_SECTION, i);

            if(m_cconfig->GetValue(buffer,"area_enable",(long)0) == 0)       //新增区域，真实区域加一
            {
                m_really_count++;
                m_cconfig->SetValue(AREA_ALL_AREA,"really_count",(long)(m_really_count));
            }
            m_cconfig->SetValue(buffer,"area_num",(long)(area.point_num));
            m_cconfig->SetValue(buffer,"area_name",(area.area_name));
            m_cconfig->SetValue(buffer,"area_enable",(long)(area.enable));
            for(j = 0; j < area.point_num;j++)
            {
                snprintf(key, sizeof(key), "areaX_%02d", j);
                m_cconfig->SetValue(buffer,key,(long)(area.point[j].x));
                snprintf(key, sizeof(key), "areaY_%02d", j);
                m_cconfig->SetValue(buffer,key,(long)(area.point[j].y));
            }
            memcpy(&m_area[i], &area, sizeof(area_information));
            
            //设置下去新的区域
            
            return 0;
        }
    }
    for( i = 0; i < m_area_count; i++)
    {
        snprintf(buffer, sizeof(buffer), "%s_%02d", AREA_SECTION, i);
        if(m_cconfig->GetValue(buffer,"area_enable",(long)0) == 0)
        {
            m_cconfig->SetValue(buffer,"area_num",(long)(area.point_num));
            m_cconfig->SetValue(buffer,"area_name",(area.area_name));
            m_cconfig->SetValue(buffer,"area_enable",(long)(area.enable));
            for(j = 0; j < area.point_num;j++)
            {
                snprintf(key, sizeof(key), "areaX_%02d", j);
                m_cconfig->SetValue(buffer,key,(long)(area.point[j].x));
                snprintf(key, sizeof(key), "areaY_%02d", j);
                m_cconfig->SetValue(buffer,key,(long)(area.point[j].y));
            }
            memcpy(&m_area[i], &area, sizeof(area_information));
            
            m_really_count++;
            m_cconfig->SetValue(AREA_ALL_AREA,"really_count",(long)(m_really_count));

            //设置下去新的区域
            return 0;
        }
    } 
    m_area_count++;
    memcpy(&m_area[m_area_count-1], &area, sizeof(area_information));
    snprintf(buffer, sizeof(buffer), "%s_%02d", AREA_SECTION, m_area_count-1);
    m_cconfig->SetValue(buffer,"area_num",(long)(area.point_num));
    m_cconfig->SetValue(buffer,"area_name",(area.area_name));
    m_cconfig->SetValue(buffer,"area_enable",(long)(area.enable));
    for(j = 0; j < area.point_num; j++)
    {
        snprintf(key, sizeof(key), "areaX_%02d", j);
        m_cconfig->SetValue(buffer,key,(long)(area.point[j].x));
        snprintf(key, sizeof(key), "areaY_%02d", j);
        m_cconfig->SetValue(buffer,key,(long)(area.point[j].y));
    }
    m_really_count++;
    m_cconfig->SetValue(AREA_ALL_AREA,"all_count",(long)(m_area_count));
    m_cconfig->SetValue(AREA_ALL_AREA,"really_count",(long)(m_really_count));
    
    mysystem("sync");
    return 0;
}

int CCAreaInvasion::SetAllAreaPoint(area_information *area, int num)
{
    char buffer[AREA_NAME_BUFF_MAX] = {0};
    char key[64] = {0};
    int ret = 0;
    std::vector<std::vector<AreaPoint>> polygon;
    for(int i = 0; i < num; i++)
    {
        std::vector<AreaPoint> tmp;
        AreaPoint tmp_point;
        snprintf(buffer, sizeof(buffer), "%s_%02d", AREA_SECTION, i);
        m_cconfig->SetValue(buffer,"area_num",(long)(area[i].point_num));
        m_cconfig->SetValue(buffer,"area_name",(area[i].area_name));
        m_cconfig->SetValue(buffer,"area_enable",(long)(area[i].enable));
        for(int j = 0; j < area[i].point_num; j++)
        {
            snprintf(key, sizeof(key), "areaX_%02d", j);
            m_cconfig->SetValue(buffer,key,(long)(area[i].point[j].x));
            snprintf(key, sizeof(key), "areaY_%02d", j);
            m_cconfig->SetValue(buffer,key,(long)(area[i].point[j].y));
            tmp_point.x = area[i].point[j].x;
            tmp_point.y = area[i].point[j].y;
            tmp.emplace_back(tmp_point);
        }
        polygon.emplace_back(tmp);
    }
    if(m_area_count > num)  //关闭多余区域使能
    {
        for(int i = num; i < m_area_count; i++)
        {
            snprintf(buffer, sizeof(buffer), "%s_%02d", AREA_SECTION, i);
            m_cconfig->SetValue(buffer,"area_enable",(long)0);
            m_area[i].enable = 0;
        }
    }
    else
    {
        m_area_count = num;
    }
    m_really_count = num;
    m_cconfig->SetValue(AREA_ALL_AREA,"really_count",(long)(m_really_count));
    m_cconfig->SetValue(AREA_ALL_AREA,"all_count",(long)(m_area_count));
    memcpy(m_area, area, num * sizeof(area_information));
#ifdef ALG_SDK
    ret = SetMonitorArea(m_han, polygon);
    if(ret != 0)
    {
        ERROR("Set algarea is err\n");
        return -1;
    }
#endif
    return 0;
}



int CCAreaInvasion::GetAreaPoint(area_information *area, int *num)
{
    if(!m_init)
    {
        return -1;
    }
    area_information *tmp_area = area;
    int i = 0;
    for(int j = 0; j < m_area_count; j++)
    {
        if(m_area[j].enable == 1)
        {
            memcpy(&tmp_area[i],&m_area[j],sizeof(area_information));
            (*num)++;
            i++;
            if(*num >= 31)
                return 0;
        }
    }
    if(*num != m_really_count)
    {
        ERROR("avl get area is err\n");
        return -1;
    }
    return 0;
}

int CCAreaInvasion::DeleteAreaPoint(const char *area_name)
{
    if(!m_init)
    {
        return -1;
    }
    //char tmp_name[AREA_NAME_BUFF_MAX];
    char buffer[AREA_NAME_BUFF_MAX] = {0};
    //m_cconfig->GetValue(buffer, "area_name", NULL, tmp_name,AREA_NAME_BUFF_MAX);
    for(int i = 0; i < m_area_count;i++)
    {
        if(strcmp(m_area[i].area_name,area_name) == 0)
        {
            snprintf(buffer, sizeof(buffer), "%s_%02d", AREA_SECTION, i);
            m_cconfig->SetValue(buffer,"area_enable",(long)(0));
            //m_cconfig->SetValue(buffer,"area_name",(char)(" "));
            m_area[i].enable = 0;
            m_really_count--;
            m_cconfig->SetValue(AREA_ALL_AREA,"really_count",(long)(m_really_count));
            break;
        }
    }
    return 0;
}

int CCAreaInvasion::SetDetectEnable(int enable)
{
    if(!m_init)
    {
        return -1;
    }
    int ret = 0;
    AlgorithmSwitch alg_switch;
    memset(&alg_switch,0,sizeof(AlgorithmSwitch));
    if(enable == 1)
    {
        set_bit(&m_alg_enable,0,1);
        
    }
    else
    {
        set_bit(&m_alg_enable,0,0);
    }
    if(get_bit(&m_alg_enable,0))
    {
        alg_switch.TargetDetectionSwitch = true;
    }
    if(get_bit(&m_alg_enable,1))
    {
        alg_switch.TargetTrackSwitch = true;
    }
    if(get_bit(&m_alg_enable,2))
    {
        alg_switch.ActivityAnalysisSwitch = true;
    }
#ifdef ALG_SDK
    ret = SetAlgorithmSwitch(m_han, alg_switch);
    if(ret != 0)
    {
        ERROR("Set alg is err\n");
        return -1;
    }
#endif
    m_cconfig->SetValue(ALG_SECTION,"alg_enable",(long)(m_alg_enable));
    return 0;
}

int CCAreaInvasion::SetTrackEnable(int enable)
{
    if(!m_init)
    {
        return -1;
    }
    int ret = 0;
    AlgorithmSwitch alg_switch;
    memset(&alg_switch,0,sizeof(AlgorithmSwitch));
    if(enable == 1)
    {
        set_bit(&m_alg_enable,1,1);
        alg_switch.TargetTrackSwitch = true;
        //开关
    }
    else
    {
        set_bit(&m_alg_enable,1,0);
        alg_switch.TargetTrackSwitch = false;
    }
    if(get_bit(&m_alg_enable,0))
    {
        alg_switch.TargetDetectionSwitch = true;
    }
    if(get_bit(&m_alg_enable,1))
    {
        alg_switch.TargetTrackSwitch = true;
    }
    if(get_bit(&m_alg_enable,2))
    {
        alg_switch.ActivityAnalysisSwitch = true;
    }
#ifdef ALG_SDK
    ret = SetAlgorithmSwitch(m_han, alg_switch);
    if(ret != 0)
    {
        ERROR("Set alg is err\n");
        return -1;
    }
#endif
    m_cconfig->SetValue(ALG_SECTION,"alg_enable",(long)(m_alg_enable));
    return 0;
}

int CCAreaInvasion::SetBehaviorEnable(int enable)
{
    if(!m_init)
    {
        return -1;
    }
    int ret = 0;
    AlgorithmSwitch alg_switch;
    memset(&alg_switch,0,sizeof(AlgorithmSwitch));
    if(enable == 1)
    {
        set_bit(&m_alg_enable,2,1);
    }
    else
    {
        set_bit(&m_alg_enable,2,0);
    }
    if(get_bit(&m_alg_enable,0))
    {
        alg_switch.TargetDetectionSwitch = true;
    }
    if(get_bit(&m_alg_enable,1))
    {
        alg_switch.TargetTrackSwitch = true;
    }
    if(get_bit(&m_alg_enable,2))
    {
        alg_switch.ActivityAnalysisSwitch = true;
    }
#ifdef ALG_SDK
    ret = SetAlgorithmSwitch(m_han, alg_switch);
    if(ret != 0)
    {
        ERROR("Set alg is err\n");
        return -1;
    }
#endif
    m_cconfig->SetValue(ALG_SECTION,"alg_enable",(long)(m_alg_enable));
    return 0;
}

int CCAreaInvasion::GetAlgEnable(int *enable)
{
    if(!m_init)
    {
        return -1;
    }
    *enable = m_alg_enable;
    return 0;
}

int CCAreaInvasion::SetDetectTrackId(std::vector<long> track_id)
{
    if(!m_init)
    {
        return -1;
    }
    m_track_id.clear();
    m_track_id = track_id;
    //下发id
    return 0;
}

int CCAreaInvasion::GetDetectTrackId(std::vector<long> &track_id)
{
    track_id = m_track_id;
    return 0;
}

int CCAreaInvasion::SetDetectGasEnable(int enable)
{
    if(!m_init)
    {
        return -1;
    }
    m_cconfig->SetValue(ALG_SECTION,"gas_detect_enable",(long)(enable));
    m_gas_enable = enable;
    return 0;
}

int CCAreaInvasion::GetDetectGasEnable(int *enable)
{
    if(!m_init)
    {
        return -1;
    }
    *enable = m_gas_enable;
    return 0;
}
