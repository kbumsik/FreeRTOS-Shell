#ifndef RTOS_WRAPPER_H
#define RTOS_WRAPPER_H

#include "FreeRTOS.h"
#include "task.h"

uint8_t frs_task_create(
	TaskFunction_t pvTaskCode,
	const char * const pcName,
	uint16_t usStackDepth,
	void *pvParameters,
	UBaseType_t uxPriority
);

uint8_t frs_task_register(
	TaskFunction_t function,
	const char * const pcName
);

uint8_t frs_task_get_tid(const char * const name);

uint8_t frs_task_run(
	uint8_t tid,
	uint16_t stackDepth,
	void *pvParameters,
	UBaseType_t uxPriority
	);

uint8_t frs_task_run_name(
	const char * const name,
	uint16_t stackDepth,
	void *pvParameters,
	UBaseType_t uxPriority
	);

void frs_task_print_flist(void);

#endif /* RTOS_WRAPPER_H */