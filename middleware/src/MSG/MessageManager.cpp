#include "MessageManager.h"
#include <unistd.h>
#include <stdlib.h>
#include <string.h>
#include <functional>
#include "sdk_log.h"
MessageManager::MessageManager()
{
    memset(m_messagehandle, 0, sizeof(MessageHandler *) * MAX_MSGHANDLE);
}

MessageManager::~MessageManager()
{

}
MessageManager *MessageManager::m_messageManagerHandle = NULL;

MessageManager *MessageManager::getInstance()
{
    if(m_messageManagerHandle == NULL)
    {
        m_messageManagerHandle = new MessageManager();
        m_messageManagerHandle->Init();
    }

    return m_messageManagerHandle;
}

void MessageManager::GetMessageThread(int arg)
{
    int id = arg;
    printf("id = %d \n", id);
    while (1)
    {
        {
            std::shared_ptr<receMessage> out_msg;
            m_messagehandle[id]->getMessage(out_msg);
            // DEBUG("name = %s data = %s \n", out_msg->send_name.c_str(), out_msg->recv_data.c_str());
        }


        usleep(10 * 1000);
    }
    
}

int MessageManager::Init()
{
    m_messagehandle[0] = new MessageHandler();
    m_messagehandle[0]->Init();

    m_GetMsgthread[0] = std::thread(std::bind(&MessageManager::GetMessageThread, this, 0));

    return 0;
}

int MessageManager::UnInit()
{
    return 0;
}

int MessageManager::MSG_SetAlarmCallback(MSG_ALARMCALLBACK cb)
{
    m_alarmCallback = cb;
    return 0;
}

int MessageManager::MSG_SendMessage(int ch, int code, std::string data, int wait_flag, int time_out, std::shared_ptr<receMessage> out_msg)
{
    int ret = 0;
    MessagePro msg;
    msg.id = code;
    msg.is_wait = wait_flag;
    msg.time_out_ms = time_out;
    msg.data = data;
    m_messagehandle[ch]->sendMessage(msg, out_msg);
    return ret;
}
