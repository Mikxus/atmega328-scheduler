#include "scheduler.h"

/**
 * @brief Pointer to the current task
 */
volatile struct task_data *c_task = nullptr;

/**
 * @brief Initializes the scheduler before main is called
 */
void _scheduler_init(void) 
{
    initialize_uart();
    initialize_clock();
    return;
}

// Save the current task's context
__attribute__((always_inline)) void _save_context(void)
{
    volatile struct task_data *task = c_task;

    c_task->cpu_state.sreg = SREG;


    return;
}