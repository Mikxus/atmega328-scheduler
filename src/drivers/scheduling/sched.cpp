#include "sched.h"
#include "../../task_utils.h"
#include "../../kernel_priv.h"

scheduler_lists_t _sched_lists;

/**
 * @brief Picks task which has been longest time unscheduled  
 */
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

    _c_task = result_node;
}

/**
 * @brief Picks task with the highest priority
 * case:
 *      Same priority
 *         - Picks task which has been unscheduled the longest 
 */
/* Not ready
void _sched_pick_next_task_highest_priority(void)
{
    uint8_t priority_cnt = 0;
    uint8_t highest_prio = 0;
    task_data_t* first_task_ptr = nullptr;
    task_data_t* seek_ptr = _sched_lists.ready_list.get_head();

    while (seek_ptr != nullptr) {
        if (seek_ptr->priority >= highest_prio) {
            if (priority_cnt == 0)
                first_task_ptr = seek_ptr;

            highest_prio = seek_ptr->priority;
            priority_cnt += 1;
        }

        seek_ptr = _sched_lists.ready_list.get_next(seek_ptr);
    }

    /* */
    if (priority_cnt == 1)
        _c_task = first_task_ptr;
}*/