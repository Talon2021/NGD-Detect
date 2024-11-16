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


 typedef struct Message_st 
 {
    int id;
    std::string data; 
    int time_out_ms;
    int is_wait;
}MessagePro;

typedef struct recvMessage_st
{
    std::string recv_data;
}receMessage;

struct ConditionData {
    std::mutex mutex;                    
    std::condition_variable cv;         
    bool processed = false;             
};

class MessageHandler {
public:
    static MessageHandler *getInstance();

    MessageHandler(const MessageHandler&) = delete;
    MessageHandler& operator=(const MessageHandler&) = delete;

    int sendMessage(MessagePro &msg, std::shared_ptr<receMessage> out_msg);

    void startReceiver();
    void stopReceiver();

    int Init();
    int UnInit();
    
    int getMessage(std::shared_ptr<receMessage> out_msg);

private:
    MessageHandler();
    ~MessageHandler();

    void receiveMessages();
    SafeQueuee<std::shared_ptr<receMessage>> receive_msg_queue;
    SafeQueuee<std::shared_ptr<receMessage>> receive_inline_queue;
    std::unordered_map<int, std::shared_ptr<ConditionData>> condition_map;
    std::mutex msg_mutex;
    std::mutex map_mutex;
    std::thread receiver_thread;
    bool stop_flag;

    static MessageHandler *m_messageHandle;
};



#endif