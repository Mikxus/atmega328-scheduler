#include "scheduler.h"

/**
 * @brief Initializes the scheduler before main is called
 */
__attribute__((constructor)) void _scheduler_init(void) 
{
    initialize_uart();
    printf("uart initialized\n");

    return;
}