/**
 * @file event.h  
 * 
 * @brief Scheduler's event subsystem.
 * 
 * @details Each blocking event has it's own underlying event structure.
 *
 */
#ifndef _EVENT_H_
#define _EVENT_H_

#include <kernel/task.h>
#include <kernel/data_types/intrusive_slinked_list.h>

typedef enum
{
    EVENT_UNUSED = 0,
    EVENT_MUTEX,
    EVENT_MSG_QUE,
    EVENT_SLEEP
} event_type_t;

typedef struct event_t
{
    event_type_t type;
    intrusive_slinked_list<task_data_t, &task_data_t::next_node> blocked_list;
    intrusive_slinked_list_node<event_t> next_event;
} event_t;

#endif // _EVENT_H_