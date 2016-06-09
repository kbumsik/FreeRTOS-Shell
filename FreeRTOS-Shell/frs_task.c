#include "FreeRTOS-Shell.h"

/*******************************************************************************
 * struct declaration     
 ******************************************************************************/
/**
 * @brief      file and task elements structure template
 * @details    This template is used for generic access of file and task.
 * 			   file_t and task_t structure must follow these format.
 */
struct template_t {
    struct template_t *next_p;    //< pointer to a next element (linked list)
    char *name;                   //< name of the element
    frs_id_t id;                   //< id of element
    void *payload;                //< rest of datafield of the element.
                                  //file_t and task_t can have different datafeild
                                  //from this point.
};
/**
 * @brief      file element
 */
struct file_t {
    struct file_t *next_p;
    char *name;
    frs_inode_t inode; 
    TaskFunction_t function;
};
/**
 * @brief      task element
 */
struct task_t {
    struct task_t *next_p;    
    char *name;
    frs_tid_t   tid;
    TaskHandle_t handle;    
};
/**
 * @brief      file and task list structure template
 * @details    This template is used for generic access of file and task list.
 *                file_list_t and task_list_t structure must follow these format.
 *                this list uses linked list
 */
struct template_list_t {
    struct template_t *start_p; //< pointer to the starting element
    struct template_t *end_p;    //< pointer to the ending element
    uint8_t len;                //< length of list
};

/*******************************************************************************
 * static function declaration 
 ******************************************************************************/
static uint8_t task_create(
    TaskFunction_t task_fn,
    const char * const task_name,
    uint16_t stack_depth,
    void *parameters,
    UBaseType_t priority
);

frs_tid_t task_run(
    struct file_t *file_p,
    uint16_t stack_depth,
    void *parameters,
    UBaseType_t priority
);

static uint8_t list_add(void *element, void *list);

static int list_delete_from_id(frs_id_t id, void *list);

static frs_id_t list_delete_from_payload(void *payload, void *list);

static struct template_t *list_find_from_name(const char * const name, void *list);

inline static TaskHandle_t list_find_handle_from_name(const char * const name, void *list)
{
    struct template_t *result = list_find_from_name(name, list);
    if (NULL == result)
    {
        return NULL;
    }
    return (TaskHandle_t) result->payload;
}

static struct template_t *list_find_from_id(const frs_id_t id, void *list);

inline static struct template_t *list_find_handle_from_id(const frs_id_t id, void *list)
{
    struct template_t *result = list_find_from_id(id, list);
    if (NULL == result)
    {
        return NULL;
    }
    return (TaskHandle_t)result->payload;
}

static inline void *malloc_wrapper(size_t size);

static int freertos_task_create_wrapper(
    TaskFunction_t task_function,
    const char * const task_name,
    const uint16_t stack_depth,
    void * const parameters,
    UBaseType_t priority,
    TaskHandle_t * const task_handle); /*lint !e971 Unqualified char types are allowed for strings and single characters only. */


/*******************************************************************************
 * local variables 
 ******************************************************************************/
static struct file_list_t {
    struct file_t *start_file_p;
    struct file_t *end_file_p;
    uint8_t len;
}file_list = {
    NULL, NULL, 0 
};

static struct task_list_t{
    struct task_t *start_task_p;
    struct task_t *end_task_p;
    uint8_t len;
}task_list = {
    NULL, NULL, 0
};

/*******************************************************************************
 * function definitions 
 ******************************************************************************/
frs_inode_t frs_task_register(TaskFunction_t task_fn, const char * const name)
{
    struct file_t *new_file_p;
    /* check if there is already same one */
    if ( NULL != list_find_from_name(name, &file_list))
    {
        frs_print("%s already exists.\n", name);
        return 0;
    }

    /* init a new task */
    new_file_p = (struct file_t *)malloc_wrapper(sizeof(struct file_t));
    if (NULL == new_file_p)
    {
        return 0;
    }
    new_file_p->function = task_fn;
    new_file_p->name = (char *)malloc_wrapper(strlen(name) + 1);
    if (NULL == new_file_p->name)
    {
        return 0;
    }
    strcpy(new_file_p->name, name);

    /* Put the task into the list */
    list_add(new_file_p, &file_list);
    return new_file_p->inode;
}

frs_tid_t frs_task_run_inode(
    frs_inode_t inode,
    uint16_t stack_depth,
    void *parameters,
    UBaseType_t priority
)
{
    struct file_t *current_file;
    current_file = file_list.start_file_p;
    while ((NULL != current_file ) &&
         (inode != current_file->inode))
    {
        current_file = current_file->next_p;
    }
    if (NULL == current_file)
    {
        frs_error("invalid inode number.\n");
            return 0;
    }
    return task_run(current_file, stack_depth, parameters, priority);
}

frs_tid_t frs_task_run_name(
    const char * const name,
    uint16_t stack_depth,
    void *parameters,
    UBaseType_t priority
)
{
    struct file_t *tmp;
    tmp = (struct file_t *)list_find_from_name(name, &file_list);
    if( NULL == tmp )
    {
        frs_error("%s does not exist.\n", name);
        return 0;
    }
    return task_run(tmp, stack_depth, parameters, priority);
}

void frs_task_suspend(frs_tid_t tid_to_pause)
{
    TaskHandle_t handle;
    if (0 == tid_to_pause)
    {
        // get current handle or tid and then delete
        handle = xTaskGetCurrentTaskHandle();
    }
    else
    {
        handle = ((struct task_t *) list_find_handle_from_id(tid_to_pause, &task_list));
    }
    if (NULL == handle)
    {
        goto end_error;
    }
    frs_print("Task suspending..\n");
    vTaskSuspend(handle);
    return;
end_error:
    frs_error("Suspending task failed\n");
}

void frs_task_suspend_name(const char * const name_to_pause)
{
    TaskHandle_t handle;
    if (NULL == name_to_pause)
    {
        // get current handle or tid and then delete
        handle = xTaskGetCurrentTaskHandle();
    }
    else
    {
        handle = ((struct task_t *) list_find_handle_from_name(name_to_pause, &task_list));
    }
    if (NULL == handle)
    {
        goto end_error;
    }
    frs_print("Task suspending..\n");
    vTaskSuspend(handle);
    return;
end_error:
    frs_error("Suspending task failed\n");
}

void frs_task_resume(frs_tid_t tid_to_resume)
{
    TaskHandle_t handle;
    if (0 == tid_to_resume)
    {
        // get current handle or tid and then delete
        handle = xTaskGetCurrentTaskHandle();
    }
    else
    {
        handle = ((struct task_t *) list_find_handle_from_id(tid_to_resume, &task_list));
    }
    if (NULL == handle)
    {
        goto end_error;
    }
    frs_print("Task resuming..\n");
    vTaskResume(handle);
    return;
end_error:
    frs_error("Resuming task failed\n");
}

void frs_task_resume_name(const char * const name_to_resume)
{
    TaskHandle_t handle;
    if (NULL == name_to_resume)
    {
        // get current handle or tid and then delete
        handle = xTaskGetCurrentTaskHandle();
    }
    else
    {
        handle = ((struct task_t *) list_find_handle_from_name(name_to_resume, &task_list));
    }
    if (NULL == handle)
    {
        goto end_error;
    }
    frs_print("Task resuming..\n");
    vTaskResume(handle);
    return;
end_error:
    frs_error("Resuming task failed\n");
}

void frs_task_kill(frs_tid_t tid_to_delete)
{
    TaskHandle_t handle;
    if (0 == tid_to_delete)
    {
        // get current handle or tid and then delete
        handle = xTaskGetCurrentTaskHandle();
    }
    else
    {
        handle = ((struct task_t *) list_find_handle_from_id(tid_to_delete, &task_list));
    }
    if (NULL == handle)
    {
        goto end_error;
    }
    list_delete_from_payload((void *)handle, &task_list);
    frs_print("task deleted\n");
    vTaskDelete(handle);
    return;
end_error:
    frs_error("Deleting task failed\n");
}

void frs_task_kill_name(const char * const name)
{
    TaskHandle_t handle;
    if (NULL == name)
    {
        // get current handle or tid and then delete
        handle = xTaskGetCurrentTaskHandle();
    }
    else
    {
        handle = ((struct task_t *) list_find_handle_from_name(name, &task_list));
    }
    if (NULL == handle)
    {
        goto end_error;
    }
    list_delete_from_payload((void *)handle, &task_list);
    frs_print("task deleted\n");
    vTaskDelete(handle);
    return;
end_error:
    frs_error("Deleting task failed\n");
}

void frs_task_print_flist(void)
{
    struct file_t *file_p;
    file_p = file_list.start_file_p;
    while(NULL != file_p)
    {
        printf("%d\t %s\n", file_p->inode, file_p->name);
        file_p = file_p->next_p;
    }
}

/*******************************************************************************
 * local function definitions 
 ******************************************************************************/
static uint8_t task_create(
    TaskFunction_t task_fn,
    const char * const task_name,
    uint16_t stack_depth,
    void *parameters,
    UBaseType_t priority
)
{
    struct task_t *new_task_p;
    
    /* init a new task */
    new_task_p = (struct task_t *)malloc_wrapper(sizeof(struct task_t));
    if (NULL == new_task_p)
    {
        return 0;
    }
    /* appending a number for the task, to avoid duplicated task name */
    uint8_t cnt = 1;
    char strtocmp[frsTASK_MAX_NAME_LEN+1];
    uint8_t namelen = (uint8_t)strlen(task_name);
    strcpy(strtocmp, task_name);
    strtocmp[namelen+1] = '\0';
    while(1)
    {
        strtocmp[namelen] = (char)'0' + cnt;
        if ( NULL == list_find_from_name(strtocmp, &task_list))
        {
            break;
        }
        cnt++;
    }
    new_task_p->name = (char *)malloc_wrapper(strlen(strtocmp)+1);
    if (NULL == new_task_p->name)
    {
        return 0;
    }
    strcpy(new_task_p->name, strtocmp);

    /* init handle */
    if (0 != freertos_task_create_wrapper(task_fn, new_task_p->name, stack_depth, parameters, priority, &(new_task_p->handle)))
    {
        return 0;
    }    
    /* Put the task into the list */
    list_add(new_task_p, &task_list);
    return new_task_p->tid;
}

frs_tid_t task_run(
    struct file_t *file_p,
    uint16_t stack_depth,
    void *parameters,
    UBaseType_t priority
)
{
    if (NULL == file_p)
    {
        frs_error("invalid file pointer.\n");
        return 0;
    }
    return task_create(file_p->function, file_p->name, stack_depth, parameters, priority);
}

static struct template_t *list_find_from_name(const char * const nametocmp, void *list)
{
    frs_inode_t cnt;
    struct template_t *current;
    current = ((struct template_list_t *)list)->start_p;
    for (cnt = 1; NULL != current; cnt++, current = current->next_p)
    {
        if (strcmp(current->name, nametocmp) == 0)
        {
            return current;
        }
    }
    return NULL;
}

static struct template_t *list_find_from_id(const frs_id_t id_to_find, void *list)
{
    frs_inode_t cnt;
    struct template_t *current;
    current = ((struct template_list_t *)list)->start_p;
    for (cnt = 1; NULL != current; cnt++, current = current->next_p)
    {
        if (current->id == id_to_find)
        {
            return current;
        }
    }
    return NULL;
}

static uint8_t list_add(void *element_to_add, void *list_to_add)
{
    struct template_list_t *list = list_to_add;
    struct template_t *current;
    struct template_t *next;
    struct template_t *new = element_to_add;

    // error check
    if ((NULL == element_to_add) || (NULL == list_to_add))
    {
        frs_error("wrong element or list.\n");
        return 0;
    }
    if (NULL == list->start_p)  // empty
    {
        list->start_p = new;
        list->end_p = new;
        new->next_p = NULL;
        new->id = 1;
    }
    else
    {
        /* allocating policy:
           the list should be in order of id.
           1. if the list is like: 1->2->3->4, the new id is 5and
              it is allocated after 4.
           2. if the list is like: 1->2->4->5, the new id is 3 and
              it is allocated between 2 and 4. */
        current = list->start_p;
        next = current->next_p;
        while (NULL != next)
        {   // find a empty id
            if ((current->id + 1) != next->id)
            {
                break;
            }
            current = current->next_p;
            next = next->next_p;
        }
        if (NULL == next)
        {   // add it to the end
            new->next_p = NULL;
            current->next_p = new;
            list->end_p = new;
        }
        else
        {   // add the missing id
            new->next_p = next;
            current->next_p = new;
        }
        new->id = current->id + 1;
    }
    return new->id;
}

static int list_delete_from_id(frs_id_t id_to_find, void *list_to_delete)
{
    struct template_list_t *list = list_to_delete;
    struct template_t *current;
    struct template_t *previous;

    // error check
    if ((NULL == list) ||
        (NULL == list->start_p))
    {
        frs_error("wrong list.\n");
        return 0;
    }
    // find id on the list
    current = list->start_p->next_p;
    previous = list->start_p;
    while (NULL != current)
    {
        if (current->id == id_to_find)
        {
            break;
        }
        previous = current;
        current = current->next_p;
    }
    if (NULL == current)
    {
        // if there is no id on list, exit
        frs_error("no TID %d on the list\n", id_to_find);
        return 1;
    }
    previous->next_p = current->next_p;
    free(current);
}


static frs_id_t list_delete_from_payload(void *payload_to_delete, void *list_to_delete)
{
    struct template_list_t *list = list_to_delete;
    struct template_t *current;
    struct template_t *previous;
    frs_id_t deleted_id;

    // error check
    if ((NULL == list_to_delete) ||
        (NULL == list->start_p))
    {
        frs_error("wrong list or empty.\n");
        return 0;
    }
    // find id on the list
    current = list->start_p->next_p;
    previous = list->start_p;
    while (NULL != current)
    {
        if (current->payload == payload_to_delete)
        {
            break;
        }
        previous = current;
        current = current->next_p;
    }
    if (NULL == current)
    {
        // if there is no id on list, exit
        frs_error("not on the list\n");
        return 1;
    }
    previous->next_p = current->next_p;
    deleted_id = current->id;
    free(current);
    return deleted_id;
}

static inline void *malloc_wrapper(size_t size)
{
    void *new_pointer;
    new_pointer = malloc(size);
    if (NULL == new_pointer)
    {
        frs_error("allocation %d byte failed.\n", size);
        return NULL;
    }
    return new_pointer;
}

static int freertos_task_create_wrapper(
    TaskFunction_t task_function,
    const char * const task_name,
    const uint16_t stack_depth,
    void * const parameters,
    UBaseType_t priority,
    TaskHandle_t * const task_handle
)
{
    BaseType_t result;
    result = xTaskCreate(task_function, task_name, stack_depth, parameters, priority, task_handle);
    if (pdPASS != result)
    {
        switch (result)
        {
        case errCOULD_NOT_ALLOCATE_REQUIRED_MEMORY:
            frs_error("allocation task %s failed. Probably not enough memory.\n", task_name);
        default:
            frs_error("unknown error.\n");
        }
    }
    frs_print("A new task %s created\n", task_name);
    return (pdPASS == result) ? 0 : 1;
}