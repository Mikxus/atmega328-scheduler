#include <kernel/event.h>
#include "drivers/scheduling/sched.h"
#include "task_utils.h"
#include "event_priv.h"

kernel_errno_t _add_event(event_t *event)
{
    if (event == nullptr)
        return KERNEL_ERR_INVALID_PARAMETER;

    _sched_lists.event_list.add_tail(event);

    return KERNEL_OK;
}

kernel_errno_t _remove_event(event_t *event)
{
    if (event == nullptr)
        return KERNEL_ERR_INVALID_PARAMETER;

    if (event->blocked_list.get_head() != nullptr)
        return KERNEL_ERR_NOT_EMPTY;

    return _sched_lists.event_list.remove(event);
}

bool _is_event_empty(const event_t* event)
{
    return (event->blocked_list.get_head() == nullptr);
}

kernel_errno_t _event_block_task(task_data_t* task, event_t* event)
{
    if (task == nullptr || event == nullptr)
        return KERNEL_ERR_INVALID_PARAMETER;

    _set_task_state(task, BLOCKED);
    _sched_lists.ready_list.remove(task);
    event->blocked_list.add_tail(task);

    return KERNEL_OK;
}

task_data_t* _event_unblock_first(event_t* event)
{
    task_data_t *task;

    if (event == nullptr)
        return nullptr;

    task = event->blocked_list.get_head();

    if (task == nullptr)
        return nullptr;

    _set_task_state(task, READY);
    event->blocked_list.remove(task);
    _sched_lists.ready_list.add_tail(task);

    return task;
}

task_data_t* _event_unblock_highest_prio(event_t* event)
{
    uint8_t priority = 0;
    task_data_t* seek_ptr = nullptr;
    task_data_t* chosen_ptr = nullptr;

    if (event == nullptr)
        return nullptr;

    /* Default to first task */
    chosen_ptr = event->blocked_list.get_head(); 
    if (chosen_ptr == nullptr)
        return nullptr;

    priority = chosen_ptr->priority;

    seek_ptr = event->blocked_list.get_next(chosen_ptr);

    while (seek_ptr != nullptr) {
        if (seek_ptr->priority > priority) {
            priority = seek_ptr->priority;
            chosen_ptr = seek_ptr;
        }
        seek_ptr = event->blocked_list.get_next(seek_ptr);
    }
    
    _set_task_state(chosen_ptr, READY);
    event->blocked_list.remove(chosen_ptr);
    _sched_lists.ready_list.add_tail(chosen_ptr);
    return chosen_ptr;
}