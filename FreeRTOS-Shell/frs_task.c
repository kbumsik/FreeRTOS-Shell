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
    TaskFunction_t function;
};
/**
 * @brief      task element
 */
struct task_t {
    struct task_t *next_p;    
    char *name;                
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
static inline void *malloc_wrapper(size_t size);

static int create_task_wrapper(
    TaskFunction_t task_function,
    const char * const task_name,
    const uint16_t stack_depth,
    void * const parameters,
    UBaseType_t priority,
    TaskHandle_t * const task_handle); /*lint !e971 Unqualified char types are allowed for strings and single characters only. */

static uint8_t frs_task_create(
    TaskFunction_t task_fn,
    const char * const task_name,
    uint16_t stack_depth,
    void *parameters,
    UBaseType_t priority
);

static frs_inode_t check_name_list(const char * const name, void *list);

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
    if ( 0 != check_name_list(name, &file_list))
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
    new_file_p->next_p = NULL;

    /* Put the task into the list */
    if (NULL == file_list.start_file_p)
    {
        file_list.start_file_p = new_file_p;
        file_list.end_file_p = new_file_p;
    }
    else
    {
        file_list.end_file_p->next_p = new_file_p;
        file_list.end_file_p = new_file_p;
    }
    return ++file_list.len;
}

inline frs_inode_t frs_task_get_inode(const char * const name)
{
    return (uint8_t)check_name_list(name, &file_list);
}

frs_tid_t frs_task_run(
    frs_inode_t inode,
    uint16_t stack_depth,
    void *parameters,
    UBaseType_t priority
)
{
    uint8_t cnt;
    struct file_t *current_file;
    if (inode > file_list.len)
    {
        frs_error("invalid inode number.\n");
        return 0;
    }
    current_file = file_list.start_file_p;
    for (cnt = 1; cnt < inode; cnt++)
    {
        current_file = current_file->next_p;
    }
    return frs_task_create(current_file->function, current_file->name, stack_depth, parameters, priority);
}

frs_tid_t frs_task_run_name(
    const char * const name,
    uint16_t stack_depth,
    void *parameters,
    UBaseType_t priority
)
{
    uint8_t inode;
    inode = (uint8_t)check_name_list(name, &file_list);
    if( 0 >= inode)
    {
        frs_error("%s does not exist.\n", name);
        return 0;
    }
    return frs_task_run(inode, stack_depth, parameters, priority);
}

void frs_task_print_flist(void)
{
    struct file_t *file_p;
    file_p = file_list.start_file_p;
    for(int i = 1; i <= file_list.len; i++)
    {
        printf("%d\t %s\n", i, file_p->name);
        file_p = file_p->next_p;
    }
}

/*******************************************************************************
 * local function definitions 
 ******************************************************************************/
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

static int create_task_wrapper(
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
    return (pdPASS == result)? 0:1;
}

static uint8_t frs_task_create(
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
        if ( 0 == check_name_list(strtocmp, &task_list))
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
    if (0 != create_task_wrapper(task_fn, new_task_p->name, stack_depth, parameters, priority, &(new_task_p->handle)))
    {
        free(new_task_p);
        return 0;
    }
    new_task_p->next_p = NULL;
    
    /* Put the task into the list */
    if (NULL == task_list.start_task_p)
    {
        task_list.start_task_p = new_task_p;
        task_list.end_task_p = new_task_p;
    }
    else
    {
        task_list.end_task_p->next_p = new_task_p;
        task_list.end_task_p = new_task_p;
    }
    return ++task_list.len;
}

static frs_inode_t check_name_list(const char * const nametocmp, void *list)
{
    frs_inode_t cnt;
    struct template_t *current;
    current = ((struct template_list_t *)list)->start_p;
    for (cnt = 1; NULL != current; cnt++, current = current->next_p)
    {
        if (strcmp(current->name, nametocmp) == 0)
        {
            return cnt;
        }
    }
    return 0;
}
