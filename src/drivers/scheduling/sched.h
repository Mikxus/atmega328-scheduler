/**
 * @file sched.h  
 * @brief   Scheduler seleca
 *          Different scheduling algorithms support different options
 *          Contains defintions with SCHEDULER_ prefix
 *          
 */
#ifndef _SCHED_H_
#define _SCHED_H_
#include <kernel/task.h>
#include <kernel/event.h>
#include <kernel/drivers/data_types/intrusive_slinked_list.h>

typedef struct
{
    // Tasks which are ready to be scheduled
    intrusive_slinked_list<task_data_t, &task_data_t::next_node> ready_list;

    // Tasks which are sleeping
    intrusive_slinked_list<task_data_t, &task_data_t::next_node> sleep_list;
    
    // List of events which hold list of tasks blocked by the event 
    intrusive_slinked_list<event_t, &event_t::next_event> event_list;
} scheduler_lists_t;

extern scheduler_lists_t _sched_lists;

void _sched_pick_next_task_round_robin(void);

#if CONF_SCHEDULER_ALGO_ROUND_ROBIN == 1
    #define _schedule_next_task() _sched_pick_next_task_round_robin()
#else
    #error "No scheduling algorithm defined"
#endif

void _add_event_to_event_list();


#endif // _SCHED_H_