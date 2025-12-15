#include "context_switch.h"

/**
 * @brief Task switch interrupt
 * naked ISR to avoid C++ prologue/epilogue generation
 * 
 */
ISR(TIMER0_COMPB_vect, ISR_NAKED) __attribute__((hot, flatten));
ISR(TIMER0_COMPB_vect)
{
    _SAVE_CTX();
    asm volatile ("clr r1" ::: "memory");

    calculate_task_execution_time(c_task);

    // schedule next task
    schedule_round_robin();

    OCR0B = freq_to_timer_comp_value(1000, 64) + TCNT0; // ~1 ms task switch interval
    c_task->exec_start_time_us = get_us();

    _RESTORE_CTX();
    asm volatile ("reti"  ::: "memory");
}

void calculate_task_execution_time(task_data_t volatile *task)
{
    #if SCHEDULER_TASK_TRACK_STATISTICS == 1
    if (task == nullptr)
        return;

    uint32_t current_time = get_us();
    uint32_t exec_time;
    
    // Handle timer wraparound correctly
    if (current_time >= task->exec_start_time_us) {
        exec_time = current_time - task->exec_start_time_us;
    } else {
        // Wraparound occurred
        exec_time = (UINT32_MAX - task->exec_start_time_us) + current_time + 1;
    }
    
    // Check if adding exec_time would overflow
    if (task->exec_time_us > UINT32_MAX - exec_time) {
        task->exec_time_overflow_count++;
        // Optional: wrap the counter or reset it
        task->exec_time_us = exec_time;  // Start fresh after overflow
    } else {
        task->exec_time_us += exec_time;
    }
    #endif
} 

void soft_yield_task(void)
{
    OCR0B = TCNT0;
}

/*
 * @brief simplest way of scheduling
 * 
 */
void schedule_round_robin(void) 
{
    task_data_t* node = _get_next_task(c_task);
    
    /* if last node */
    if (node == nullptr)
        node = _get_head_task();

    c_task = node;
}

void start_context_switch_timer(void)
{
    uint8_t sreg = SREG;
    cli();

    // assume timer0 is already initialized 
    enable_timer0_interrupt(COMPB_INTERRUPT);

    OCR0B = freq_to_timer_comp_value(1000, 64) + TCNT0;  // try to trigger at 1 ms intervals 
    SREG = sreg;
    return;
    
}
