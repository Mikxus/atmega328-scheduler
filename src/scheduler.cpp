#include "scheduler.h"

/**
 * @brief Pointer to the current task
 */
volatile task_data_t *c_task = nullptr;

task_data_t init_task;


/**
 * @brief Initializes the scheduler before main is called
 */
void _scheduler_init(void) 
{
    cli();
    initialize_uart();
    initialize_clock();

    c_task = &init_task;
    init_task.burst_time_ms = 1;
    init_task.state = UNDEFINED;

    initialize_context_switch_timer(1);

    // init 
    sei();
    return;
}
