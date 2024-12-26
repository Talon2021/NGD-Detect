#include "yolo_rknn.h"
#include <unistd.h>
#include <string.h>
#include "meida_common.h"
int Detect_Task::GetFrameDetectThread()
{

    while(!m_GetFrameExit)
    {
        algor_frame *frame;
        m_DetectFrame->pop(frame);
        m_ThreadPool->enqueue([this,frame](){
            this->taskModelReson(frame);
        });
        usleep(10 *1000);
    }
    return 0;
}

int Detect_Task::GetFreeHandle()
{
    std::lock_guard<std::mutex> lock(m_HandleMutex);
    for(int i = 0; i < MAX_PTHREAD_NUM; i++)
    {
        if(m_HandleValid[i] == 0)
        {
            m_HandleValid[i] = 1;
            return i;
        }
    }
    return -1;
}

int Detect_Task::SerFreeHandle(int index)
{
    std::lock_guard<std::mutex> lock(m_HandleMutex);
    m_HandleValid[index] = 0;
    return 0;
}

int Detect_Task::taskModelReson(algor_frame *frame)
{
    int ret = 0;
    int index = GetFreeHandle();
    Detect_Result *od_results= (Detect_Result *)calloc(1, sizeof(Detect_Result));
    if(od_results == NULL)
    {
        fprintf(stderr, "od_results alloc memory is fail \n");
        SerFreeHandle(index);
        return -1;
    }
    ret =  inference_yolov8_seg_model(&m_Handle[index], &(frame->src_frame), &od_results->result);
    if(ret != 0)
    {
        fprintf(stderr, " yolov8 Run is fail ret = %d ", ret);
        goto free_result;
    }
    //SDK_DBG("result = count = %d \n", od_results->result.count);
    od_results->userdata = frame->user_data;
    od_results->frame_id = frame->frame_id;
    m_DetectResult->push(od_results);
    SerFreeHandle(index);
    return 0;
free_result:
    free(od_results); 
    SerFreeHandle(index);
    return -1;
}

Detect_Task::Detect_Task()
{
    memset(m_HandleValid, 0, sizeof(int) * MAX_PTHREAD_NUM);
    m_GetFrameExit = 0;
}

Detect_Task::~Detect_Task()
{
    UnInit();
}

int Detect_Task::Init(const char *model_path)
{
    int i = 0;
    init_post_process();
    for(i = 0; i < MAX_PTHREAD_NUM; i++)
    {
        init_yolov8_seg_model(model_path,  &m_Handle[i]);
    }
    m_DetectFrame = new SafeQueuee<algor_frame*>(true, 16);
    m_DetectResult = new SafeQueuee<Detect_Result*>(true, 32);
    m_ThreadPool = new ThreadPool(MAX_PTHREAD_NUM);
    m_GetFrameExit = 0;
    m_GetFrameThread = new std::thread(std::bind(&Detect_Task::GetFrameDetectThread,this));
    return 0;
}

int Detect_Task::UnInit()
{
    m_GetFrameExit = 1;
    m_GetFrameThread->join();
    delete m_GetFrameThread;
    m_GetFrameThread = NULL;

    delete m_DetectFrame; 
    delete m_DetectResult;
    delete m_ThreadPool;
    return 0;
}

int Detect_Task::Detect_task_SendFrame(algor_frame *frame)
{
    algor_frame *detect_frame = (algor_frame *)calloc(1,sizeof(algor_frame));
    if(detect_frame == NULL)
    {
        fprintf(stderr,"alloc memory is err\n");
        return -1;
    }
    memcpy(detect_frame, frame, sizeof(algor_frame));
    detect_frame->user_data = frame->user_data;
    detect_frame->src_frame.virt_addr = frame->src_frame.virt_addr;
    m_DetectFrame->push(detect_frame);
    return 0;
}

int Detect_Task::Detect_task_GetResult(Detect_Result **result)
{
    m_DetectResult->pop(*result);
    return 0;
}

int Detect_Task::Detect_task_result_Delete(Detect_Result *result)
{
    if(result == NULL)
    {
        fprintf(stderr, "delete result is err \n");
        return -1;
    }
    if(result->result.results_seg[0].seg_mask && result->result.count >= 1)
    {
        free(result->result.results_seg[0].seg_mask);
        result->result.results_seg[0].seg_mask = NULL;
    }
    free(result);
    return 0;
}
