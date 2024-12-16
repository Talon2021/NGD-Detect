#ifndef _ASYNC_PRO_H
#define _ASYNC_PRO_H


#include <mutex>
#include <condition_variable>
#include <thread>
#include <queue>
#include <unordered_map>
#include <string>
#include <memory>
#include "safe_quene.hpp"
#include "mqtt_client.h"


 typedef struct Message_st 
 {
    int id;
    std::string data; 
    int time_out_ms;
    int is_wait;
}MessagePro;

typedef struct recvMessage_st
{
    std::string send_name;
    std::string recv_data;
    recvMessage_st(const std::string& name, const std::string& data)
        : send_name(name), recv_data(data) {}
}receMessage;

struct ConditionData {
    std::mutex mutex;                    
    std::condition_variable cv;         
    bool processed = false;             
};

class MessageHandler {
public:
     MessageHandler();
    ~MessageHandler();
    int sendMessage(MessagePro &msg, std::shared_ptr<receMessage> &out_msg);

    void startReceiver();
    void stopReceiver();

    int Init();
    int UnInit();
    
    int getMessage(std::shared_ptr<receMessage> out_msg);

private:
   

    void receiveMessages();
    SafeQueuee<std::shared_ptr<receMessage>> receive_msg_queue;
    SafeQueuee<std::shared_ptr<receMessage>> receive_inline_queue;
    std::unordered_map<int, std::shared_ptr<ConditionData>> condition_map;
    std::mutex msg_mutex;
    std::mutex map_mutex;
    std::thread receiver_thread;
    bool stop_flag;

    mqtt_client *m_mqtt_handle;
};



#endif