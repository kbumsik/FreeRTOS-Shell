#include "FreeRTOS-Shell.h"
#include "frs_queue.h"
#include "frs_user_tasks.h"

frs_queue_h frs_stdin;
frs_queue_h frs_stdout;
frs_queue_h frs_stderr;

int frs_init(void)
{
	int result;
    // create stdio queue
    frs_stdin = frs_queue_create(frsSTDIN_LEN, sizeof(char)*frsSTDIO_MSG_SIZE);
    frs_stdout = frs_queue_create(frsSTDOUT_LEN, sizeof(char)*frsSTDIO_MSG_SIZE);
    frs_stderr = frs_queue_create(frsSTDERR_LEN, sizeof(char)*frsSTDIO_MSG_SIZE);
    if ((frs_stdin == NULL) || (frs_stdout == NULL) || (frs_stderr == NULL))
    {
        frs_error("stdio init failed!!!\n");
        return 1;
    }
	// create sh task
    result = frs_task_register(frs_user_sh, "sh");
    result = frs_task_run_inode((frs_inode_t)result, frsSH_STACK_SIZE,
    						 NULL, frsSH_PRIORITY);
    if (0 == result)
    {
        frs_error("sh init failed!!!\n");
    	return 1;
    }
    /* Here, we add user functions
    e.g) result = frs_task_register(function pointer, "function name");
    */
    result = frs_task_register(frs_user_ls, "ls");  // TODO: add error detection
    result = frs_task_register(frs_user_ps, "ps");
    result = frs_task_register(frs_user_top, "top");
    result = frs_task_register(frs_user_kill, "kill");
    result = frs_task_register(frs_user_resume, "resume");
    result = frs_task_register(frs_user_stop, "stop");
    return 0;
}
