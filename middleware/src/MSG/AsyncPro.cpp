#include "AsyncPro.h"
#include <functional>

void MessageHandler::receiveMessages() 
{
    while (!stop_flag) {
        MessagePro msg;
        {
            std::unique_lock<std::mutex> lock(msg_mutex);
            
            //接收消息
            // if (message_queue.empty()) {
            //     lock.unlock();
            //     std::this_thread::sleep_for(std::chrono::milliseconds(100));
            //     continue;
            // }
        }

        {
            std::shared_ptr<ConditionData> condition_data;
            {
                std::lock_guard<std::mutex> lock(map_mutex);
                if (condition_map.find(msg.id) != condition_map.end()) {
                    condition_data = condition_map[msg.id];
                }
            }
            auto message = std::make_shared<receMessage>();
            if (condition_data) {
                std::lock_guard<std::mutex> lock(condition_data->mutex);
                condition_data->processed = true;
               
                //组装数据
                receive_inline_queue.push(message);
                condition_data->cv.notify_one();
            }
            else
            {
                receive_msg_queue.push(message);
            }
        }
    }
}



MessageHandler::MessageHandler() 
{
    stop_flag  = false;
}

MessageHandler::~MessageHandler() 
{
    stop_flag = true;
    if (receiver_thread.joinable()) {
        receiver_thread.join();
    }
}

MessageHandler *MessageHandler::m_messageHandle = NULL;

MessageHandler *MessageHandler::getInstance()
{
    if(m_messageHandle == NULL)
    {
        m_messageHandle = new MessageHandler();
        m_messageHandle->Init();
    }

    return m_messageHandle;
}

int MessageHandler::sendMessage(MessagePro &msg, std::shared_ptr<receMessage> out_msg)
{
    int id = msg.id;
    {
        std::lock_guard<std::mutex> lock(msg_mutex);
        //发送消息
    }
    if(msg.is_wait)
    {
        std::shared_ptr<ConditionData> condition_data;
        {
            std::lock_guard<std::mutex> lock(map_mutex);
            if (condition_map.find(id) == condition_map.end()) {
                condition_map[id] = std::make_shared<ConditionData>();
            }
            condition_data = condition_map[id];
            condition_map[id]->processed = false;
        }

        {
            std::unique_lock<std::mutex> lock(condition_data->mutex);
            if (condition_data->cv.wait_for(lock, std::chrono::milliseconds(msg.time_out_ms), [&] { return  condition_map[id]->processed; })) 
            {
                receive_inline_queue.pop(out_msg);
                return id;
            } 
        }
    }
    return 0;
}

void MessageHandler::startReceiver() 
{
    receiver_thread = std::thread(std::bind(&MessageHandler::receiveMessages, this));
}

void MessageHandler::stopReceiver() 
{
    stop_flag = true;
    if (receiver_thread.joinable()) {
        receiver_thread.join();
    }
}

int MessageHandler::UnInit()
{
    stopReceiver();
    return 0;
}

int MessageHandler::Init()
{
    receive_msg_queue.set_maxsize(64);
    receive_msg_queue.set_block(true);
    receive_inline_queue.set_maxsize(64);
    receive_inline_queue.set_block(true);
    startReceiver();    
    return 0;
}

int MessageHandler::getMessage(std::shared_ptr<receMessage> out_msg)
{
    receive_msg_queue.pop(out_msg);
    return 0;
}