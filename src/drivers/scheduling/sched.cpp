#include "sched.h"
#include "../../task_utils.h"

scheduler_lists_t _sched_lists;

void _sched_pick_next_task_round_robin(void) 
{
    uint32_t last_exec_start_us = UINT32_MAX;
    task_data_t* node;
    task_data_t* result_node;
    node = _sched_lists.ready_list.get_head();

    while (node != nullptr) {
        if (node->exec_start_time_us <= last_exec_start_us && node->state == READY) {
            last_exec_start_us = node->exec_start_time_us; 
            result_node = node;
        }
        
        node = _sched_lists.ready_list.get_next(node);
    }

    c_task = result_node;
}