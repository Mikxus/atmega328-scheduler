#include <kernel/event.h>
#include "event_priv.h"

kernel_errno_t _add_event(event_t *event)
{
    if (event == nullptr)
        return KERNEL_ERR_INVALID_PARAMETER;

    _sched_lists.event_list.add_tail(&event);

    return KERNEL_OK;
}

kernel_errno_t _remove_event(event_t *event)
{
    if (event == nullptr)
        return KERNEL_ERR_INVALID_PARAMETER;

    return _sched_lists.event_list.remove(event)
}

bool _is_event_empty(event_t *event)
{
    return (event->task_count != 0) ? 1 : 0;
}