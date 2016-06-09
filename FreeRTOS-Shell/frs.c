#include "FreeRTOS-Shell.h"
#include "frs_sh.h"
#include "frs_queue.h"
#include "frs_user_tasks.h"

frs_queue_h frs_stdin;
frs_queue_h frs_stdout;
frs_queue_h frs_stderr;

inline static void print_ready(const char * const current_path);
static int extract_cmd(struct frs_arg_t *fst_arg, char *buf);
static char *str_skip_spaces(char *buf);
static int get_nonwhitespace_len(char *buf);

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
    result = frs_task_register(frs_sh, "sh");
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
    result = frs_task_register(frs_user_ls, "ls");
    result = frs_task_register(frs_user_ps, "ps");
    result = frs_task_register(frs_user_top, "top");
    result = frs_task_register(frs_user_kill, "kill");
    return 0;
}

void frs_sh(void *parameters)
{
    // init the sh
    char buf[24];
    char current_path[24];
    struct frs_arg_t fst_arg;
    struct frs_arg_t snd_arg;
    int result;
    int cmd;
    strcpy(current_path, "/");
    while (1)
    {
        print_ready(current_path);
        fgets(buf, 24, stdin);
        // get the first commend
        cmd = extract_cmd(&fst_arg, buf);
        switch (cmd)
        {
        case FRS_SH_CMD_SIMPLE:
            {   // critical section
                taskENTER_CRITICAL();
                frs_task_run_name(fst_arg.argv[0], frsSH_STACK_SIZE,
                    &fst_arg, frsSH_PRIORITY);
                taskEXIT_CRITICAL();
            }
            taskYIELD();
            // TODO: inactivate shell until task goes background
            break;
        case FRS_SH_CMD_PIPELINE:

            break;
        case FRS_SH_CMD_REDIRECT:

            break;
        }
        vTaskDelay(50);
        // check if there is pipeline
        // check if there is redirection
        // extract additional arguments of the fisrt commend
        // get the seconde commend if there is pipeline or redirection
        // extract additional arguments of the second commend
        // get into a critical section and run two commend
    }
}

inline static void print_ready(const char * const current_path)
{
    printf("FreeRTOS-Shell:%s$ ", current_path);
}


static int extract_cmd(struct frs_arg_t *fst_arg, char *buf)
{
    int len;
    char current_char;
    fst_arg->argc = 0;
    buf = str_skip_spaces(buf);
    current_char = *buf;
    while ((current_char != '\n')&&
           (current_char != '\0')&&
           (current_char != '|')&&
           (current_char != '>'))   // TODO: add more and find a batter way
    {
        len = get_nonwhitespace_len(buf);
        strncpy(fst_arg->argv[fst_arg->argc], buf, len);
        fst_arg->argv[fst_arg->argc][len] = '\0';
        fst_arg->argc++;
        buf = buf + len;
        buf = str_skip_spaces(buf);
        current_char = *buf;
    }
    switch (current_char)
    {
    case '|':
        return FRS_SH_CMD_PIPELINE;
    case '>':
        return FRS_SH_CMD_REDIRECT;
    default:
        return FRS_SH_CMD_SIMPLE;
    }
}


static char *str_skip_spaces(char *buf)
{
    while (*buf == ' ')
    {
        buf++;
    }
    return buf;
}


static int get_nonwhitespace_len(char *buf)
{
    int result;
    char current;
    result = 0;
    current = *buf;
    while ((current != ' ') && // space
           (current != '\n') &&  // newline
           (current != '\0'))    // null charater
    {
        result++;
        buf++;
    	current = *buf;
    }
    return result;
}