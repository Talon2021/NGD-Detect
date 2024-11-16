
#ifndef _CCODER_H
#define _CCODER_H
#include "jpsdk.h"
#include "CConfig.h"
typedef enum 
{
	A_CODEC_TYPE_AAC,
	A_CODEC_TYPE_G711A,
	A_CODEC_TYPE_G711U,
	A_CODEC_TYPE_G726,
}AUDIO_ENCTYPE;

typedef struct video_codec_st
{
    VideoEncodeType_t encoderType;
    int bitRate;
    int farmeRate;
    int profileLever;
    int encWidth;
    int encHeight;
    int gop;
}video_codec;

typedef struct audio_codec_st
{
    AUDIO_ENCTYPE encoderType;
    int sampleRate;
    int NBSamples;
    int channel;
}audio_codec;

typedef struct time_config_st
{
    int TimeZone;
    int TimeFormat;
    int SummerTimeEnable;
}time_cfg;


class CCoder
{
private:
    void *handle;
    int m_Channel;
    video_codec m_vEncType;
    audio_codec m_aEncType;
    int init;
    char m_Language[32];
    time_cfg m_TimeCfg;

    CConfig *m_CConfig;
    int LoadParam();
public:
    CCoder(void *handle);
    ~CCoder();

    int Init();

    int UnInit();

    int GetVideoCoderEncParam(video_codec *Enc);

    int SetVideoCoderEncParam(video_codec Enc);

    int GetAudioCoderEncParam(audio_codec *Enc);

    int SetAudioCoderEncParam(audio_codec Enc);

    int SetLanguage(char *language);

    int GetLanguage(char *language);

    int SetTimeConfig(time_cfg cfg);

    int GetTimeConfig(time_cfg *cfg);
};



#endif