/*
 * @Author: error: error: git config user.name & please set dead value or install git && error: git config user.email & please set dead value or install git & please set dead value or install git
 * @Date: 2024-05-21 13:58:55
 * @LastEditors: error: error: git config user.name & please set dead value or install git && error: git config user.email & please set dead value or install git & please set dead value or install git
 * @LastEditTime: 2024-07-30 14:53:21
 * @FilePath: \panoramic_code\src\MPU\mpu_ciu.h
 * @Description: 这是默认设置,请设置`customMade`, 打开koroFileHeader查看配置 进行设置: https://github.com/OBKoro1/koro1FileHeader/wiki/%E9%85%8D%E7%BD%AE
 */
#ifndef __MPU_CIU_H_
#define __MPU_CIU_H_



#define     CONFIG_AREAS            1
#define     CONFIG_PTZCTRL          2
#define     CONFIG_CHIPCTRL         3
#define     CONFIG_OTHRECTRL        4 
#define     CONFIG_ALGORITHEM       5


#define     ONVIF_VIDEO_ENCODER        11
#define     ONVIF_AUDIO_ENCODER        12
#define     ONVIF_PRESETS              13

int mpu_ciu_config_init();
int mpu_ciu_config_deinit();

int MPU_CIU_Set_ALL_ConfigGure(unsigned int major, unsigned int minor, void *buffer, int bufflen, void *reserve, int reserveLen);

int MPU_CIU_Get_ALL_ConfigGure(unsigned int major, unsigned int minor, void *buffer, int bufflen, void *reserve, int reserveLen);

void CIU_RegisterConfig();
void CIU_UnRegisterConfig();
#endif