#ifndef __CVISLIGHT_IMAGE_H
#define __CVISLIGHT_IMAGE_H
#include "CConfig.h"
#include <pthread.h>


class CVisLightImage
{

    
public:
    CVisLightImage(void *handle, int ch);
    ~CVisLightImage();
    
    int Init();
    int UnInit();

    int SetBrightness(int value);
    int GetBrightness(int *value);

    int SetContrast(int value);
    int GetContrast(int *value);

    int SetAutoFocuEnabele(int enable);
    int GetAutoFocuEnabele(int *enable);

    int SetSaturation(int value);
    int GetSaturation(int *value);

    int SetSharpness(int value);
    int GetSharpness(int *value);

private:
    int LoadParam();
    int m_init;
    int m_brightness;
    int m_contrast;
    int m_auto_focu_enable;
    int m_saturation;
    int m_sharpness;
    pthread_mutex_t m_Lock;
    CConfig *m_cconfig;
};





#endif