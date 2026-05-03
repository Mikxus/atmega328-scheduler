#include "sched.h"
#include "../../task_utils.h"

scheduler_lists_t _sched_lists;

void _sched_pick_next_task_round_robin(void) 
{
    task_data_t* node = _sched_lists.ready_list.get_next(c_task);
    
    /* if last node */
    if (node == nullptr)
        node = _sched_lists.ready_list.get_head();

    c_task = node;
}