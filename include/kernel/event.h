#ifndef _EVENT_H_
#define _EVENT_H_
#include <kernel/task.h>
#include <kernel/drivers/data_types/intrusive_slinked_list.h>

typedef enum
{
    EVENT_UNUSED = 0,
    EVENT_MUTEX,
    EVENT_SLEEP
} event_type_t;


typedef struct event_t
{
    event_type_t type;
    task_data_t* task_arr[CONF_EVENT_MAX_TASK_CNT];
    intrusive_slinked_list_node<event_t> next_event;
} event_t;

static_assert(CONF_EVENT_MAX_TASK_CNT <= 255, "CONF_EVENT_MAX_TASK_CNT has to be less <= 255");

#endif // _EVENT_H_