#ifndef _SCHEDULER_H_
#define _SCHEDULER_H_

#include <stdio.h>
#include <avr/cpufunc.h>

#include "task.h"
#include "context_switch.h"
#include "drivers/uart/uart.h" 
#include "drivers/clock/clock.h"

/**
 * @brief Initializes the scheduler
 */
void init_scheduler(void);

/**
 * @brief Starts the scheduler
 * @note Forces context switch to the first task this doesn't return
 */
void __attribute__((noreturn)) start_scheduler(void);

/**
 * @brief Adds a new task to the scheduler
 * 
 * @param task 
 * @param name task name
 * @param priority task priority (0 - 255)
 * @param slice_ms lenght of time slice in milliseconds
 * @param entry Task entry function ptr
 * @return true 
 * @return false 
 */
bool create_task(
    task_data_t &task,
    volatile uint8_t *stack_array,
    const uint16_t stack_size,
    const char *name,
    const uint8_t priority,
    const uint8_t slice_ms,
    void (entry)(void));

#endif