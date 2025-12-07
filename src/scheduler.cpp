#include "scheduler.h"

/**
 * @brief Pointer to the current task
 */

task_data_t init_task;
/**
 * @brief Initializes the scheduler before main is called
 */
void _scheduler_init(void) 
{
    cli();
    initialize_uart();
    initialize_clock();

    create_task(
        init_task,
        (volatile uint8_t *) 0x00,
        100,
        "init_task",
        0,
        1,
        nullptr);

    initialize_context_switch_timer(1);

    // init 
    sei();
    return;
}
