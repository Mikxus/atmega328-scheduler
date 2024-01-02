#include "cpu.h"

struct cpu_state regs;

/**
 * @brief Initializes the scheduler before main is called
 */
__attribute__((constructor)) void _scheduler_init(void) 
{
    return; 
}