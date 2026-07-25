/**
 * @file task_utils.cpp  
 * @brief PRIVATE - internal task managment utilities
 */
#include "task_utils.h"

uint16_t _get_task_stack_size(task_data_t *task)
{
    return (size_t) task->stack.size;
}

uint16_t _get_task_stack_usage(task_data_t *task)
{
    // stack top - sp
    return (uint16_t) &task->stack.ptr[task->stack.size - 1] - task->cpu_state.sp;
}

void _set_task_state(task_data_t* task, task_state_t state)
{
    if (task == nullptr)
        return;

    task->state = state;
    return;
}

void _set_task_priority(task_data_t* task, uint8_t priority)
{
    #if SCHEDULER_HAS_PRIORITIES == 1
    task->priority = priority;
    #endif
    return;
}

uint8_t _get_task_priority(task_data_t* task)
{
    #if SCHEDULER_HAS_PRIORITIES == 1
    return task->priority;
    #endif
    return 0;
}

task_data_t* _get_ready_list_head()
{
    return _sched_lists.ready_list.get_head();
}