#include "FreeRTOS-Shell.h"

struct template_t {
	struct template_t *next_p;
	char *name;
	void *payload;
};

struct file_t {
	struct file_t *next_p;
	char *name;
	TaskFunction_t function;
};

struct task_t {
	struct task_t *next_p;	/* next task */
	char *name;				/* task name */
	TaskHandle_t handle;	/* task handler */
};

struct template_list_t {
	struct template_t *start_p;
	struct template_t *end_p;
	uint8_t len;
};

static struct file_list_t {
	struct file_t *start_file_p;
	struct file_t *end_file_p;
	uint8_t len;
}file_list = {
	NULL, NULL, 0 };

static struct task_list_t{
	struct task_t *start_task_p;
	struct task_t *end_task_p;
	uint8_t len;
}task_list = {
	NULL, NULL, 0};

static inline void *malloc_wrapper(size_t size);

static int create_task_wrapper(
	TaskFunction_t pxTaskCode,
	const char * const pcName,
	const uint16_t usStackDepth,
	void * const pvParameters,
	UBaseType_t uxPriority,
	TaskHandle_t * const pxCreatedTask); /*lint !e971 Unqualified char types are allowed for strings and single characters only. */

static int check_name_list(const char * const name, void *list);

uint8_t frs_task_create(
	TaskFunction_t pvTaskCode,
	const char * const pcName,
	uint16_t usStackDepth,
	void *pvParameters,
	UBaseType_t uxPriority
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
	uint8_t namelen = (uint8_t)strlen(pcName);
	strcpy(strtocmp, pcName);
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
	if (0 != create_task_wrapper(pvTaskCode, new_task_p->name, usStackDepth, pvParameters, uxPriority, &(new_task_p->handle)))
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

uint8_t frs_task_register(TaskFunction_t fn, const char * const name
)
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
	new_file_p->function = fn;
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

uint8_t frs_task_get_tid(const char * const name)
{
	return (uint8_t)check_name_list(name, &file_list);
}

uint8_t frs_task_run(
	uint8_t tid,
	uint16_t stackDepth,
	void *pvParameters,
	UBaseType_t uxPriority
)
{
	uint8_t cnt;
	struct file_t *current_file;
	if (tid > file_list.len)
	{
		frs_error("invalid tid number.\n");
		return 0;
	}
	current_file = file_list.start_file_p;
	for (cnt = 1; cnt < tid; cnt++)
	{
		current_file = current_file->next_p;
	}
	return frs_task_create(current_file->function, current_file->name, stackDepth, pvParameters, uxPriority);
}

uint8_t frs_task_run_name(
	const char * const name,
	uint16_t stackDepth,
	void *pvParameters,
	UBaseType_t uxPriority
	)
{
	uint8_t tid;
	tid = (uint8_t)check_name_list(name, &file_list);
	if( 0 >= tid)
	{
		frs_error("%s does not exist.\n", name);
		return 0;
	}
	return frs_task_run(tid, stackDepth, pvParameters, uxPriority);
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

static int create_task_wrapper(
	TaskFunction_t pxTaskCode,
	const char * const pcName,
	const uint16_t usStackDepth,
	void * const pvParameters,
	UBaseType_t uxPriority,
	TaskHandle_t * const pxCreatedTask
)
{
	BaseType_t result;
	result = xTaskCreate(pxTaskCode, pcName, usStackDepth, pvParameters, uxPriority, pxCreatedTask);
	if (pdPASS != result)
	{
		switch (result)
		{
		case errCOULD_NOT_ALLOCATE_REQUIRED_MEMORY:
			frs_error("allocation task %s failed. Probably not enough memory.\n", pcName);
		default:
			frs_error("unknown error.\n");
		}
	}
	frs_print("A new task %s created\n", pcName);
	return (pdPASS == result)? 0:1;
}

static int check_name_list(const char * const nametocmp, void *list)
{
	int cnt;
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