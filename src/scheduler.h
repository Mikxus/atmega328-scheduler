#ifndef _SCHEDULER_H_
#define _SCHEDULER_H_

#include <stdio.h>
#include "lib/singly_linked_list/singly_linked_list.h"
#include "drivers/uart/uart.h" 
#include "drivers/clock/clock.h"
#include "kernel/task.h"

/**
 * @brief Pointer to the current task
 */
extern volatile struct task_data *c_task;


void _scheduler_init(void);

void _save_context(void);
void _do_context_switch(void);

#endif