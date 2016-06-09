#ifndef _FRS_TASK_H_
#define _FRS_TASK_H_

#include "FreeRTOS.h"
#include "task.h"

typedef uint8_t frs_id_t;       //< Generic ID number type
typedef frs_id_t frs_tid_t;		//< TID (task ID) of FreeRTOS task
typedef frs_id_t frs_inode_t;	//< inode of pseudo-filesystem

#ifdef __cplusplus
extern "C"{
#endif
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
 * @brief      run task
 *
 * @param[in]  inode         The inode of task to run
 * @param[in]  stack_depth   The stack depth of task. The value specifies the 
 								number of words the stack can hold, not the 
 								number of bytes. In 32-bit architecture,the 
 								stack size is multiply of 4 bytes.
 * @param      parameters    The parameters being sent to the task
 * @param[in]  priority		 The priority of task to run. It should be 
 								between 0 and (configMAX_PRIORITIES-1)
 *
 * @return     allocated tid(task id) of task
 */
frs_tid_t frs_task_run_inode(
    frs_inode_t inode,
    uint16_t stack_depth,
    void *parameters,
    UBaseType_t priority
);
/**
 * @brief      run task using name
 *
 * @param[in]  name         The name of task file to run
 * @param[in]  stack_depth  The stack depth of task. The value specifies the 
 								number of words the stack can hold, not the 
 								number of bytes. In 32-bit architecture,the 
 								stack size is multiply of 4 bytes.
 * @param      parameters    The parameters being sent to the task
 * @param[in]  priority		 The priority of task to run. It should be 
 								between 0 and (configMAX_PRIORITIES-1)
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

void frs_task_suspend(frs_tid_t tid);

void frs_task_suspend_name(const char * const name);

void frs_task_resume(frs_tid_t tid);

void frs_task_resume_name(const char * const name);

void frs_task_kill(frs_tid_t tid);

void frs_task_kill_name(const char * const name);

void frs_task_print_flist(void);

#ifdef __cplusplus
}
#endif

#endif /* _FRS_TASK_H_ */
