#ifndef _FRS_QUEUE_H_
#define _FRS_QUEUE_H_

#include "frs_queue.h"
#include "FreeRTOS-Shell.h"
#include "queue.h"
#include <stdint.h>

typedef void * frs_queue_h;

inline frs_queue_h frs_queue_create(int queue_len, size_t item_size)
{
    frs_queue_h result;
    result = (frs_queue_h)xQueueCreate((UBaseType_t)queue_len, (UBaseType_t)item_size);
    if (NULL == result)
    {
        frs_error("not enough memory.\n");
        return NULL;
    }
    return result;
}

inline int frs_queue_send(frs_queue_h queue, const void *item_to_send, int time_to_wait)
{
    int result;
    result = (int)xQueueSend((QueueHandle_t)queue, item_to_send, time_to_wait);
    if (pdPASS != result)
    {
        frs_error("Queue is full during %d\n", time_to_wait);
        return 1;
    }
    return 0;
}

inline int frs_queue_send_isr(frs_queue_h queue, const void *item_to_send,
    int *higher_task_unblocked)
{
    int result;
    result = (int)xQueueSendFromISR((QueueHandle_t)queue, item_to_send,
        (BaseType_t *)higher_task_unblocked);
    if (pdPASS != result)
    {
        frs_error("Queue is full \n");
        return 1;
    }
    return 0;
}

inline int frs_queue_recv(frs_queue_h queue, void *buf_to_recv, int time_to_wait)
{
    int result;
    result = (int)xQueueReceive((QueueHandle_t)queue, buf_to_recv, time_to_wait);
    if (pdPASS != result)
    {
        frs_error("no message received during %d\n", time_to_wait);
        return 1;
    }
    return 0;
}

inline int frs_queue_recv_isr(frs_queue_h queue, void *buf_to_recv,
    int *higher_task_unblocked)
{
    int result;
    result = (int)xQueueReceiveFromISR((QueueHandle_t)queue, buf_to_recv,
        (BaseType_t *)higher_task_unblocked);
    if (pdPASS != result)
    {
        frs_error("Queue is empty \n");
        return 1;
    }
    return 0;
}

inline int frs_yield_from_isr(int yield_or_not)
{
    portYIELD_FROM_ISR(yield_or_not);
}



#endif // !_FRS_QUEUE_H_