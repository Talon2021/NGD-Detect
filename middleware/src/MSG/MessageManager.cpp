#include "MessageManager.h"
#include <unistd.h>
#include <stdlib.h>
#include <string.h>
#include <functional>
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
    while (1)
    {
        std::shared_ptr<receMessage> out_msg;
        m_messagehandle[id]->getMessage(out_msg);


        usleep(10 * 1000);
    }
    
}

int MessageManager::Init()
{
    m_messagehandle[0] = MessageHandler::getInstance();

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


