#ifndef _LGC_ALGO_H
#define _LGC_ALGO_H


#include <pthread.h>

#include "meida_common.h"
#include "jpsdk.h"
#include "mpp_venc.h"
#ifdef __cplusplus
extern "C" {
#endif


typedef struct PlatformChannelContext_S{
    
    ThreadInfo_t m_GasFrameSendThread;
    ThreadInfo_t m_GasResultRecvThread;
    void *m_Detect_handle;
    GasDetectResult_CALLBACK m_GasResultCallBack;
    void *m_GasResultUserData;
    int m_AlgoPicWidth;
    int m_AlgoPicHeight;
}CPlatformChannelContext;

int LGC_ALGO_Init(void);

int LGC_ALGO_DeInit(void);

int LGC_ALGO_RegisterGasResultCb(GasDetectResult_CALLBACK callback, void *userdata);

#ifdef __cplusplus
} /* extern "C" */
#endif

#endif