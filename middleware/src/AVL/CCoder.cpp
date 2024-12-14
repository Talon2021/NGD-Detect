
#include "CCoder.h"
#include "sdk_log.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/time.h>

#define TIMECFG_SECTION     "Time_Cfg"
#define TIME_ZONE_KEY       "time_zone"
#define TIME_DISPLAYFORMAT  "time_format"
#define TIME_SUMMERTIMEENABLE "time_summer_enable"

#define SYSTIME_INFO_SECTION    "System_info"
#define SYSTIME_LANGUAGE        "system_language"
int CCoder::LoadParam()
{
    char language[32] = {0};
    time_cfg cfg_time;
    m_CConfig->GetValue(SYSTIME_INFO_SECTION, SYSTIME_LANGUAGE, "Chinese", language, sizeof(language));
    SetLanguage(language);

    cfg_time.TimeZone = m_CConfig->GetValue(TIMECFG_SECTION, TIME_ZONE_KEY, (long)480);     //8 *60
    cfg_time.TimeFormat = m_CConfig->GetValue(TIMECFG_SECTION, TIME_ZONE_KEY, (long)1);
    cfg_time.SummerTimeEnable = m_CConfig->GetValue(TIMECFG_SECTION, TIME_ZONE_KEY, (long)0);
    SetTimeConfig(cfg_time);
    return 0;
}
CCoder::CCoder(void *handle, int ch)
{
    memset(&m_vEncType, 0, sizeof(video_codec));
    memset(&m_aEncType, 0, sizeof(audio_codec));
    handle = handle;
    init = 0;
}


CCoder::~CCoder()
{
    UnInit();
}

int CCoder::Init()
{
    m_vEncType.encoderType = H264;
    m_vEncType.bitRate = 2048 * 1024;
    m_vEncType.farmeRate = 30;
    m_vEncType.profileLever = 88;
    m_vEncType.encWidth = 640;
    m_vEncType.encHeight = 512;
    m_vEncType.gop = 30;

    m_aEncType.encoderType = A_CODEC_TYPE_G711A;
    m_aEncType.sampleRate = 16000;
    m_aEncType.NBSamples = 1152;
    m_aEncType.channel = 1;

    init = 1;
    m_CConfig = CConfig::GetInstance();
    LoadParam();
    return 0;
}

int CCoder::UnInit()
{
    return 0;
}

int CCoder::GetVideoCoderEncParam(video_codec *Enc)
{
    if(!init)
    {
        ERROR("CODER IS NOT INIT \n");
        return -1;
    }
    memcpy(Enc, &m_vEncType, sizeof(video_codec));
    return 0;
}

int CCoder::SetVideoCoderEncParam(video_codec Enc)
{
    if(!init)
    {
        ERROR("CODER IS NOT INIT \n");
        return -1;
    }
    memcpy(&m_vEncType, &Enc, sizeof(video_codec));
    return 0;
}

int CCoder::GetAudioCoderEncParam(audio_codec *Enc)
{
    if(!init)
    {
        ERROR("CODER IS NOT INIT \n");
        return -1;
    }
    memcpy(Enc, &m_aEncType, sizeof(audio_codec));
    return 0;
}

int CCoder::SetAudioCoderEncParam(audio_codec Enc)
{
    if(!init)
    {
        ERROR("CODER IS NOT INIT \n");
        return -1;
    }
    memcpy(&m_aEncType, &Enc, sizeof(video_codec));
    return 0;
}

int CCoder::SetLanguage(char *language)
{
    if(!init)
    {
        ERROR("CODER IS NOT INIT \n");
        return -1;
    }
    memcpy(m_Language, language, strlen(language));
    m_CConfig->SetValue(SYSTIME_INFO_SECTION, SYSTIME_LANGUAGE, m_Language);
    return 0;
}

int CCoder::GetLanguage(char *language)
{
    if(!init)
    {
        ERROR("CODER IS NOT INIT \n");
        return -1;
    }
    memcpy(language, m_Language, strlen(m_Language));
    return 0;
}

int CCoder::SetTimeConfig(time_cfg cfg)
{
    if(!init)
    {
        ERROR("CODER IS NOT INIT \n");
        return -1;
    }
    memcpy(&m_TimeCfg, &cfg, sizeof(time_cfg));
    m_CConfig->SetValue(TIMECFG_SECTION, TIME_ZONE_KEY, (long)m_TimeCfg.TimeZone);
    m_CConfig->SetValue(TIMECFG_SECTION, TIME_DISPLAYFORMAT, (long)m_TimeCfg.TimeFormat);
    m_CConfig->SetValue(TIMECFG_SECTION, TIME_SUMMERTIMEENABLE, (long)m_TimeCfg.SummerTimeEnable);
    return 0;
}

int CCoder::GetTimeConfig(time_cfg *cfg)
{
    if(!init)
    {
        ERROR("CODER IS NOT INIT \n");
        return -1;
    }
    memcpy(cfg, &m_TimeCfg, sizeof(time_cfg));
    return 0;
}

int CCoder::SetTime(unsigned int time)
{
    struct timeval tv;
    tv.tv_sec = time + (m_TimeCfg.TimeZone * 60);  
    tv.tv_usec = 0;

    if (settimeofday(&tv, NULL) == -1) 
    {
        ERROR("settimeofday is err\n");
        return -1;
    }
    return 0;
}
