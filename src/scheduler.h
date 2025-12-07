#ifndef _SCHEDULER_H_
#define _SCHEDULER_H_

#include <stdio.h>
#include "task.h"
#include "drivers/uart/uart.h" 
#include "drivers/clock/clock.h"
#include "context_switch.h"

/*
 * Init task 
 */
extern task_data_t init_task;

void _scheduler_init(void);

#endif