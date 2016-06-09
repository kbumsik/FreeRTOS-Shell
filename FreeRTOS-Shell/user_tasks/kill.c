#include "FreeRTOS-Shell.h"
#include "frs_user_tasks.h"
#include <stdlib.h>
#include <errno.h>

#define print_help(name)  \
            printf("%s: usage: kill [-n] target-name or tid\n", name);

void frs_user_kill(void *parameters)
{
    int target_index = 0;
    struct {
        char name_or_tid;
    }flags = { 0 };

    /* get the parameters */
    struct frs_arg_t* param = (struct frs_arg_t *)parameters;

    for (int i = 1; i < param->argc; i++)
    {
        char tmp = param->argv[i][0];
        switch (tmp)
        {
        case '-':
            if ('-' == (param->argv[i][1]))
            {
                goto end_task_err;
            }
            for (int j = 1; '\0' != param->argv[i][j]; j++)
            {
                char arg_char = param->argv[i][j];
                switch (arg_char)
                {
                case 'n':
                    flags.name_or_tid = 1;
                    break;
                default:
                    goto end_task_err;
                }
            }
            break;
        default:
            target_index = i;
            break;
        }
    }
    if (0 == target_index)
    {
        frs_error("No target specified,\n");
        goto end_task_err;
    }
    if (0 != flags.name_or_tid)
    {
        frs_task_kill_name(param->argv[target_index]);
    }
    else
    {
        frs_tid_t tid_to_del = (frs_tid_t)strtol(param->argv[target_index], NULL, 10);
        if (0 == tid_to_del) {  // TODO: more error detection
            frs_error("Wrong target TID number \n");
            goto end_task_err;
        }
        frs_task_kill(tid_to_del);
    }
    goto end_task;

end_task: /* kill task */
    frs_task_kill_name(NULL);
end_task_err:
    print_help(param->argv[0]);
    frs_task_kill_name(NULL);
}