#include "scheduler.h"

void init_scheduler(void) 
{
    uint8_t sreg = SREG;
    cli();
    initialize_uart();
    initialize_clock();

    SREG = sreg;
    return;
}

void __attribute__((noreturn)) start_scheduler(void)
{
    kernel_start();
    __builtin_unreachable();
}

bool create_task(
    task_data_t &task,
    volatile uint8_t *stack_array,
    const uint16_t stack_size,
    const char *name,
    const uint8_t priority,
    const uint8_t slice_ms,
    void (entry)(void))
{
    uint8_t sreg = SREG;
    cli();

    #if SCHEDULER_HAS_PRIORITIES == 1
    task.priority = priority;
    #endif
    task.time_slice_ms = slice_ms;
    #if CONF_TRACK_TASK_CPU_TIME == 1
    task.exec_time_us = 0;
    task.exec_time_overflow_count = 0;
    task.exec_start_time_us = 0;
    #endif
    task.next_node = nullptr;
    task.stack.memory_ptr = stack_array;
    task.stack.size = stack_size;
    task.state = READY;
    task.cpu_state.sreg = 0x80; // interrupts enabled

    task.next_node = nullptr;

    if (strlen(name) >= CONF_TASK_NAME_MAX_LENGTH)
        return 1;

    strcpy(task.name, name);

    // initialize stack pointer to third byte? so [0 - 1] can have entry's pc 
    task.cpu_state.sp = (uint16_t) &stack_array[stack_size - 3]; // might be incorrect address

    // Set entry to stack
    task.stack.memory_ptr[stack_size - 1] = (uint8_t) ((uint16_t) entry & 0xFF);        // pc l 
    task.stack.memory_ptr[stack_size - 2] = (uint8_t) (((uint16_t) entry >> 8) & 0xFF); // pc h

    _add_task(&task);

    /* if there is no existing task */
    if (c_task == nullptr)
        c_task = _get_head_task();

    SREG = sreg;
    return 0;
}

bool remove_task(task_data_t *task)
{
    if (_remove_task_from_ready_list(task))
        return 1;

    _set_task_state(task, UNDEFINED);
    return 0;
}