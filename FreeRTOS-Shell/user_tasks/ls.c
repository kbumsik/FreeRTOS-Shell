#include "FreeRTOS-Shell.h"
#include "frs_user_tasks.h"

void frs_user_ls(void *parameters)
{
    frs_task_print_flist();

    /* kill task */
    frs_task_kill_name(NULL);
}