#include <kernel/task.h>
#include "task_utils.h"
#include "drivers/scheduling/sched.h"

kernel_errno_t create_task(
    task_data_t &task,
    volatile uint8_t *stack_array,
    const uint16_t stack_size,
    PGM_P name,
    const uint8_t priority,
    const uint8_t slice_ms,
    void (entry)(void))
{
    ATOMIC_GUARD();

    #if SCHEDULER_HAS_PRIORITIES == 1
    task.priority = priority;
    #endif
    task.time_slice_ms = slice_ms;
    #if CONF_TRACK_TASK_CPU_TIME == 1
    task.exec_time_us = 0;
    task.exec_time_overflow_count = 0;
    task.exec_start_time_us = 0;
    #endif
    task.stack.ptr = stack_array;
    task.stack.size = stack_size;
    task.state = READY;
    task.cpu_state.sreg = 0x80; // interrupts enabled

    task.name = name;

    // init stack pointer
    task.cpu_state.sp = (uint16_t) &stack_array[stack_size - 3];

    // Set entry to stack
    task.stack.ptr[stack_size - 1] = (uint8_t) ((uint16_t) entry & 0xFF);        // pc l 
    task.stack.ptr[stack_size - 2] = (uint8_t) (((uint16_t) entry >> 8) & 0xFF); // pc h

    _sched_lists.ready_list.add_tail(&task);

    /* if there is no existing task */
    if (c_task == nullptr)
        c_task = _get_ready_list_head();

    return KERNEL_OK;
}

void _calc_arg_stack_size(
    bool &args_to_stack,
    uint16_t &regs_used,
    uint16_t &stack_used,
    uint16_t arg_size
) {
    constexpr uint8_t MAX_REG_USAGE = 17;
    uint8_t padding = arg_size % 2;

    if (
        regs_used + arg_size + padding < MAX_REG_USAGE 
        && args_to_stack == false
    ) {
        regs_used += arg_size + padding;
    } else {
        args_to_stack = true;
        stack_used += arg_size;
    }
    return;
}

kernel_errno_t remove_task(task_data_t *task)
{
    ATOMIC_GUARD();
    /* TODO: handle case when task is being blocked by a event */
    if (_sched_lists.ready_list.remove(task) != KERNEL_OK)
        return KERNEL_ERR_NOT_FOUND;

    _set_task_state(task, UNDEFINED);
    return KERNEL_OK;
}


task_data_t* get_current_task()
{
    task_data_t* task;

    ATOMIC_BLOCK() {
    task = c_task;
    }
    return task;
}

uint16_t get_task_pc(task_data_t *task) 
{
    uint16_t sp = 0;
    uint8_t pc_l = 0;
    uint8_t pc_h = 0;

    if (task == nullptr)
        return 0;

    /* 
     * Getting current task isn't supported as
     * task.cpu_state.sp is only updated on ctx switch.
     */
    if (task == get_current_task())
        return 0;

    ATOMIC_BLOCK() {
        sp = task->cpu_state.sp;
        /* 
         * Stack grows toward lower memory addresses
         */
        pc_l = *(uint8_t*)(sp + 2);
        pc_h = *(uint8_t*)(sp + 1);
    }

    return (uint16_t) (pc_h << 8U) | pc_l;
}

kernel_errno_t suspend_task(task_data_t *task)
{
    ATOMIC_GUARD();
    if (task == nullptr)
        return KERNEL_ERR_INVALID_PARAMETER;
    
    if (task->state == BLOCKED)
        return TASK_ERR_BLOCKED;

    _set_task_state(task, STOPPED);
    return KERNEL_OK;
}


kernel_errno_t resume_task(task_data_t *task)
{
    ATOMIC_GUARD();
    if (task == nullptr)
        return KERNEL_ERR_INVALID_PARAMETER;

    if (task->state != STOPPED)
        return KERNEL_ERR_INVALID_PARAMETER;

    _set_task_state(task, READY);
    return KERNEL_OK;
}

uint16_t get_task_stack_size(
    task_data_t* task)
{
    return _get_task_stack_size(task);
}

uint16_t get_task_stack_usage(
    task_data_t* task)
{
    return _get_task_stack_usage(task);
}