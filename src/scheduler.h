#ifndef _SCHEDULER_H_
#define _SCHEDULER_H_

#include <stdio.h>
#include "lib/singly_linked_list/singly_linked_list.h"
#include "drivers/uart/uart.h" 
#include "drivers/clock/clock.h"
#include "context_switch.h"
#include "task.h"

/**
 * @brief Pointer to the current task
 */
extern volatile task_data_t *c_task;

/*
 * Init task 
 */
extern task_data_t init_task;

void _scheduler_init(void);

#endif