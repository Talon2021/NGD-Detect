#ifndef _CCINFRARED_IMAGE_H_
#define _CCINFRARED_IMAGE_H_
#include "Cserial.h"
#include "common.h"
#define G_UART_DEV5 "/dev/ttyS5"
#define READ_SERIAL_MS  100
class CCInfraredImage
{
private:
    Cserial *m_serial;
    ThreadInfo_t m_serialThread;
    int m_init;
    void *m_han;

    float m_ElectronicZoom;
    int m_AutoFocus;
    int m_GasEnhanced;
    int m_brightness;
    int m_contrast;
    int m_pseudo;
    pthread_mutex_t m_Lock;

public:
    CCInfraredImage(void *handle, int ch);
    ~CCInfraredImage();

    int Init();

    int UnInit();
    /**
     * @description: 设置亮度
     * @param {int} value 0 ~ 99 
     * @return {int} successful 0
     */    
    int SetImageBrightness(int value);

    /**
     * @description: 获取亮度
     * @param {int} *value default 50
     * @return {int} successful 0
     */
    int GetImageBrightness(int *value);

    /**
     * @description: 设置对比度
     * @param {int} value 0 ~ 99
     * @return {int} successful 0
     */
    int SetImageContrast(int value);

    /**
     * @description: 获取对比度
     * @param {int} *value default 50
     * @return {int} successful 0
     */
    int GetImageContrast(int *value);

    /**
     * @description: 设置红外追踪
     * @param {HotspotTracking_en} value 
     *         close 0
     *         open 1
     * @return {int} successful 0
     */
    int SetHotspotTracking(int value);

    /**
     * @description: 获取红外追踪
     * @param {int} *value
     *         close 0
     *         open 1
     * @return {int} successful 0
     */
    int GetHotspotTracking(int *value);
    /**
     * @description: 外点清除设置
     * @param {int} opt
     *         坏点消除的阈值设置 0
     *         开启清除坏点 1
     *         恢复本次清除操作 2
     *         保存坏点空间 3 
     * @param {int} value
     *          opt == 0 value 阈值 20~32
     *          opt == 1 value 开关 0关闭，1打开
     *          opt == 2 value 开关 0关闭，1打开
     *          opt == 3 value 开关 0关闭，1打开
     * @return {int} successful 0
     */    
    int ManualDefectRemoval(int opt, int value);

    int GetManualDefectRemoval(int *opt, int *value);
    /**
     * @description: 画中画
     * @param {int} value
     * @return {int} successful 0
     */    
    int SetPictureInPictureSwitch(int value);
    /**
     * @description: 获取画中画
     * @param {int} *value
     * @return {int} successful 0
     */    
    int GetPictureInPictureSwitch(int *value);
    /**
     * @description: 设置锐化dde
     * @param {int} ddelv   0~7 default 4
     * @return  {int} successful 0
     */    
    int SetInfraredImageSharpening(int ddelv);
    /**
     * @description: 获取锐化等级
     * @param {int} ddelv
     * @return  {int} successful 0
     */    
    int GetInfraredImageSharpening(int *ddelv);
    /**
     * @description: 极性调节
     * @param {int} value 0~7 default 0
     *              0：白热；1：黑热；2：铁红；3：沙漠黄；4：绿热；5：红热；6：天空 7：边缘
     * @return {int} successful 0
     */    
    int SetInfraredImagePolarity(int value);
    /**
     * @description: 获取机型设置
     * @param {int} *value
     * @return {int} successful 0
     */    
    int GetInfraredImagePolarity(int *value);
    /**
     * @description: 关机报错设置，接口阻塞3s
     * @return {int} successful 0
     */    
    int InfraredImageSaveCmd();
    /**
     * @description: 设置pal视频 
     * @param {int} status 0开，1关 default 0
     * @return {int} successful 0
     */    
    int SetInfraredImagePAL(int status);
    /**
     * @description: 获取PAL视频开关
     * @param {int} status 0开，1关 default 0
     * @return {int} successful 0
     */    
    int GetInfraredImagePAL(int *status);

    /// @brief 设置电子变倍
    /// @param value 
    /// @return successful 0
    int SetInfraredImageElectronicZoom(float value);

    /// @brief 获取电子变倍
    /// @param value 
    /// @return successful 0
    int GetInfraredImageElectronicZoom(float *value);

    /// @brief 设置自动红外变焦
    /// @param enable 
    /// @return successful 0
    int SetInfraredImageAutoFocus(int enable);

    /// @brief 获取自动红外变焦使能
    /// @param enable 
    /// @return successful 0
    int GetInfraredImageAutoFocus(int *enable);

    /// @brief 设置气体增强
    /// @param enable 
    /// @return successful 0
    int SetGasEnhanced(int enable);

    /**
     * @brief Get the Gas Enhanced object
     * 
     * @param enable 
     * @return int successful 0
     */
    int GetGasEnhanced(int *enable);

    int SetElectricFocu(int action);
};






#endif