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

typedef struct
{
    // Tasks which are ready to be scheduled
    task_data_t* volatile ready_list = nullptr;

    // Tasks which are sleeping
    task_data_t* volatile sleep_list = nullptr;
    
    // List of events which hold list of tasks blocked by the event 
    event_t* volatile event_list = nullptr;
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