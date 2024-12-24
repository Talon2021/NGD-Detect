#ifndef YOLO_RKNN_H
#define YOLO_RKNN_H

#define MAX_PTHREAD_NUM         3
#include <mutex>
#include <condition_variable>
#include "rknn_unitl.h"
#include "safe_quene.hpp"
#include "ThreadPool.h"

class EnhancedInterfaceLock {
public:
    void sharedMethod() {
        startSharedAccess();
        // 执行共享读取操作
        // 示例输出，实际操作替换这里
        endSharedAccess();
    }

    void exclusiveMethod() {
        mtx.lock(); // 独占锁定
        // 执行独占写入操作
        // 示例输出，实际操作替换这里
        mtx.unlock(); // 解锁
    }

private:
    std::mutex mtx; // 用于写操作的互斥锁
    std::mutex count_mutex; // 用于保护reader_count的互斥锁
    int reader_count = 0; // 跟踪当前读者数量

    // 开始共享访问的辅助函数
    void startSharedAccess() {
        std::lock_guard<std::mutex> lk(count_mutex);
        if (reader_count++ == 0) {
            mtx.lock(); // 如果这是第一个读者，则锁定mtx
        }
    }

    // 结束共享访问的辅助函数
    void endSharedAccess() {
        std::lock_guard<std::mutex> lk(count_mutex);
        if (--reader_count == 0) {
            mtx.unlock(); // 如果所有读者都完成了读操作，则解锁mtx
        }
    }
};

typedef struct _algor_frame
{
    image_buffer_t src_frame;   //放检测帧数据
    const void *user_data;  //放原始帧数据，帧号，等
}algor_frame;

typedef struct Detect_Result
{
    object_detect_result_list result;
    const void *userdata;
}Detect_Result;

class Detect_Task
{
private:
    rknn_app_context_t m_Handle[MAX_PTHREAD_NUM];
    int m_HandleValid[MAX_PTHREAD_NUM];
    SafeQueuee<algor_frame*> *m_DetectFrame;

    SafeQueuee<Detect_Result*> *m_DetectResult;

    ThreadPool *m_ThreadPool;
    std::thread *m_GetFrameThread;
    int m_GetFrameExit;

    std::mutex m_HandleMutex;

    EnhancedInterfaceLock m_SendFrameLock;
    EnhancedInterfaceLock m_GetResultLock;

    int GetFrameDetectThread();
    int GetFreeHandle();
    int SerFreeHandle(int index);
    int taskModelReson(algor_frame *frame);
public:
    Detect_Task();
    ~Detect_Task();

    int Init(const char *model_path);
    int UnInit();

    int Detect_task_SendFrame(algor_frame *frame);
    int Detect_task_GetResult(Detect_Result **result);
    int Detect_task_result_Delete(Detect_Result *result);
};


#endif