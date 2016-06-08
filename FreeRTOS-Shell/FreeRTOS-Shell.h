#ifndef _FREERTOS_Shell_H_
#define _FREERTOS_Shell_H_

#include "FreeRTOS.h"
/*******************************************************************************
 * FreeRTOS configurations
 ******************************************************************************/
#define frsTASK_MAX_NAME_LEN	10	/* length of filename */

#define frsSH_STACK_SIZE	500 // TODO: find optimal sh stack size.
#define frsSH_PRIORITY		(configMAX_PRIORITIES-1) //< priority of shell task

#define frsSTDIO_MSG_SIZE   30 //< size of stdio messages
#define frsSTDOUT_LEN       3 //< length of STDOUT queue
#define frsSTDIN_LEN        5 //< length of STDOUT queue
#define frsSTDERR_LEN       3 //< length of STDOUT queue

#include "frs_task.h"

/*******************************************************************************
 * function declarations
 ******************************************************************************/
#define frs_error(msg, ...)	fprintf(stderr, "FreeRTOS-Sh: %s:%s:%d " msg, \
 								__FILE__, __FUNCTION__, __LINE__, ##__VA_ARGS__)
#define frs_print(msg, ...)	fprintf(stdout, "FreeRTOS-Sh: " msg, ##__VA_ARGS__)

struct frs_arg_t {
    int argc;
    char argv[4][6];
};


/**
 * @brief      initialize FreeRTOS-Shell system and sh
 *
 * @return     0 if init is successful.
 */
int frs_init(void);
/**
 * @brief      start scheduler of FreeRTOS
 */
inline void frs_start(void)
{
    vTaskStartScheduler();
}


#endif // !_FREERTOS-Shell_H_
