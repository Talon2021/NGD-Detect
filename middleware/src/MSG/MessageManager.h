#ifndef _MESSAGE_MANAGER_H_
#define _MESSAGE_MANAGER_H_

#include "AsyncPro.h"
#define MAX_MSGHANDLE   1
typedef struct msg_alarmInfo
{
    int type;
    int state;
    unsigned long time;
}msg_alarmInfo;


typedef int (*MSG_ALARMCALLBACK)(msg_alarmInfo *info);

class MessageManager
{

public:
    MessageManager(/* args */);

    ~MessageManager();

    static MessageManager *getInstance();

    int Init();

    int UnInit();

    int MSG_SetAlarmCallback(MSG_ALARMCALLBACK cb);

    int MSG_SendMessage(int ch, int code, std::string data, int wait_flag, int time_out, std::shared_ptr<receMessage> out_msg);
private:

    void GetMessageThread(int arg); 

    MessageHandler *m_messagehandle[MAX_MSGHANDLE];

    MSG_ALARMCALLBACK m_alarmCallback;

    static MessageManager *m_messageManagerHandle;
    
    std::thread m_GetMsgthread[MAX_MSGHANDLE];
    
    
};



#endif