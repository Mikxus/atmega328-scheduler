#ifndef _EVENT_H_
#define _EVENT_H_
#include <kernel/task.h>

typedef enum
{
    EVENT_UNUSED = 0,
    EVENT_MUTEX,
    EVENT_SLEEP
} event_type_t;

typedef struct event_t
{
    event_type_t type;
    task_data_t* volatile task_list;
    event_t *next_node;
} event_t;

#endif // _EVENT_H_