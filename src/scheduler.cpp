#include "scheduler.h"

/**
 * @brief Initializes the scheduler before main is called
 */
void _scheduler_init(void) 
{
    initialize_uart();
    initialize_clock();
    return;
}