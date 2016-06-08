#include "FreeRTOS-Shell.h"
#include "frs_user_tasks.h"

void frs_user_ls(void *parameters)
{
    struct frs_arg_t* param = (struct frs_arg_t *)parameters;
    frs_task_print_flist();
}