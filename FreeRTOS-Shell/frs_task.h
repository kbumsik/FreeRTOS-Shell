#ifndef _FRS_TASK_H_
#define _FRS_TASK_H_

#include "FreeRTOS.h"
#include "task.h"

typedef uint8_t frs_tid_t;
typedef uint8_t frs_inode_t;

/**
 * @brief      register task as file
 *
 * @param[in]  task_fn  A pointer to a task function
 * @param[in]  name     The name of the file
 *
 * @return     inode being registered
 */
frs_inode_t frs_task_register(TaskFunction_t task_fn, const char * const name);

/**
 * @brief      get inode of task file using name
 *
 * @param[in]  name  The name of task to find
 *
 * @return     inode of task file
 */
frs_inode_t frs_task_get_inode(const char * const name);
/**
 * @brief      run task
 *
 * @param[in]  inode         The inode of task to run
 * @param[in]  stack_depth   The stack depth of task
 * @param      parameters    The parameters being sent to the task
 * @param[in]  priority		 The priority of task to run
 *
 * @return     allocated tid(task id) of task
 */
frs_tid_t frs_task_run(
    frs_inode_t inode,
    uint16_t stack_depth,
    void *parameters,
    UBaseType_t priority
);
/**
 * @brief      run task using name
 *
 * @param[in]  name         The name of task file to run
 * @param[in]  stack_depth  The stack depth of task
 * @param      parameters   The parameters being sent to the task
 * @param[in]  priority     The priority of task to run
 *
 * @return     allocated tid(task id) of task
 */
frs_tid_t frs_task_run_name(
    const char * const name,
    uint16_t stack_depth,
    void *parameters,
    UBaseType_t priority
);

frs_tid_t frs_task_get_tid(const char * const name);

int frs_task_pause(frs_tid_t tid);

int frs_task_pause_name(const char * const name);

int frs_task_kill(frs_tid_t tid);

int frs_task_kill_name(const char * const name);

void frs_task_print_flist(void);

#endif /* _FRS_TASK_H_ */