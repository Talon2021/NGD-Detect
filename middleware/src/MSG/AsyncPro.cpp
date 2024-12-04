#include "AsyncPro.h"
#include <functional>
#include "nlohmann_json.hpp"
#include "sdk_log.h"
#include <unistd.h>
using json_t = nlohmann::json;
void MessageHandler::receiveMessages() 
{
    int ret = 0;
    char tmp_msg[2048] = {0};
    char tmp_topic[64] = {0};
    while (!stop_flag) {
        MessagePro msg;
        {
            //printf("======================= line = %d\n",__LINE__);
            //std::unique_lock<std::mutex> lock(msg_mutex);
            ret = mqtt_receive(m_mqtt_handle, 200);
        }
        if(ret == MQTT_SUCCESS)
        {
            printf("received Topic=%s, Message=%s message_len = %d\n", m_mqtt_handle->received_topic, m_mqtt_handle->received_message,m_mqtt_handle->received_message_len);
            memcpy(tmp_msg, m_mqtt_handle->received_message, m_mqtt_handle->received_message_len);
            memcpy(tmp_topic, m_mqtt_handle->received_topic, m_mqtt_handle->received_topic_len);
            json_t j;
            j = json_t::parse(tmp_msg, nullptr,false);
            if(j.is_discarded())
            {
                ERROR("json::parse error!\n");
                continue;
            }
            if(!j["code"].is_null())
            {
                msg.id = j["code"];
            }
        }
        else if(ret == MQTT_DISCONNECTED)
        {
            ERROR("mqtt is disconnect\n");
            mqtt_connect(m_mqtt_handle, NULL, NULL);
            mqtt_subscribe(m_mqtt_handle, "pre", QOS_EXACTLY_ONCE);
            continue;
        }
        else
        {
            usleep(20 * 1000);
            continue;
        }

        {
            std::shared_ptr<ConditionData> condition_data;
            {
                std::lock_guard<std::mutex> lock(map_mutex);
                if (condition_map.find(msg.id) != condition_map.end()) {
                    condition_data = condition_map[msg.id];
                }
            }
            auto message = std::make_shared<receMessage>(receMessage(tmp_topic, tmp_msg));
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
        usleep(20 * 1000);
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

int MessageHandler::sendMessage(MessagePro &msg, std::shared_ptr<receMessage> out_msg)
{
    int id = msg.id;
    {
        //std::lock_guard<std::mutex> lock(msg_mutex);
        mqtt_publish(m_mqtt_handle, "host", const_cast<char*>(msg.data.c_str()), QOS_EXACTLY_ONCE);
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
                printf("sed is success line = %d\n", __LINE__);
                receive_inline_queue.pop(out_msg);
                return id;
            } 
        }
    }
    printf("sed is success line = %d\n", __LINE__);
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
    
    int ret = 0;   
    m_mqtt_handle  = mqtt_new("127.0.0.1", 1883, "xh_client");
    if(m_mqtt_handle == NULL)
    {
        ERROR("mqtt new is err \n");
        return -1;
    }
    ret = mqtt_connect(m_mqtt_handle, NULL, NULL);
    if(ret != MQTT_SUCCESS)
    {
        ERROR("mqtt connect is err \n");
        return -1;
    }
    ret = mqtt_subscribe(m_mqtt_handle, "pre", QOS_EXACTLY_ONCE);
    ret = mqtt_subscribe(m_mqtt_handle, "vo", QOS_EXACTLY_ONCE);
    startReceiver();    
    
    return 0;
}

int MessageHandler::getMessage(std::shared_ptr<receMessage> out_msg)
{
    receive_msg_queue.pop(out_msg);
    return 0;
}