#include "context_switch.h"

/* task switch interrupt */
ISR(TIMER0_COMPA_vect)
{
    _do_context_switch();
    return;
}

void initialize_context_switch_timer(uint8_t burst_lenght)
{
    uint8_t sreg = SREG;

    cli();

    SREG = sreg;
    return;
    
}

// Save the current task's context
__attribute__((always_inline)) inline void _save_context(void)
{
    volatile struct task_data *task = c_task;

    c_task->cpu_state.sreg = SREG;


    return;
}