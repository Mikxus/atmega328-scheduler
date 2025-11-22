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
