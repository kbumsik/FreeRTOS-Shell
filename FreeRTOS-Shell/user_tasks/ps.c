#include <stdint.h>
#include "frs_user_tasks.h"

void frs_user_ps(void *parameters)
{
    char buf[256];
    vTaskList(buf);
    printf("Name\t State \t Priority\t Stack\t Num\n");
    printf(buf);

    /* kill task */
    frs_task_kill(NULL);
}