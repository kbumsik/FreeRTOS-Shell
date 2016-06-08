#include <stdint.h>
#include "frs_user_tasks.h"

void frs_user_top(void *parameters)
{
    char buf[256];
    vTaskGetRunTimeStats(buf);
    printf("Task\t Abs Time \t Time\n");
    printf(buf);

    /* kill task */
    frs_task_kill(NULL);
}