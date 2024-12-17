/*
 * @Author: error: error: git config user.name & please set dead value or install git && error: git config user.email & please set dead value or install git & please set dead value or install git
 * @Date: 2024-05-14 11:55:32
 * @LastEditors: error: error: git config user.name & please set dead value or install git && error: git config user.email & please set dead value or install git & please set dead value or install git
 * @LastEditTime: 2024-08-29 14:15:00
 * @FilePath: \panoramic_code\src\peripaherls\CChannelManager.cpp
 * @Description: 这是默认设置,请设置`customMade`, 打开koroFileHeader查看配置 进行设置: https://github.com/OBKoro1/koro1FileHeader/wiki/%E9%85%8D%E7%BD%AE
 */
#include "CChannelManager.h"
#include "common.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/prctl.h>
#include "CCompass.h"

#include "sdk_log.h"
#include "connect.h"
#include <vector>
#include "jpsdk.h"
CChannelManager *CChannelManager::pCChannelManger = NULL;

static void *Fnx_ReportThread(void *arg)
{
    prctl(PR_SET_NAME, "thread_report", 0, 0, 0);
    CChannelManager *pccm = (CChannelManager *)arg;

    int ret;
    // double yaw_angle;
    // double temp;
    // double picth_angle;
    // double roll_angle;
    peripheral_info_st info;
    while (1)
    {
        if(pccm == NULL)
        {
            break;
        }
        memset(&info, 0, sizeof(peripheral_info_st));
        ret = pccm->GetperipheralInfo(&info);
        if(ret == -3)
        {
            sleep(3);
            continue;
        }
        else if(ret == -2)
        {
            break;
        }
        //printf("value = %lf\n",info.yaw);
        if(pccm->m_callback)
            pccm->m_callback(info);

        sleep(1);
    }

    return NULL;
    
}

CChannelManager::CChannelManager(/* args */)
{
    int i = 0;
    for(i = 0; i < MAX_CH_NUM; i++)
    {
        m_infraredImage[i] = NULL;
        m_ccgyro[i] = NULL;
        m_areahannle[i] = NULL;
        m_compasshannel[i] = NULL;
    }
    m_PeripheralReportThread = 0;
    m_reportflag = 0;
    m_iEncChannelCnt = 0;
    m_bInit = 0;
    m_reportExit = 0;
    m_callback = NULL;
}

CChannelManager::~CChannelManager()
{   
    UnInit();
}

int CChannelManager::Init()
{
    if(m_bInit == 1)
    {
        ERROR("avl is init\n");
        return -1;
    }
        
    int i = 0;
    int ret = 0;
    m_reportExit = 0;
    void *hannle = NULL;
    /*初始化底层放在该处*/
#ifdef JPMPP_SDK
    JPSys_Init(0);
    JPSys_EncRegisterFrameHookCallback(FrameHook_FNX);
#endif

#ifdef ALG_SDK
    std::vector<std::vector<AreaPoint>> polygonInit;
    hannle = SetAlgorithmInit(polygonInit); 
    DEBUG("alg sdk init\n");
    if(hannle == NULL)
    {
        ERROR("alg init is fail\n");
    }
#endif


    CConfig *pcfg = CConfig::GetInstance();
    m_reportflag = pcfg->GetValue("peripheral_info", "report_enable", (long)1);

    m_iEncChannelCnt = MAX_CH_NUM;
    for(i = 0; i < m_iEncChannelCnt; i++)
    {
        m_infraredImage[i] = new CCInfraredImage(hannle, i);
        m_ccgyro[i] = new CCGyro(hannle, i);
        m_areahannle[i] = new CCAreaInvasion(hannle, i);
        m_compasshannel[i] = new CCompass(hannle, i);
        m_Actionhannel[i] = new CActionAlarm(hannle, i);
        m_Ptzhannel[i] = new CPtzCtrl(hannle, i);
        m_coderhannel[i] = new CCoder(hannle, i);
        m_ExtCtrl[i] = new CExtctrl(hannle, i);
        m_VisLightImage[i] = new CVisLightImage(hannle, i);
        
        //ret |= m_ccgyro[i]->Init();
        ret |= m_infraredImage[i]->Init();
        //ret |= m_areahannle[i]->Init();
        //ret |= m_compasshannel[i]->Init();
        ret |= m_Actionhannel[i]->Init();
        ret |= m_Ptzhannel[i]->Init();
        ret |= m_coderhannel[i]->Init();
        ret |= m_ExtCtrl[i]->Init();
        ret |= m_VisLightImage[i]->Init();
    }
    // ret = pthread_create(&m_PeripheralReportThread, NULL, Fnx_ReportThread, this);
    // if(ret != 0)
    // {
    //     ERROR("creat m_PeripheralReportThread thread is err\n");
    //     UnInit();
    //     return -1;
    // }
    m_bInit = 1;
    return 0;
}
int CChannelManager::UnInit()
{
    int i = 0;
    m_reportflag = 0;
    m_reportExit = 1;
    pthread_cancel(m_PeripheralReportThread);
    pthread_join(m_PeripheralReportThread,NULL);
    m_PeripheralReportThread = 0;
    
    for(i = 0; i < m_iEncChannelCnt; i++)
    {
        delete m_infraredImage[i];
        m_infraredImage[i] = NULL;

        delete m_ccgyro[i];
        m_ccgyro[i] = NULL;

        delete m_areahannle[i];
        m_areahannle[i] = NULL;

        delete m_compasshannel[i];
        m_compasshannel[i] = NULL;

        delete m_Actionhannel[i];
        m_Actionhannel[i] = NULL;
        delete m_Ptzhannel[i];
        m_Ptzhannel[i] = NULL;
    }

    m_bInit = 0;
    m_iEncChannelCnt = 0;
    return 0;
}

int CChannelManager::SetAVCallBack(int (*AvCallBack)(int nCh, unsigned char *Buff, int nBuffLen))
{
    if (!m_bInit)
	{
		ERROR("[CChannelManager:%s]not inited\n", __FUNCTION__);
		return -1;
	}
	if (NULL == AvCallBack)
	{
		ERROR("[CChannelManager:%s]AvCallBack==NULL\n", __FUNCTION__);
		return -1;
	}
    m_FnxAvCallBack = AvCallBack;
    return 0;
}

int CChannelManager::RVAvReadStream(FRAME_HOOK_INFO *param)
{
    if(param == NULL)
    {
        return -1;
    }
    if((PktIFrames == param->frameType || PktPFrames == param->frameType))
    {
        m_FnxAvCallBack(0, param->pDataBuf, param->uiDataLen);
    }
    return 0;
}

int CChannelManager::FrameHook_FNX(FRAME_HOOK_INFO *param)
{
   CChannelManager *pccm = CChannelManager::GetInstanceHandle();
	
	if (NULL != pccm)
	{
		pccm->RVAvReadStream(param);
	}

	return 0;
}

CChannelManager *CChannelManager::GetInstanceHandle()
{
    if(pCChannelManger == NULL)
    {
        pCChannelManger = new CChannelManager;
        if(pCChannelManger == NULL)
        {
            ERROR("CChannelManager new is fail\n");
            return NULL;
        }
    }
    return pCChannelManger;
}

void CChannelManager::Release()
{
    if(pCChannelManger != NULL)
    {
        delete pCChannelManger;
        pCChannelManger = NULL;
    }
}

int CChannelManager::GetperipheralInfo(peripheral_info_st *info)
{
    if(m_reportExit == 1)
    {
        return -2;
    }
    if(m_reportflag == 0)
    {
        return -3;
    }
    Compass_GetYawAngle(0, &(info->yaw));
    Groy_GetGyroAngle(0, &(info->pitch), &(info->roll), &(info->temp));
    return 0;
}

int CChannelManager::SetperiphearaEnable(int enable)
{
    m_reportflag = enable;
    CConfig *pcfg = CConfig::GetInstance();
    pcfg->SetValue("peripheral_info", "report_enable", (long)m_reportflag);
    return 0;
}

int CChannelManager::SetMpuCallback(peripheralInfoCallback callback)
{
    m_callback = callback;
    return 0;
}

int CChannelManager::InfraredImage_Init(int nch)
{
    if(!m_bInit)
    {
        ERROR("CChannelManager not inited\n");
        return -1;
    }
    if(nch < 0 || nch >= m_iEncChannelCnt)
    {
        ERROR("CChannelManager channel_no err channel = %d\n",nch);
        return -1;
    }
    return m_infraredImage[nch]->Init();
}

int CChannelManager::InfraredImage_UnInit(int nch)
{
    if(!m_bInit)
    {
        ERROR("CChannelManager not inited\n");
        return -1;
    }
    if(nch < 0 || nch >= m_iEncChannelCnt)
    {
        ERROR("CChannelManager channel_no err channel = %d\n",nch);
        return -1;
    }
    return m_infraredImage[nch]->UnInit();
}

int CChannelManager::InfraredImage_SetImageBrightness(int nch, int value)
{
    if(!m_bInit)
    {
        ERROR("CChannelManager not inited\n");
        return -1;
    }
    if(nch < 0 || nch >= m_iEncChannelCnt)
    {
        ERROR("CChannelManager channel_no err channel = %d\n",nch);
        return -1;
    }
    
    return m_infraredImage[nch]->SetImageBrightness(value);
}

int CChannelManager::InfraredImage_GetImageBrightness(int nch, int *value)
{
    if(!m_bInit)
    {
        ERROR("CChannelManager not inited\n");
        return -1;
    }
    if(nch < 0 || nch >= m_iEncChannelCnt)
    {
        ERROR("CChannelManager channel_no err channel = %d\n",nch);
        return -1;
    }
    
    return m_infraredImage[nch]->GetImageBrightness(value);
}

int CChannelManager::InfraredImage_SetImageContrast(int nch, int value)
{
    if(!m_bInit)
    {
        ERROR("CChannelManager not inited\n");
        return -1;
    }
    if(nch < 0 || nch >= m_iEncChannelCnt)
    {
        ERROR("CChannelManager channel_no err channel = %d\n",nch);
        return -1;
    }
    
    return m_infraredImage[nch]->SetImageContrast(value);
}

int CChannelManager::InfraredImage_GetImageContrast(int nch, int *value)
{
    if(!m_bInit)
    {
        ERROR("CChannelManager not inited\n");
        return -1;
    }
    if(nch < 0 || nch >= m_iEncChannelCnt)
    {
        ERROR("CChannelManager channel_no err channel = %d\n",nch);
        return -1;
    }
    
    return m_infraredImage[nch]->GetImageContrast(value);
}

int CChannelManager::InfraredImage_SetHotspotTracking(int nch, int value)
{
    if(!m_bInit)
    {
        ERROR("CChannelManager not inited\n");
        return -1;
    }
    if(nch < 0 || nch >= m_iEncChannelCnt)
    {
        ERROR("CChannelManager channel_no err channel = %d\n",nch);
        return -1;
    }
    
    return m_infraredImage[nch]->SetHotspotTracking(value);
}

int CChannelManager::InfraredImage_GetHotspotTracking(int nch, int *value)
{
    if(!m_bInit)
    {
        ERROR("CChannelManager not inited\n");
        return -1;
    }
    if(nch < 0 || nch >= m_iEncChannelCnt)
    {
        ERROR("CChannelManager channel_no err channel = %d\n",nch);
        return -1;
    }
    
    return m_infraredImage[nch]->GetHotspotTracking(value);
}

int CChannelManager::InfraredImage_ManualDefectRemoval(int nch, int opt, int value)
{
    if(!m_bInit)
    {
        ERROR("CChannelManager not inited\n");
        return -1;
    }
    if(nch < 0 || nch >= m_iEncChannelCnt)
    {
        ERROR("CChannelManager channel_no err channel = %d\n",nch);
        return -1;
    }
    
    return m_infraredImage[nch]->ManualDefectRemoval(opt,value);
}

int CChannelManager::InfraredImage_GetManualDefectRemoval(int nch, int *opt, int *value)
{
    if(!m_bInit)
    {
        ERROR("CChannelManager not inited\n");
        return -1;
    }
    if(nch < 0 || nch >= m_iEncChannelCnt)
    {
        ERROR("CChannelManager channel_no err channel = %d\n",nch);
        return -1;
    }
    return m_infraredImage[nch]->GetManualDefectRemoval(opt, value);

}

int CChannelManager::InfraredImage_SetPictureInPictureSwitch(int nch, int value)
{
    if(!m_bInit)
    {
        ERROR("CChannelManager not inited\n");
        return -1;
    }
    if(nch < 0 || nch >= m_iEncChannelCnt)
    {
        ERROR("CChannelManager channel_no err channel = %d\n",nch);
        return -1;
    }
    
    return m_infraredImage[nch]->SetPictureInPictureSwitch(value);
}

int CChannelManager::InfraredImage_GetPictureInPictureSwitch(int nch, int *value)
{
    if(!m_bInit)
    {
        ERROR("CChannelManager not inited\n");
        return -1;
    }
    if(nch < 0 || nch >= m_iEncChannelCnt)
    {
        ERROR("CChannelManager channel_no err channel = %d\n",nch);
        return -1;
    }
    
    return m_infraredImage[nch]->GetPictureInPictureSwitch(value);
}

int CChannelManager::InfraredImage_SetInfraredImageSharpening(int nch, int ddelv)
{
    if(!m_bInit)
    {
        ERROR("CChannelManager not inited\n");
        return -1;
    }
    if(nch < 0 || nch >= m_iEncChannelCnt)
    {
        ERROR("CChannelManager channel_no err channel = %d\n",nch);
        return -1;
    }
    
    return m_infraredImage[nch]->SetInfraredImageSharpening(ddelv);
}

int CChannelManager::InfraredImage_GetInfraredImageSharpening(int nch, int *ddelv)
{
    if(!m_bInit)
    {
        ERROR("CChannelManager not inited\n");
        return -1;
    }
    if(nch < 0 || nch >= m_iEncChannelCnt)
    {
        ERROR("CChannelManager channel_no err channel = %d\n",nch);
        return -1;
    }
    
    return m_infraredImage[nch]->GetInfraredImageSharpening(ddelv);
}

int CChannelManager::InfraredImage_SetInfraredImagePolarity(int nch, int value)
{
    if(!m_bInit)
    {
        ERROR("CChannelManager not inited\n");
        return -1;
    }
    if(nch < 0 || nch >= m_iEncChannelCnt)
    {
        ERROR("CChannelManager channel_no err channel = %d\n",nch);
        return -1;
    }
    
    return m_infraredImage[nch]->SetInfraredImagePolarity(value);
}

int CChannelManager::InfraredImage_GetInfraredImagePolarity(int nch, int *value)
{
    if(!m_bInit)
    {
        ERROR("CChannelManager not inited\n");
        return -1;
    }
    if(nch < 0 || nch >= m_iEncChannelCnt)
    {
        ERROR("CChannelManager channel_no err channel = %d\n",nch);
        return -1;
    }
    
    return m_infraredImage[nch]->GetInfraredImagePolarity(value);
}

int CChannelManager::InfraredImage_InfraredImageSaveCmd(int nch)
{
    if(!m_bInit)
    {
        ERROR("CChannelManager not inited\n");
        return -1;
    }
    if(nch < 0 || nch >= m_iEncChannelCnt)
    {
        ERROR("CChannelManager channel_no err channel = %d\n",nch);
        return -1;
    }
    
    return m_infraredImage[nch]->InfraredImageSaveCmd();
}

int CChannelManager::InfraredImage_SetInfraredImagePAL(int nch, int status)
{
    if(!m_bInit)
    {
        ERROR("CChannelManager not inited\n");
        return -1;
    }
    if(nch < 0 || nch >= m_iEncChannelCnt)
    {
        ERROR("CChannelManager channel_no err channel = %d\n",nch);
        return -1;
    }
    
    return m_infraredImage[nch]->SetInfraredImagePAL(status);
}

int CChannelManager::InfraredImage_GetInfraredImagePAL(int nch, int *status)
{
    if(!m_bInit)
    {
        ERROR("CChannelManager not inited\n");
        return -1;
    }
    if(nch < 0 || nch >= m_iEncChannelCnt)
    {
        ERROR("CChannelManager channel_no err channel = %d\n",nch);
        return -1;
    }
    
    return m_infraredImage[nch]->GetInfraredImagePAL(status);
}

int CChannelManager::InfraredImage_SetInfraredImageElectronicZoom(int nch, float value)
{
    if(!m_bInit)
    {
        ERROR("CChannelManager not inited\n");
        return -1;
    }
    if(nch < 0 || nch >= m_iEncChannelCnt)
    {
        ERROR("CChannelManager channel_no err channel = %d\n",nch);
        return -1;
    }
    
    return m_infraredImage[nch]->SetInfraredImageElectronicZoom(value);
}

int CChannelManager::InfraredImage_GetInfraredImageElectronicZoom(int nch, float *value)
{
    if(!m_bInit)
    {
        ERROR("CChannelManager not inited\n");
        return -1;
    }
    if(nch < 0 || nch >= m_iEncChannelCnt)
    {
        ERROR("CChannelManager channel_no err channel = %d\n",nch);
        return -1;
    }
    
    return m_infraredImage[nch]->GetInfraredImageElectronicZoom(value);
}

int CChannelManager::InfraredImage_SetInfraredImageFocusMode(int nch, int mode)
{
   if(!m_bInit)
    {
        ERROR("CChannelManager not inited\n");
        return -1;
    }
    if(nch < 0 || nch >= m_iEncChannelCnt)
    {
        ERROR("CChannelManager channel_no err channel = %d\n",nch);
        return -1;
    }
    
    return m_infraredImage[nch]->SetInfraredImageFocusMode(mode);
}

int CChannelManager::InfraredImage_GetInfraredImageFocusMode(int nch, int *mode)
{
    if(!m_bInit)
    {
        ERROR("CChannelManager not inited\n");
        return -1;
    }
    if(nch < 0 || nch >= m_iEncChannelCnt)
    {
        ERROR("CChannelManager channel_no err channel = %d\n",nch);
        return -1;
    }
    
    return m_infraredImage[nch]->GetInfraredImageFocusMode(mode);
}

int CChannelManager::InfraredImage_SetGasEnhanced(int nch, int enable)
{
    if(!m_bInit)
    {
        ERROR("CChannelManager not inited\n");
        return -1;
    }
    if(nch < 0 || nch >= m_iEncChannelCnt)
    {
        ERROR("CChannelManager channel_no err channel = %d\n",nch);
        return -1;
    }
    
    return m_infraredImage[nch]->SetGasEnhanced(enable);
}

int CChannelManager::InfraredImage_GetGasEnhanced(int nch, int *enable)
{
    if(!m_bInit)
    {
        ERROR("CChannelManager not inited\n");
        return -1;
    }
    if(nch < 0 || nch >= m_iEncChannelCnt)
    {
        ERROR("CChannelManager channel_no err channel = %d\n",nch);
        return -1;
    }
    
    return m_infraredImage[nch]->GetGasEnhanced(enable);
}

int CChannelManager::InfraredImage_SetElectricFocu(int nch, int action)
{
     if(!m_bInit)
    {
        ERROR("CChannelManager not inited\n");
        return -1;
    }
    if(nch < 0 || nch >= m_iEncChannelCnt)
    {
        ERROR("CChannelManager channel_no err channel = %d\n",nch);
        return -1;
    }
    
    return m_infraredImage[nch]->SetElectricFocu(action);
}

int CChannelManager::InfraredImage_SetAutoFocuData(int nch, int type)
{
    if(!m_bInit)
    {
        ERROR("CChannelManager not inited\n");
        return -1;
    }
    if(nch < 0 || nch >= m_iEncChannelCnt)
    {
        ERROR("CChannelManager channel_no err channel = %d\n",nch);
        return -1;
    }
    
    return m_infraredImage[nch]->SetElectricFocu(type);
}

int CChannelManager::InfraredImage_SetImagesaturation(int nch, int value)
{
    if(!m_bInit)
    {
        ERROR("CChannelManager not inited\n");
        return -1;
    }
    if(nch < 0 || nch >= m_iEncChannelCnt)
    {
        ERROR("CChannelManager channel_no err channel = %d\n",nch);
        return -1;
    }
    return m_infraredImage[nch]->SetImagesaturation(value);
}

int CChannelManager::InfraredImage_GetImagesaturation(int nch, int *value)
{
    if(!m_bInit)
    {
        ERROR("CChannelManager not inited\n");
        return -1;
    }
    if(nch < 0 || nch >= m_iEncChannelCnt)
    {
        ERROR("CChannelManager channel_no err channel = %d\n",nch);
        return -1;
    }
    return m_infraredImage[nch]->GetImagesaturation(value);
}

/*======================================================================================================================================*/


int CChannelManager::Groy_Init(int nch)
{
    int ret;
    if(!m_bInit)
    {
        ERROR("CChannelManager not inited\n");
        return -1;
    }
    if(nch < 0 || nch >= m_iEncChannelCnt)
    {
        ERROR("CChannelManager channel_no err channel = %d\n",nch);
        return -1;
    }
    
    ret = m_ccgyro[nch]->Init();
    return ret;
}

int CChannelManager::Groy_UnInit(int nch)
{
    int ret = 0;
    if(!m_bInit)
    {
        ERROR("CChannelManager not inited\n");
        return -1;
    }
    if(nch < 0 || nch >= m_iEncChannelCnt)
    {
        ERROR("CChannelManager channel_no err channel = %d\n",nch);
        return -1;
    }
    ret = m_ccgyro[nch]->UnInit();
    return ret;
}

int CChannelManager::Groy_GetGyroAngle(int nch, double *picth_angle, double *roll_angle, double *temp)
{
    if(!m_bInit)
    {
        ERROR("CChannelManager not inited\n");
        return -1;
    }
    if(nch < 0 || nch >= m_iEncChannelCnt)
    {
        ERROR("CChannelManager channel_no err channel = %d\n",nch);
        return -1;
    }

    return m_ccgyro[nch]->GetGyroAngle(picth_angle, roll_angle, temp);
}

int CChannelManager::Groy_SetCalibrationSamples(int nch, int samples)
{
    if(!m_bInit)
    {
        ERROR("CChannelManager not inited\n");
        return -1;
    }
    if(nch < 0 || nch >= m_iEncChannelCnt)
    {
        ERROR("CChannelManager channel_no err channel = %d\n",nch);
        return -1;
    }

    return m_ccgyro[nch]->SetCalibrationSamples(samples);
}

int CChannelManager::Groy_GetCalibrationSamples(int nch)
{
    if(!m_bInit)
    {
        ERROR("CChannelManager not inited\n");
        return -1;
    }
    if(nch < 0 || nch >= m_iEncChannelCnt)
    {
        ERROR("CChannelManager channel_no err channel = %d\n",nch);
        return -1;
    }

    return m_ccgyro[nch]->GetCalibrationSamples();
}

int CChannelManager::Groy_SetCalibration(int nch, int flag)
{
    if(!m_bInit)
    {
        ERROR("CChannelManager not inited\n");
        return -1;
    }
    if(nch < 0 || nch >= m_iEncChannelCnt)
    {
        ERROR("CChannelManager channel_no err channel = %d\n",nch);
        return -1;
    }

    return m_ccgyro[nch]->SetCalibration(flag);
}

int CChannelManager::Groy_GetCalibrationStatus(int nch)
{
    if(!m_bInit)
    {
        ERROR("CChannelManager not inited\n");
        return -1;
    }
    if(nch < 0 || nch >= m_iEncChannelCnt)
    {
        ERROR("CChannelManager channel_no err channel = %d\n",nch);
        return -1;
    }

    return m_ccgyro[nch]->GetCalibrationStatus();
}

int CChannelManager::Groy_SetGyroAngle(int nch, double picth_angle, double roll_angle, double temp)
{
    if(!m_bInit)
    {
        ERROR("CChannelManager not inited\n");
        return -1;
    }
    if(nch < 0 || nch >= m_iEncChannelCnt)
    {
        ERROR("CChannelManager channel_no err channel = %d\n",nch);
        return -1;
    }

    return m_ccgyro[nch]->SetGyroAngle(picth_angle, roll_angle, temp);
}

int CChannelManager::Groy_GetTemp(int nch, double *value)
{
     if(!m_bInit)
    {
        ERROR("CChannelManager not inited\n");
        return -1;
    }
    if(nch < 0 || nch >= m_iEncChannelCnt)
    {
        ERROR("CChannelManager channel_no err channel = %d\n",nch);
        return -1;
    }

    return m_ccgyro[nch]->GetTemp(value);
}

/*=================================================================================================================================*/

int CChannelManager::Area_Init(int nch)
{
    if(!m_bInit)
    {
        ERROR("CChannelManager not inited\n");
        return -1;
    }
    if(nch < 0 || nch >= m_iEncChannelCnt)
    {
        ERROR("CChannelManager channel_no err channel = %d\n",nch);
        return -1;
    }
    return m_areahannle[nch]->Init();
}

int CChannelManager::Area_UnInit(int nch)
{
    if(!m_bInit)
    {
        ERROR("CChannelManager not inited\n");
        return -1;
    }
    if(nch < 0 || nch >= m_iEncChannelCnt)
    {
        ERROR("CChannelManager channel_no err channel = %d\n",nch);
        return -1;
    }
    return m_areahannle[nch]->UnInit();
}

int CChannelManager::Area_SetAreaPoint(int nch, area_information area)
{
    if(!m_bInit)
    {
        ERROR("CChannelManager not inited\n");
        return -1;
    }
    if(nch < 0 || nch >= m_iEncChannelCnt)
    {
        ERROR("CChannelManager channel_no err channel = %d\n",nch);
        return -1;
    }
    return m_areahannle[nch]->SetAreaPoint(area);
}

int CChannelManager::Area_SetAllAreaPoint(int nch, area_information *area, int num)
{
    if(!m_bInit)
    {
        ERROR("CChannelManager not inited\n");
        return -1;
    }
    if(nch < 0 || nch >= m_iEncChannelCnt)
    {
        ERROR("CChannelManager channel_no err channel = %d\n",nch);
        return -1;
    }
    return m_areahannle[nch]->SetAllAreaPoint(area, num);
}

int CChannelManager::Area_GetAreaPoint(int nch, area_information *area, int *num)
{
    if(!m_bInit)
    {
        ERROR("CChannelManager not inited\n");
        return -1;
    }
    if(nch < 0 || nch >= m_iEncChannelCnt)
    {
        ERROR("CChannelManager channel_no err channel = %d\n",nch);
        return -1;
    }
    return m_areahannle[nch]->GetAreaPoint(area, num);
}

int CChannelManager::Area_DeleteAreaPoint(int nch, const char *area_name)
{
    if(!m_bInit)
    {
        ERROR("CChannelManager not inited\n");
        return -1;
    }
    if(nch < 0 || nch >= m_iEncChannelCnt)
    {
        ERROR("CChannelManager channel_no err channel = %d\n",nch);
        return -1;
    }
    return m_areahannle[nch]->DeleteAreaPoint(area_name);
}

int CChannelManager::Alg_SetDetectEnable(int nch, int enable)
{
    if(!m_bInit)
    {
        ERROR("CChannelManager not inited\n");
        return -1;
    }
    if(nch < 0 || nch >= m_iEncChannelCnt)
    {
        ERROR("CChannelManager channel_no err channel = %d\n",nch);
        return -1;
    }
    return m_areahannle[nch]->SetDetectEnable(enable);
}

int CChannelManager::Alg_SetTrackEnable(int nch, int enable)
{
    if(!m_bInit)
    {
        ERROR("CChannelManager not inited\n");
        return -1;
    }
    if(nch < 0 || nch >= m_iEncChannelCnt)
    {
        ERROR("CChannelManager channel_no err channel = %d\n",nch);
        return -1;
    }
    return m_areahannle[nch]->SetTrackEnable(enable);
}

int CChannelManager::Alg_SetBehaviorEnable(int nch, int enable)
{
    if(!m_bInit)
    {
        ERROR("CChannelManager not inited\n");
        return -1;
    }
    if(nch < 0 || nch >= m_iEncChannelCnt)
    {
        ERROR("CChannelManager channel_no err channel = %d\n",nch);
        return -1;
    }
    return m_areahannle[nch]->SetBehaviorEnable(enable);
}

int CChannelManager::Alg_GetAlgEnable(int nch, int *enable)
{
    if(!m_bInit)
    {
        ERROR("CChannelManager not inited\n");
        return -1;
    }
    if(nch < 0 || nch >= m_iEncChannelCnt)
    {
        ERROR("CChannelManager channel_no err channel = %d\n",nch);
        return -1;
    }
    return m_areahannle[nch]->GetAlgEnable(enable);
}

int CChannelManager::Alg_SetDetectTrackId(int nch, std::vector<long> track_id)
{
    if(!m_bInit)
    {
        ERROR("CChannelManager not inited\n");
        return -1;
    }
    if(nch < 0 || nch >= m_iEncChannelCnt)
    {
        ERROR("CChannelManager channel_no err channel = %d\n",nch);
        return -1;
    }
    return m_areahannle[nch]->SetDetectTrackId(track_id);
}

int CChannelManager::Alg_GetDetectTrackId(int nch, std::vector<long> track_id)
{
    if(!m_bInit)
    {
        ERROR("CChannelManager not inited\n");
        return -1;
    }
    if(nch < 0 || nch >= m_iEncChannelCnt)
    {
        ERROR("CChannelManager channel_no err channel = %d\n",nch);
        return -1;
    }
    return m_areahannle[nch]->GetDetectTrackId(track_id);
}

int CChannelManager::Alg_SetDetectGasEnable(int nch, int enable)
{
    if(!m_bInit)
    {
        ERROR("CChannelManager not inited\n");
        return -1;
    }
    if(nch < 0 || nch >= m_iEncChannelCnt)
    {
        ERROR("CChannelManager channel_no err channel = %d\n",nch);
        return -1;
    }
    return m_areahannle[nch]->SetDetectGasEnable(enable);
}

int CChannelManager::Alg_GetDetectGasEnable(int nch, int *enable)
{
    if(!m_bInit)
    {
        ERROR("CChannelManager not inited\n");
        return -1;
    }
    if(nch < 0 || nch >= m_iEncChannelCnt)
    {
        ERROR("CChannelManager channel_no err channel = %d\n",nch);
        return -1;
    }
    return m_areahannle[nch]->GetDetectGasEnable(enable);
}

int CChannelManager::Compass_Init(int nch)
{
    int ret;
    if(!m_bInit)
    {
        ERROR("CChannelManager not inited\n");
        return -1;
    }
    if(nch < 0 || nch >= m_iEncChannelCnt)
    {
        ERROR("CChannelManager channel_no err channel = %d\n",nch);
        return -1;
    }
    ret = m_compasshannel[nch]->Init();

    return ret;
}

int CChannelManager::Compass_UnInit(int nch)
{
    if(!m_bInit)
    {
        ERROR("CChannelManager not inited\n");
        return -1;
    }
    if(nch < 0 || nch >= m_iEncChannelCnt)
    {
        ERROR("CChannelManager channel_no err channel = %d\n",nch);
        return -1;
    }
    return m_compasshannel[nch]->UnInit();
}

int CChannelManager::Compass_GetYawAngle(int nch, double *value)
{
    if(!m_bInit)
    {
        ERROR("CChannelManager not inited\n");
        return -1;
    }
    if(nch < 0 || nch >= m_iEncChannelCnt)
    {
        ERROR("CChannelManager channel_no err channel = %d\n",nch);
        return -1;
    }
    return m_compasshannel[nch]->GetYawAngle(value);
}

int CChannelManager::Compass_SetCalibrationSamples(int nch, int value)
{
    if(!m_bInit)
    {
        ERROR("CChannelManager not inited\n");
        return -1;
    }
    if(nch < 0 || nch >= m_iEncChannelCnt)
    {
        ERROR("CChannelManager channel_no err channel = %d\n",nch);
        return -1;
    }
    return m_compasshannel[nch]->SetCalibrationSamples(value);
}

int CChannelManager::Compass_GetCalibrationSamples(int nch, int *value)
{
    if(!m_bInit)
    {
        ERROR("CChannelManager not inited\n");
        return -1;
    }
    if(nch < 0 || nch >= m_iEncChannelCnt)
    {
        ERROR("CChannelManager channel_no err channel = %d\n",nch);
        return -1;
    }
    return m_compasshannel[nch]->GetCalibrationSamples(value);
}

int CChannelManager::Compass_SetCalibrationFlag(int nch, int flag)
{
    if(!m_bInit)
    {
        ERROR("CChannelManager not inited\n");
        return -1;
    }
    if(nch < 0 || nch >= m_iEncChannelCnt)
    {
        ERROR("CChannelManager channel_no err channel = %d\n",nch);
        return -1;
    }
    return m_compasshannel[nch]->SetCalibrationFlag(flag);
}

int CChannelManager::Compass_GetCalibrationFlag(int nch, int *flag)
{
    if(!m_bInit)
    {
        ERROR("CChannelManager not inited\n");
        return -1;
    }
    if(nch < 0 || nch >= m_iEncChannelCnt)
    {
        ERROR("CChannelManager channel_no err channel = %d\n",nch);
        return -1;
    }
    return m_compasshannel[nch]->GetCalibrationFlag(flag);
}

int CChannelManager::Alarm_Init(int nch)
{
    if(!m_bInit)
    {
        ERROR("CChannelManager not inited\n");
        return -1;
    }
    if(nch < 0 || nch >= m_iEncChannelCnt)
    {
        ERROR("CChannelManager channel_no err channel = %d\n",nch);
        return -1;
    }
    return m_Actionhannel[nch]->Init();
}

int CChannelManager::Alarm_UnInit(int nch)
{
    if(!m_bInit)
    {
        ERROR("CChannelManager not inited\n");
        return -1;
    }
    if(nch < 0 || nch >= m_iEncChannelCnt)
    {
        ERROR("CChannelManager channel_no err channel = %d\n",nch);
        return -1;
    }
    return m_Actionhannel[nch]->UnInit();
}

int CChannelManager::Alarm_EnableAction(int nch, int Action, int bEnable)
{
    if(!m_bInit)
    {
        ERROR("CChannelManager not inited\n");
        return -1;
    }
    if(nch < 0 || nch >= m_iEncChannelCnt)
    {
        ERROR("CChannelManager channel_no err channel = %d\n",nch);
        return -1;
    }
    return m_Actionhannel[nch]->EnableAction(Action, bEnable);
}

int CChannelManager::Alarm_SetAlarmActionCallback(int nch, ALARMCALLBACK callback)
{
    if(!m_bInit)
    {
        ERROR("CChannelManager not inited\n");
        return -1;
    }
    if(nch < 0 || nch >= m_iEncChannelCnt)
    {
        ERROR("CChannelManager channel_no err channel = %d\n",nch);
        return -1;
    }
    return m_Actionhannel[nch]->SetAlarmActionCallback(callback);
}



int CChannelManager::Ptz_Init(int nch)
{
    if(!m_bInit)
    {
        ERROR("CChannelManager not inited\n");
        return -1;
    }
    if(nch < 0 || nch >= m_iEncChannelCnt)
    {
        ERROR("CChannelManager channel_no err channel = %d\n",nch);
        return -1;
    }
    return m_Ptzhannel[nch]->Init();
}

int CChannelManager::Ptz_UnInit(int nch)
{
    if(!m_bInit)
    {
        ERROR("CChannelManager not inited\n");
        return -1;
    }
    if(nch < 0 || nch >= m_iEncChannelCnt)
    {
        ERROR("CChannelManager channel_no err channel = %d\n",nch);
        return -1;
    }
    return m_Ptzhannel[nch]->UnInit();
}

int CChannelManager::Ptz_SetPtzAngle(int nch, double yaw, double pitch)
{
    int ret = 0;
    if(!m_bInit)
    {
        ERROR("CChannelManager not inited\n");
        return -1;
    }
    if(nch < 0 || nch >= m_iEncChannelCnt)
    {
        ERROR("CChannelManager channel_no err channel = %d\n",nch);
        return -1;
    }
    //ret |= m_Ptzhannel[nch]->SetPtzYawAngle(yaw);
    ret |= m_Ptzhannel[nch]->SetPtzPitchAngle(pitch);
    return ret;
}

int CChannelManager::Ptz_GetPtzAngle(int nch, double *yaw, double *pitch)
{
    int ret = 0;
    if(!m_bInit)
    {
        ERROR("CChannelManager not inited\n");
        return -1;
    }
    if(nch < 0 || nch >= m_iEncChannelCnt)
    {
        ERROR("CChannelManager channel_no err channel = %d\n",nch);
        return -1;
    }
    ret |= m_Ptzhannel[nch]->GetPtzYawAngle(yaw);
    ret |= m_Ptzhannel[nch]->GetPtzPitchAngle(pitch);
    return ret;
}

int CChannelManager::Ptz_SetScanMode(int nch, int mode)
{
    if(!m_bInit)
    {
        ERROR("CChannelManager not inited\n");
        return -1;
    }
    if(nch < 0 || nch >= m_iEncChannelCnt)
    {
        ERROR("CChannelManager channel_no err channel = %d\n",nch);
        return -1;
    }
    return m_Ptzhannel[nch]->SetScanMode(mode);
}

int CChannelManager::Ptz_GetScanMode(int nch, int *mode)
{
    if(!m_bInit)
    {
        ERROR("CChannelManager not inited\n");
        return -1;
    }
    if(nch < 0 || nch >= m_iEncChannelCnt)
    {
        ERROR("CChannelManager channel_no err channel = %d\n",nch);
        return -1;
    }
    return m_Ptzhannel[nch]->GetScanMode(mode);
}

int CChannelManager::Ptz_SetStep(int nch, double step)
{
    if(!m_bInit)
    {
        ERROR("CChannelManager not inited\n");
        return -1;
    }
    if(nch < 0 || nch >= m_iEncChannelCnt)
    {
        ERROR("CChannelManager channel_no err channel = %d\n",nch);
        return -1;
    }
    return m_Ptzhannel[nch]->SetStep(step);
}

int CChannelManager::Ptz_GetStep(int nch, double *step)
{
    if(!m_bInit)
    {
        ERROR("CChannelManager not inited\n");
        return -1;
    }
    if(nch < 0 || nch >= m_iEncChannelCnt)
    {
        ERROR("CChannelManager channel_no err channel = %d\n",nch);
        return -1;
    }
    return m_Ptzhannel[nch]->GetStep(step);
}

int CChannelManager::Ptz_SetPtzEnable(int nch, int enable)
{
    if(!m_bInit)
    {
        ERROR("CChannelManager not inited\n");
        return -1;
    }
    if(nch < 0 || nch >= m_iEncChannelCnt)
    {
        ERROR("CChannelManager channel_no err channel = %d\n",nch);
        return -1;
    }
    return m_Ptzhannel[nch]->SetPtzEnable(enable);
}

int CChannelManager::Ptz_GetPtzEnable(int nch, int *enable)
{
    if(!m_bInit)
    {
        ERROR("CChannelManager not inited\n");
        return -1;
    }
    if(nch < 0 || nch >= m_iEncChannelCnt)
    {
        ERROR("CChannelManager channel_no err channel = %d\n",nch);
        return -1;
    }
    return m_Ptzhannel[nch]->GetPtzEnable(enable);
}

int CChannelManager::Ptz_ZeroInit(int nch)
{
    if(!m_bInit)
    {
        ERROR("CChannelManager not inited\n");
        return -1;
    }
    if(nch < 0 || nch >= m_iEncChannelCnt)
    {
        ERROR("CChannelManager channel_no err channel = %d\n",nch);
        return -1;
    }
    return m_Ptzhannel[nch]->PtzZeroInit();
}

int CChannelManager::Ptz_SetTargetLocation(int nch, target_data data)
{
    if(!m_bInit)
    {
        ERROR("CChannelManager not inited\n");
        return -1;
    }
    if(nch < 0 || nch >= m_iEncChannelCnt)
    {
        ERROR("CChannelManager channel_no err channel = %d\n",nch);
        return -1;
    }
    return m_Ptzhannel[nch]->SetTargetLocation(data);
}

int CChannelManager::Ptz_GetTargetLocation(int nch, target_data *data)
{
    if(!m_bInit)
    {
        ERROR("CChannelManager not inited\n");
        return -1;
    }
    if(nch < 0 || nch >= m_iEncChannelCnt)
    {
        ERROR("CChannelManager channel_no err channel = %d\n",nch);
        return -1;
    }
    return m_Ptzhannel[nch]->GetTargetLocation(data);
}

int CChannelManager::Ptz_SetFanScanAngle(int nch, double startAngle, double endAngle)
{
    if(!m_bInit)
    {
        ERROR("CChannelManager not inited\n");
        return -1;
    }
    if(nch < 0 || nch >= m_iEncChannelCnt)
    {
        ERROR("CChannelManager channel_no err channel = %d\n",nch);
        return -1;
    }
    return m_Ptzhannel[nch]->SetFanScanAngle(startAngle, endAngle);
}

int CChannelManager::Ptz_GetFanScanAngle(int nch, double *startAngle, double *endAngle)
{
    if(!m_bInit)
    {
        ERROR("CChannelManager not inited\n");
        return -1;
    }
    if(nch < 0 || nch >= m_iEncChannelCnt)
    {
        ERROR("CChannelManager channel_no err channel = %d\n",nch);
        return -1;
    }
    return m_Ptzhannel[nch]->GetFanScanAngle(startAngle, endAngle);
}

int CChannelManager::Ptz_SetScanSpeed(int nch, unsigned short speed)
{
    if(!m_bInit)
    {
        ERROR("CChannelManager not inited\n");
        return -1;
    }
    if(nch < 0 || nch >= m_iEncChannelCnt)
    {
        ERROR("CChannelManager channel_no err channel = %d\n",nch);
        return -1;
    }
    return m_Ptzhannel[nch]->SetScanSpeed(speed);
}

int CChannelManager::Ptz_GetScanSpeed(int nch, unsigned short *speed)
{
    if(!m_bInit)
    {
        ERROR("CChannelManager not inited\n");
        return -1;
    }
    if(nch < 0 || nch >= m_iEncChannelCnt)
    {
        ERROR("CChannelManager channel_no err channel = %d\n",nch);
        return -1;
    }
    return m_Ptzhannel[nch]->GetScanSpeed(speed);
}

int CChannelManager::Ptz_SetFanEnable(int nch, int enable)
{
    if(!m_bInit)
    {
        ERROR("CChannelManager not inited\n");
        return -1;
    }
    if(nch < 0 || nch >= m_iEncChannelCnt)
    {
        ERROR("CChannelManager channel_no err channel = %d\n",nch);
        return -1;
    }
    return m_Ptzhannel[nch]->SetFanEnable(enable);
}

int CChannelManager::Ptz_GetFanEnable(int nch)
{
    if(!m_bInit)
    {
        ERROR("CChannelManager not inited\n");
        return -1;
    }
    if(nch < 0 || nch >= m_iEncChannelCnt)
    {
        ERROR("CChannelManager channel_no err channel = %d\n",nch);
        return -1;
    }
    return m_Ptzhannel[nch]->GetFanEnable();
}

int CChannelManager::Ptz_SetHeatingEnbale(int nch, int enable)
{
    if(!m_bInit)
    {
        ERROR("CChannelManager not inited\n");
        return -1;
    }
    if(nch < 0 || nch >= m_iEncChannelCnt)
    {
        ERROR("CChannelManager channel_no err channel = %d\n",nch);
        return -1;
    }
    return m_Ptzhannel[nch]->SetHeatingEnbale(enable);
}

int CChannelManager::Ptz_GetHeatingEnbale(int nch)
{
    if(!m_bInit)
    {
        ERROR("CChannelManager not inited\n");
        return -1;
    }
    if(nch < 0 || nch >= m_iEncChannelCnt)
    {
        ERROR("CChannelManager channel_no err channel = %d\n",nch);
        return -1;
    }
    return m_Ptzhannel[nch]->GetHeatingEnbale();
}

int CChannelManager::Ptz_SetPreset(int nch, char *preset_name)
{
    if(!m_bInit)
    {
        ERROR("CChannelManager not inited\n");
        return -1;
    }
    if(nch < 0 || nch >= m_iEncChannelCnt)
    {
        ERROR("CChannelManager channel_no err channel = %d\n",nch);
        return -1;
    }
    return m_Ptzhannel[nch]->SetPreset(preset_name);
}

int CChannelManager::Ptz_GetPreset(int nch, traget_preset *presetInfo, int *num)
{
    if(!m_bInit)
    {
        ERROR("CChannelManager not inited\n");
        return -1;
    }
    if(nch < 0 || nch >= m_iEncChannelCnt)
    {
        ERROR("CChannelManager channel_no err channel = %d\n",nch);
        return -1;
    }
    return m_Ptzhannel[nch]->GetPreset(presetInfo, num);
}

int CChannelManager::Ptz_DelPreset(int nch, char *preset_name)
{
    if(!m_bInit)
    {
        ERROR("CChannelManager not inited\n");
        return -1;
    }
    if(nch < 0 || nch >= m_iEncChannelCnt)
    {
        ERROR("CChannelManager channel_no err channel = %d\n",nch);
        return -1;
    }
    return m_Ptzhannel[nch]->DelPreset(preset_name);
}

int CChannelManager::Ptz_SetPresetEx(int nch, traget_preset *presetInfo, int num)
{
    if(!m_bInit)
    {
        ERROR("CChannelManager not inited\n");
        return -1;
    }
    if(nch < 0 || nch >= m_iEncChannelCnt)
    {
        ERROR("CChannelManager channel_no err channel = %d\n",nch);
        return -1;
    }
    return m_Ptzhannel[nch]->SetPresetEx(presetInfo, num);
}

int CChannelManager::Ptz_DataTransm(int nch, void *data, int len, void *out)
{
    if(!m_bInit)
    {
        ERROR("CChannelManager not inited\n");
        return -1;
    }
    if(nch < 0 || nch >= m_iEncChannelCnt)
    {
        ERROR("CChannelManager channel_no err channel = %d\n",nch);
        return -1;
    }
    return m_Ptzhannel[nch]->DataTransm(data, len, out);
}

int CChannelManager::Coder_Init(int nch)
{
    if(!m_bInit)
    {
        ERROR("CChannelManager not inited\n");
        return -1;
    }
    if(nch < 0 || nch >= m_iEncChannelCnt)
    {
        ERROR("CChannelManager channel_no err channel = %d\n",nch);
        return -1;
    }
    return m_coderhannel[nch]->Init();
}

int CChannelManager::Coder_UnInit(int nch)
{
    if(!m_bInit)
    {
        ERROR("CChannelManager not inited\n");
        return -1;
    }
    if(nch < 0 || nch >= m_iEncChannelCnt)
    {
        ERROR("CChannelManager channel_no err channel = %d\n",nch);
        return -1;
    }
    return m_coderhannel[nch]->UnInit();
}

int CChannelManager::Coder_GetVideoCoderEncParam(int nch, video_codec *Enc)
{
    if(!m_bInit)
    {
        ERROR("CChannelManager not inited\n");
        return -1;
    }
    if(nch < 0 || nch >= m_iEncChannelCnt)
    {
        ERROR("CChannelManager channel_no err channel = %d\n",nch);
        return -1;
    }
    return m_coderhannel[nch]->GetVideoCoderEncParam(Enc);
}

int CChannelManager::Coder_SetVideoCoderEncParam(int nch, video_codec Enc)
{
    if(!m_bInit)
    {
        ERROR("CChannelManager not inited\n");
        return -1;
    }
    if(nch < 0 || nch >= m_iEncChannelCnt)
    {
        ERROR("CChannelManager channel_no err channel = %d\n",nch);
        return -1;
    }
    return m_coderhannel[nch]->SetVideoCoderEncParam(Enc);
}

int CChannelManager::Coder_GetAudioCoderEncParam(int nch, audio_codec *Enc)
{
    if(!m_bInit)
    {
        ERROR("CChannelManager not inited\n");
        return -1;
    }
    if(nch < 0 || nch >= m_iEncChannelCnt)
    {
        ERROR("CChannelManager channel_no err channel = %d\n",nch);
        return -1;
    }
    return m_coderhannel[nch]->GetAudioCoderEncParam(Enc);
}

int CChannelManager::Coder_SetAudioCoderEncParam(int nch, audio_codec Enc)
{
    if(!m_bInit)
    {
        ERROR("CChannelManager not inited\n");
        return -1;
    }
    if(nch < 0 || nch >= m_iEncChannelCnt)
    {
        ERROR("CChannelManager channel_no err channel = %d\n",nch);
        return -1;
    }
    return m_coderhannel[nch]->SetAudioCoderEncParam(Enc);
}

int CChannelManager::Coder_SetLanguage(int nch, char *language)
{
    if(!m_bInit)
    {
        ERROR("CChannelManager not inited\n");
        return -1;
    }
    if(nch < 0 || nch >= m_iEncChannelCnt)
    {
        ERROR("CChannelManager channel_no err channel = %d\n",nch);
        return -1;
    }
    return m_coderhannel[nch]->SetLanguage(language);
}

int CChannelManager::Coder_GetLanguage(int nch, char *language)
{
    if(!m_bInit)
    {
        ERROR("CChannelManager not inited\n");
        return -1;
    }
    if(nch < 0 || nch >= m_iEncChannelCnt)
    {
        ERROR("CChannelManager channel_no err channel = %d\n",nch);
        return -1;
    }
    return m_coderhannel[nch]->GetLanguage(language);
}

int CChannelManager::Coder_SetTimeConfig(int nch, time_cfg cfg)
{
   if(!m_bInit)
    {
        ERROR("CChannelManager not inited\n");
        return -1;
    }
    if(nch < 0 || nch >= m_iEncChannelCnt)
    {
        ERROR("CChannelManager channel_no err channel = %d\n",nch);
        return -1;
    }
    return m_coderhannel[nch]->SetTimeConfig(cfg);
}

int CChannelManager::Coder_GetTimeConfig(int nch, time_cfg *cfg)
{
   if(!m_bInit)
    {
        ERROR("CChannelManager not inited\n");
        return -1;
    }
    if(nch < 0 || nch >= m_iEncChannelCnt)
    {
        ERROR("CChannelManager channel_no err channel = %d\n",nch);
        return -1;
    }
    return m_coderhannel[nch]->GetTimeConfig(cfg);
}

int CChannelManager::Coder_SetTime(int nch, unsigned int time)
{
    if(!m_bInit)
    {
        ERROR("CChannelManager not inited\n");
        return -1;
    }
    if(nch < 0 || nch >= m_iEncChannelCnt)
    {
        ERROR("CChannelManager channel_no err channel = %d\n",nch);
        return -1;
    }
    return m_coderhannel[nch]->SetTime(time);
}

int CChannelManager::Ext_SetWipersEnable(int nch, int enable)
{
    if(!m_bInit)
    {
        ERROR("CChannelManager not inited\n");
        return -1;
    }
    if(nch < 0 || nch >= m_iEncChannelCnt)
    {
        ERROR("CChannelManager channel_no err channel = %d\n",nch);
        return -1;
    }
    return m_ExtCtrl[nch]->SetWipersEnable(enable);
}

int CChannelManager::Ext_GetWipersEnable(int nch, int *enable)
{
    if(!m_bInit)
    {
        ERROR("CChannelManager not inited\n");
        return -1;
    }
    if(nch < 0 || nch >= m_iEncChannelCnt)
    {
        ERROR("CChannelManager channel_no err channel = %d\n",nch);
        return -1;
    }
    return m_ExtCtrl[nch]->GetWipersEnable(enable);
}

int CChannelManager::Ext_SetAutoLightEnable(int nch, int enable)
{
    if(!m_bInit)
    {
        ERROR("CChannelManager not inited\n");
        return -1;
    }
    if(nch < 0 || nch >= m_iEncChannelCnt)
    {
        ERROR("CChannelManager channel_no err channel = %d\n",nch);
        return -1;
    }
    return m_ExtCtrl[nch]->SetAutoLightEnable(enable);
}

int CChannelManager::Ext_GetAutoLightEnable(int nch, int *enable)
{
    if(!m_bInit)
    {
        ERROR("CChannelManager not inited\n");
        return -1;
    }
    if(nch < 0 || nch >= m_iEncChannelCnt)
    {
        ERROR("CChannelManager channel_no err channel = %d\n",nch);
        return -1;
    }
    return m_ExtCtrl[nch]->GetAutoLightEnable(enable);
}

int CChannelManager::Ext_SetCvbsEnable(int nch, int enable)
{
    if(!m_bInit)
    {
        ERROR("CChannelManager not inited\n");
        return -1;
    }
    if(nch < 0 || nch >= m_iEncChannelCnt)
    {
        ERROR("CChannelManager channel_no err channel = %d\n",nch);
        return -1;
    }
    return m_ExtCtrl[nch]->SetCvbsEnable(enable);
}

int CChannelManager::Ext_GetCvbsEnable(int nch, int *enable)
{
    if(!m_bInit)
    {
        ERROR("CChannelManager not inited\n");
        return -1;
    }
    if(nch < 0 || nch >= m_iEncChannelCnt)
    {
        ERROR("CChannelManager channel_no err channel = %d\n",nch);
        return -1;
    }
    return m_ExtCtrl[nch]->GetCvbsEnable(enable);
}

int CChannelManager::Ext_GetDevVersionInfo(int nch, DevInfo_st *info)
{
     if(!m_bInit)
    {
        ERROR("CChannelManager not inited\n");
        return -1;
    }
    if(nch < 0 || nch >= m_iEncChannelCnt)
    {
        ERROR("CChannelManager channel_no err channel = %d\n",nch);
        return -1;
    }
    return m_ExtCtrl[nch]->GetDevVersionInfo(info);
}

int CChannelManager::Vis_SetBrightness(int nch, int value)
{
    if(!m_bInit)
    {
        ERROR("CChannelManager not inited\n");
        return -1;
    }
    if(nch < 0 || nch >= m_iEncChannelCnt)
    {
        ERROR("CChannelManager channel_no err channel = %d\n",nch);
        return -1;
    }
    return m_VisLightImage[nch]->SetBrightness(value);
}

int CChannelManager::Vis_GetBrightness(int nch, int *value)
{
    if(!m_bInit)
    {
        ERROR("CChannelManager not inited\n");
        return -1;
    }
    if(nch < 0 || nch >= m_iEncChannelCnt)
    {
        ERROR("CChannelManager channel_no err channel = %d\n",nch);
        return -1;
    }
    return m_VisLightImage[nch]->GetBrightness(value);
}

int CChannelManager::Vis_SetContrast(int nch, int value)
{
    if(!m_bInit)
    {
        ERROR("CChannelManager not inited\n");
        return -1;
    }
    if(nch < 0 || nch >= m_iEncChannelCnt)
    {
        ERROR("CChannelManager channel_no err channel = %d\n",nch);
        return -1;
    }
    return m_VisLightImage[nch]->SetContrast(value);
}

int CChannelManager::Vis_GetContrast(int nch, int *value)
{
    if(!m_bInit)
    {
        ERROR("CChannelManager not inited\n");
        return -1;
    }
    if(nch < 0 || nch >= m_iEncChannelCnt)
    {
        ERROR("CChannelManager channel_no err channel = %d\n",nch);
        return -1;
    }
    return m_VisLightImage[nch]->GetContrast(value);
}

int CChannelManager::Vis_SetFocuMode(int nch, int mode)
{
    if(!m_bInit)
    {
        ERROR("CChannelManager not inited\n");
        return -1;
    }
    if(nch < 0 || nch >= m_iEncChannelCnt)
    {
        ERROR("CChannelManager channel_no err channel = %d\n",nch);
        return -1;
    }
    return m_VisLightImage[nch]->SetFocuMode(mode);
}

int CChannelManager::Vis_GetFocuMode(int nch, int *mode)
{
    if(!m_bInit)
    {
        ERROR("CChannelManager not inited\n");
        return -1;
    }
    if(nch < 0 || nch >= m_iEncChannelCnt)
    {
        ERROR("CChannelManager channel_no err channel = %d\n",nch);
        return -1;
    }
    return m_VisLightImage[nch]->GetFocuMode(mode);
}

int CChannelManager::Vis_SetSaturation(int nch, int value)
{
    if(!m_bInit)
    {
        ERROR("CChannelManager not inited\n");
        return -1;
    }
    if(nch < 0 || nch >= m_iEncChannelCnt)
    {
        ERROR("CChannelManager channel_no err channel = %d\n",nch);
        return -1;
    }
    return m_VisLightImage[nch]->SetSaturation(value);
}

int CChannelManager::Vis_GetSaturation(int nch, int *value)
{
    if(!m_bInit)
    {
        ERROR("CChannelManager not inited\n");
        return -1;
    }
    if(nch < 0 || nch >= m_iEncChannelCnt)
    {
        ERROR("CChannelManager channel_no err channel = %d\n",nch);
        return -1;
    }
    return m_VisLightImage[nch]->GetSaturation(value);
}

int CChannelManager::Vis_SetSharpness(int nch, int value)
{
    if(!m_bInit)
    {
        ERROR("CChannelManager not inited\n");
        return -1;
    }
    if(nch < 0 || nch >= m_iEncChannelCnt)
    {
        ERROR("CChannelManager channel_no err channel = %d\n",nch);
        return -1;
    }
    return m_VisLightImage[nch]->SetSharpness(value);
}

int CChannelManager::Vis_GetSharpness(int nch, int *value)
{
     if(!m_bInit)
    {
        ERROR("CChannelManager not inited\n");
        return -1;
    }
    if(nch < 0 || nch >= m_iEncChannelCnt)
    {
        ERROR("CChannelManager channel_no err channel = %d\n",nch);
        return -1;
    }
    return m_VisLightImage[nch]->GetSharpness(value);
}
