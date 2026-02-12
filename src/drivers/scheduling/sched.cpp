#include <kernel/drivers/scheduling/sched.h>
#include "../../task_utils.h"

void schedule_round_robin(void) 
{
    task_data_t* node = _get_next_task(c_task);
    
    /* if last node */
    if (node == nullptr)
        node = _get_head_task();

    c_task = node;
}