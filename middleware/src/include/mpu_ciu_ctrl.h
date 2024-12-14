#ifndef __MPU_CIU_CTRL_H
#define __MPU_CIU_CTRL_H



int MPU_CIU_CTRL_MODLES(int type, void *buffer, int len, void *reserve, int reserveLen);
void CIU_RegisterDevCrtl();
void CIU_UnRegisterDevCrtl();





#endif