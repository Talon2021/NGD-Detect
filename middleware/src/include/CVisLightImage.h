#ifndef __CVISLIGHT_IMAGE_H
#define __CVISLIGHT_IMAGE_H
#include "CConfig.h"
#include <pthread.h>
#include "Cserial.h"
#include "IR_common.h"


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

    int SetFocuMode(int mode);
    int GetFocuMode(int *mode);

    int SetSaturation(int value);
    int GetSaturation(int *value);

    int SetSharpness(int value);
    int GetSharpness(int *value);

    int SetDigitalZoom(float value);
    int GetDigitalZoom(float *value);

    int RegisterVisCtrlCb(VisControlFunctions cb);

    int SetAutoFocuData(int mode);
private:
    int LoadParam();
    int m_init;
    int m_brightness;
    int m_contrast;
    int m_focu_mode;
    int m_saturation;
    int m_sharpness;
    float m_DigitalZoom;
    pthread_mutex_t m_Lock;
    CConfig *m_cconfig;

    Cserial *m_serial;

    VisControlFunctions m_CtrlFnxCb;
};





#endif