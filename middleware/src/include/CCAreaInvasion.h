/*
 * @Author: error: error: git config user.name & please set dead value or install git && error: git config user.email & please set dead value or install git & please set dead value or install git
 * @Date: 2024-05-14 16:34:21
 * @LastEditors: error: error: git config user.name & please set dead value or install git && error: git config user.email & please set dead value or install git & please set dead value or install git
 * @LastEditTime: 2024-06-19 09:51:29
 * @FilePath: \panoramic_code\src\AVL\CCAreaInvasion.h
 * @Description: 这是默认设置,请设置`customMade`, 打开koroFileHeader查看配置 进行设置: https://github.com/OBKoro1/koro1FileHeader/wiki/%E9%85%8D%E7%BD%AE
 */
#ifndef _CCAREAINVASION_H_
#define _CCAREAINVASION_H_

#define AREA_POINT_MAX      8
#define AREA_COUNT_MAX      32
#define AREA_NAME_BUFF_MAX  256
#define ALG_MAX_NUM         3

#define AREA_ALL_AREA       "area_all_information"
#define AREA_SECTION        "area"
#define ALG_SECTION          "alg_information"

#include <vector>
#include "CConfig.h"

typedef struct area_point
{
    int x;
    int y;
}area_point;


typedef struct area_information
{
    area_point point[AREA_POINT_MAX];
    int point_num;
    char area_name[AREA_NAME_BUFF_MAX];
    int type;
    int enable;
}area_information;


class CCAreaInvasion
{
private:
    int m_area_count;
    int m_really_count;
    area_information m_area[AREA_COUNT_MAX];
    int m_init;
    int m_alg_enable;       //0位代表目标检测，1位代表跟踪 2位代表行为分析
    std::vector<long> m_track_id;
    void *m_han;
    int m_gas_enable;
    CConfig *m_cconfig;
public:
    CCAreaInvasion(void *handle, int ch);

    ~CCAreaInvasion();

    int Init();

    int UnInit();

    int SetAreaPoint(area_information area);

    int SetAllAreaPoint(area_information *area, int num);

    int GetAreaPoint(area_information *area, int *num);
    
    int DeleteAreaPoint(const char *area_name);

    int SetDetectEnable(int enable);

    int SetTrackEnable(int enable);

    int SetBehaviorEnable(int enable);

    int GetAlgEnable(int *enable);

    int SetDetectTrackId(std::vector<long> track_id);
    
    int GetDetectTrackId(std::vector<long> &track_id);

    int SetDetectGasEnable(int enable);

    int GetDetectGasEnable(int *enable);
};


#endif