/*
 * @Author: error: error: git config user.name & please set dead value or install git && error: git config user.email & please set dead value or install git & please set dead value or install git
 * @Date: 2024-05-14 14:28:10
 * @LastEditors: error: error: git config user.name & please set dead value or install git && error: git config user.email & please set dead value or install git & please set dead value or install git
 * @LastEditTime: 2024-07-30 14:55:33
 * @FilePath: \panoramic_code\src\MPU\mpu_init.cpp
 * @Description: 这是默认设置,请设置`customMade`, 打开koroFileHeader查看配置 进行设置: https://github.com/OBKoro1/koro1FileHeader/wiki/%E9%85%8D%E7%BD%AE
 */
#include "mpu_init.h"
#include "mpu_avl_api.h"
#include "common.h"
#include "CActionAlarm.h"
#include "sdk_log.h"
#include "network.h"
#include "network_common.h"
#include "mpu_ciu_config.h"
#include "CChannelManager.h"
#include "mpu_ciu_preview.h"
#include "mpu_ciu_ptz.h"
//#include "onvif.h"
#include "mpu_net_api.h"
#include <map>
#include <unistd.h>
//调用初始化动作
#define MPU_CALL(x)																\
do																				\
{																				\
	int nRet = x;																		\
																				\
	if (0 != nRet)															\
	{																			\
		ERROR("%s, return %d\n", #x, nRet);						                \
	}																			\
                                                                   \
}																				\
while (0)


static std::map<std::string, int> mapping_onvif = {
    {"video_encoder", ONVIF_VIDEO_ENCODER},
    {"audio_encoder", ONVIF_AUDIO_ENCODER},
    {"presets", ONVIF_PRESETS},
};
static int getMappedValue(const std::map<std::string, int> map, const std::string& key) {
    auto it = map.find(key);
    if (it != map.end()) {
        return it->second;  
    } else {
        return -1;  
    }
}
#ifdef ONVIF_PRO
static std::map<int, int> Ptz_ctrl_onvif = {
    {ONVIF_PTZ_CTRL_UP, JP_PTZ_CTRL_UP},
    {ONVIF_PTZ_CTRL_DOWN, JP_PTZ_CTRL_DOWN},
    {ONVIF_PTZ_CTRL_LEFT, JP_PTZ_CTRL_LEFT},
	{ONVIF_PTZ_CTRL_RIGHT, JP_PTZ_CTRL_RIGHT},
	{ONVIF_PTZ_CTRL_HOME, JP_PTZ_CTRL_HOME},
	{ONVIF_PTZ_CTRL_PRESET, JP_PTZ_CTRL_PRESET},
};

static int MPU_GetVideoEncoderCfg(OnvifVideoEncoder *pcfg)
{
    int ret = 0;
    if(pcfg == NULL)
    {
        ERROR("param is err\n");
        return -1;
    }
    video_codec Enc;
    AVL_Coder_GetVideoCoderEncParam(0, &Enc);
    if(Enc.encoderType == H264)
    {
        pcfg->encoding = ONVIF_VIDEO_ENCODING_H264;
        if(Enc.encoderType == 77)
            pcfg->profile = ONVIF_H264_MAIN;
    }
    else if(Enc.encoderType == MJPEG)
    {
        pcfg->encoding = ONVIF_VIDEO_ENCODING_JPEG;
        pcfg->profile = ONVIF_MPEG4_SP;
    }
    pcfg->gov_length = Enc.gop;
    pcfg->resolution_width = Enc.encWidth;
    pcfg->resolution_height = Enc.encHeight;
    printf(" pcfg->encoding = %d pcfg->profile = %d pcfg->gov_length = %d pcfg->resolution_width = %d pcfg->resolution_height =%d\n",pcfg->encoding, pcfg->profile, pcfg->gov_length, pcfg->resolution_width, pcfg->resolution_height);
    return 0;
}

static int MPU_SetVideoEncoderCfg(OnvifVideoEncoder *pcfg)
{
    if(pcfg == NULL)
    {
        ERROR("param is err\n");
        return -1;
    }
    video_codec Enc;
    AVL_Coder_GetVideoCoderEncParam(0, &Enc);
    Enc.gop = pcfg->gov_length;
    Enc.encHeight = pcfg->resolution_height;
    Enc.encWidth = pcfg->resolution_width;
    if(pcfg->encoding == ONVIF_VIDEO_ENCODING_H264)
    {
        Enc.encoderType == H264;
        if(pcfg->profile == ONVIF_H264_MAIN)
            Enc.profileLever = 77;
        else if(pcfg->profile == ONVIF_H264_BASELINE)
            Enc.profileLever = 66;
        else if(pcfg->profile == ONVIF_H264_EXTENDED)
            Enc.profileLever = 88;
        else if(pcfg->profile == ONVIF_H264_HIGH)
            Enc.profileLever = 100;
    }
    else if(pcfg->encoding == ONVIF_VIDEO_ENCODING_JPEG)
    {
        Enc.encoderType == MJPEG;
    }
    AVL_Coder_SetVideoCoderEncParam(0, Enc);
    return 0;
}

static int MPU_GetAudioEncoderCfg(OnvifAudioEncoder *pcfg)
{
    if(pcfg == NULL)
    {
        ERROR("param is err\n");
        return -1;
    }
    audio_codec Enc;
    AVL_Coder_GetAudioCoderEncParam(0, &Enc);
    if(Enc.encoderType == A_CODEC_TYPE_AAC)
    {
        pcfg->encoding = ONVIF_AUDIO_ENCODING_AAC;
    }
    else if(Enc.encoderType == A_CODEC_TYPE_G711A || Enc.encoderType == A_CODEC_TYPE_G711U)
    {
        pcfg->encoding = ONVIF_AUDIO_ENCODING_G711;
    }
    else if(Enc.encoderType == A_CODEC_TYPE_G726)
    {
        pcfg->encoding = ONVIF_AUDIO_ENCODING_G726;
    }
    pcfg->channels = Enc.channel;
    pcfg->sample_rate = Enc.sampleRate;
    pcfg->bitrate = Enc.NBSamples;
    return 0;
}

static int MPU_SetAudioEncoderCfg(OnvifAudioEncoder *pcfg)
{
    if(pcfg == NULL)
    {
        ERROR("param is err\n");
        return -1;
    }
    
    return 0;
}

static int MPU_GetPTZrCfg(OnvifPresets *pcfg)
{
    if(pcfg == NULL)
    {
        ERROR("param is err\n");
        return -1;
    }
    AVL_Ptz_GetPreset(0, pcfg->preset, &pcfg->preset_num);
    return 0;
}

static int MPU_SetPTZrCfg(OnvifPresets *pcfg)
{
    if(pcfg == NULL)
    {
        ERROR("param is err\n");
        return -1;
    }
    if(pcfg->type == ONVIF_PRESET_CTRL_ADD)
    {
        AVL_Ptz_SetPreset(0, pcfg->preset[0]);
    }
    else if(pcfg->type == ONVIF_PRESET_CTRL_DEL)
    {
        AVL_Ptz_DelPreset(0, pcfg->preset[0]);
    }
    return 0;
}


int MPU_OnvifOperCb(OnvifOperType type, void *buf1, void *buf2)
{
    int ret = 0;
    int opt_type = -1;
    DEBUG("type = %d buf1 = %p\n",type, buf1);
    switch (type)
    {
    case ONVIF_OPER_GET_CONFIG:
        opt_type = getMappedValue(mapping_onvif, (char *)buf1);
        switch (opt_type)
        {
        case ONVIF_VIDEO_ENCODER:
        {
            OnvifVideoEncoder *pcfg = (OnvifVideoEncoder * )buf2;
            if(pcfg == NULL)
            {
                ERROR("param is err\n");
                return -1;
            }
            ret = MPU_GetVideoEncoderCfg(pcfg);
            break;
        }
        case ONVIF_AUDIO_ENCODER:
        {
            OnvifAudioEncoder *pcfg = (OnvifAudioEncoder * )buf2;
            if(pcfg == NULL)
            {
                ERROR("param is err\n");
                return -1;
            }
            ret = MPU_GetAudioEncoderCfg(pcfg);
            break;
        }
        case ONVIF_PRESETS:
        {
            OnvifPresets *pcfg = (OnvifPresets * )buf2;
            if(pcfg == NULL)
            {
                ERROR("param is err\n");
                return -1;
            }
            ret = MPU_GetPTZrCfg(pcfg);
            break;
        }
        default:
            break;
        }
        break;
    case ONVIF_OPER_SET_CONFIG:
        opt_type = getMappedValue(mapping_onvif, (char *)buf1);
        switch (opt_type)
        {
        case ONVIF_VIDEO_ENCODER:
        {
            OnvifVideoEncoder *pcfg = (OnvifVideoEncoder * )buf2;
            if(pcfg == NULL)
            {
                ERROR("param is err\n");
                return -1;
            }
            ret = MPU_SetVideoEncoderCfg(pcfg);
            break;
        }
        case ONVIF_AUDIO_ENCODER:
        {
            OnvifAudioEncoder *pcfg = (OnvifAudioEncoder * )buf2;
            if(pcfg == NULL)
            {
                ERROR("param is err\n");
                return -1;
            }
            ret = MPU_SetAudioEncoderCfg(pcfg);
            break;
        }
        case ONVIF_PRESETS:
        {
            OnvifPresets *pcfg = (OnvifPresets * )buf2;
            if(pcfg == NULL)
            {
                ERROR("param is err\n");
                return -1;
            }
            ret = MPU_SetPTZrCfg(pcfg);
            break;
        }
        default:
            break;
        }
        break;
    case ONVIF_OPER_PTZ_CTRL:
    {
        int minorType = *(int *)buf1;
		auto it = Ptz_ctrl_onvif.find(minorType);
		if((it == Ptz_ctrl_onvif.end()))
		{
			ERROR("param is err minorType = %d\n",minorType);
			return -1;
		}
        MPU_PtzAllAction(JP_PTZ_CONTORL, it->second, buf2, NULL);
        break;
    }
    default:
        break;
    }
    return ret;
}
#endif

int mpu_init()
{
	int ret;
	char addr[32] = {0};
    ret = AVL_Init();
	if(ret != 0)
	{
		ERROR("avl init is fail");
	}
    NET_Init();

    while(1)
    {
        if(!get_localip("eth0", addr))
        {
            break;
        }
        sleep(1);
    }
#ifdef HTTP_UPLOAD
	ret = NetworkInit(addr);
	if(ret != 0)
	{
		ERROR("NET WORK init is fail");
	}
#endif
	ret = mpu_ciu_config_init();
	if(ret != 0)
	{
		ERROR("mpu_ciu_init is fail");
	}
	
	INFO("mpu init is success\n");

#ifdef ONVIF_PRO

    OnvifDevInfo dev_info;
    ret =  OnvifInit(addr, dev_info, MPU_OnvifOperCb);
    if(ret != 0)
    {
        ERROR("network init is err\n");
        return -1;
    }
    DEBUG("onvif is init success\n");
#endif
    return 0;
}

int mpu_uninit()
{
#ifdef ONVIF_PRO
    OnvifUnInit();
#endif
	mpu_ciu_config_deinit();
#ifdef HTTP_UPLOAD
	NetworkUnInit();
#endif
	MPU_CALL(AVL_UnInit());
	return 0;
}



