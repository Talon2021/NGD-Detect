
#ifndef __MPU_CIU_DEV_H_
#define __MPU_CIU_DEV_H_


int MPU_CIU_GET_DevConfig(unsigned int type, void *buff, int bufflen);
int MPU_CIU_SET_DevConfig(unsigned int type, void *buff, int bufflen);

void CIU_RegisterDevInfo();
void CIU_UnRegisterDevInfo();
#endif