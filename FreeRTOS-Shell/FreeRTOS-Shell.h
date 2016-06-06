#ifndef _FREERTOS_Shell_H_
#define _FREERTOS_Shell_H_

#define frs_error(msg, ...)	fprintf(stderr, "FreeRTOS-Sh: %s:%s:%d " msg, __FILE__, __FUNCTION__, __LINE__, ##__VA_ARGS__)
#define frs_print(msg, ...)	fprintf(stdout, "FreeRTOS-Sh: " msg, ##__VA_ARGS__)
#define frsTASK_MAX_NAME_LEN	10	/* length of filename */

#include "frs_task.h"

#endif // !_FREERTOS-Shell_H_
