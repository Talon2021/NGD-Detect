#include "mpu_ciu_preview.h"

#include "common.h"
#include "CChannelManager.h"
#include "mpu_ciu_config.h"
#ifdef RTSP_PREVIEW
#include "rtsp_demo.h"
static rtsp_demo_handle m_rtst_handle = NULL;
static rtsp_session_handle m_rtsp_session_handle = NULL;
#endif

static int CIU_AvCallBack(int nCh, unsigned char *Buff, int nBuffLen)
{
    H264DATAPACKETHEADER *p_head =  (H264DATAPACKETHEADER *)Buff;
    if(p_head->byLoadType == H264)
    {
#ifdef RTSP_PREVIEW
        rtsp_tx_video(m_rtsp_session_handle, (const uint8_t *)(Buff+sizeof(H264DATAPACKETHEADER)), p_head->uOverloadLen,
                  p_head->uShowTime);
        rtsp_do_event(m_rtst_handle);
#endif
    }
    else if(p_head->byLoadType == H265)
    {
        
    }
    return 0;
}

void CIU_RegisterPreview()
{
#ifdef RTSP_PREVIEW
    m_rtst_handle = create_rtsp_demo(554);
    m_rtsp_session_handle = create_rtsp_session(m_rtst_handle, "/live/main_stream");
    rtsp_set_video(m_rtsp_session_handle, RTSP_CODEC_ID_VIDEO_H264, NULL, 0);
    rtsp_sync_video_ts(m_rtsp_session_handle, rtsp_get_reltime(), rtsp_get_ntptime());
    CChannelManager *pMgr = CChannelManager::GetInstanceHandle();
    if (NULL != pMgr)
	{
		pMgr->SetAVCallBack(CIU_AvCallBack);
	}
#endif 
}