#include "detect_api.h"
#include "yolo_rknn.h"
#include "meida_common.h"
typedef struct detect_alg_handle_st
{
    Detect_Task *m_detect_handle;
    int m_confidence;
} detect_alg_handle;

static detect_alg_handle *g_handle = NULL;

void *detect_init_handle_v2(DETECT_HANDLE_OPTPARAM_S param)
{
    g_handle = (detect_alg_handle *)calloc(1, sizeof(detect_alg_handle));
    if(g_handle == NULL)
    {
        SDK_ERR("init is err\n");
        return NULL;
    }
    g_handle->m_confidence = param.confidence;

    g_handle->m_detect_handle = new Detect_Task();
    if(g_handle->m_detect_handle == NULL)
    {
        SDK_ERR("init is err\n");
        return NULL;
    }
    g_handle->m_detect_handle->Init(param.model_path);

    return g_handle;
}

int detect_uninit_handle_v2(void **handle)
{
    if(*handle == NULL)
    {
        SDK_ERR("parma is err \n");
        return -1;
    }
    
    free(*handle);
    *handle = NULL;

    return 0;
}

int detect_send_frame_task(void *handle, DETECT_FRAME *frame)
{
    int ret = 0;
    if(handle == NULL)
    {
        SDK_ERR("handle is err \n");
        return -1;
    }
    detect_alg_handle *tmp_handle  = (detect_alg_handle *)handle;
    algor_frame tmp_frame;
    tmp_frame.src_frame.width = frame->width;
    tmp_frame.src_frame.height = frame->height;
    tmp_frame.src_frame.width_stride = frame->width;
    tmp_frame.src_frame.height_stride = frame->height;
    tmp_frame.src_frame.format = frame->format;
    tmp_frame.src_frame.virt_addr = frame->frame_date;
    tmp_frame.src_frame.size = frame->width * frame->width * 3;

    tmp_frame.user_data = frame->user_data;

    ret = tmp_handle->m_detect_handle->Detect_task_SendFrame(&tmp_frame);

    return ret;
}

int detect_recv_task_result(void *handle, DETECT_TASK_RESULT *result)
{
    int ret = 0;
    int i = 0;
    if(handle == NULL || result == NULL)
    {
        SDK_ERR("handle is err \n");
        return -1;
    }
    detect_alg_handle *tmp_handle  = (detect_alg_handle *)handle;
    Detect_Result *tmp_result;
    ret = tmp_handle->m_detect_handle->Detect_task_GetResult(&tmp_result);

    memset(result, 0, sizeof(DETECT_TASK_RESULT));
    result->gas_list = calloc(1, sizeof(DETECT_TASK_COMMON) * tmp_result->result.count );
    result->gas_mask_list = calloc(1, sizeof(DETECT_TASK_SETMASK) * tmp_result->result.count);

    for(i = 0; i < tmp_result->result.count; i++)
    {
        if(tmp_result->result.results[i].cls_id == 0 && tmp_result->result.results[i].prop >= tmp_handle->m_confidence)
        {
            result->gas_list[result->gas_list_size].confidence = tmp_result->result.results[i].prop;
            result->gas_list[result->gas_list_size].rect.left = tmp_result->result.results[i].box.left;
            result->gas_list[result->gas_list_size].rect.right = tmp_result->result.results[i].box.right;
            result->gas_list[result->gas_list_size].rect.top = tmp_result->result.results[i].box.top;
            result->gas_list[result->gas_list_size].rect.bottom = tmp_result->result.results[i].box.bottom;
            result->gas_list_size++;
        }
    }
    if(result->gas_list_size > 0)
    {
        result->gas_mask_list[0].seg_mask = calloc(1, 640 * 512);

        memcpy(result->gas_mask_list[0].seg_mask, tmp_result->result.results_seg[0].seg_mask, 640 * 512);
    }
    result->user_data = tmp_result->userdata;
    tmp_handle->m_detect_handle->Detect_task_result_Delete(tmp_result);

    return ret;
}

int detect_reuslt_delete(void *p)
{
    if(p == NULL)
    {
        SDK_ERR("handle is err \n");
        return -1;
    }
    DETECT_TASK_RESULT *result = (DETECT_TASK_RESULT *)p;
    if(result->gas_mask_list[0].seg_mask)
    {
        free(result->gas_mask_list[0].seg_mask);
    }
    if(result->gas_list)
    {
        free(result->gas_list);
    }
    if(result->gas_mask_list)
    {
        free(result->gas_mask_list);
    }
    
    return 0;
}
