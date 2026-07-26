/**
 * @file sched.h  
 * @brief   Selects different scheduling algorithms from defined definitions.
 *
 */
#ifndef _SCHED_H_
#define _SCHED_H_

#include <kernel/task.h>
#include <kernel/event.h>
#include <kernel/data_types/intrusive_slinked_list.h>
#include <kernel/data_types/sched_slinked_list.h>

typedef struct
{
    // Tasks which are ready to be scheduled
    sorted_slinked_list<
        task_data_t,
        &task_data_t::next_node,
        &task_data_t::priority
        > ready;

    // Tasks which are sleeping
    //intrusive_slinked_list<task_data_t, &task_data_t::next_node> sleep_list;
    
    // List of events which hold list of tasks blocked by the event 
    intrusive_slinked_list<event_t, &event_t::next_event> event;
} scheduler_lists_t;

extern scheduler_lists_t _sched_lists;

void _sched_pick_next_rr(void);

void _sched_pick_next_prr(void);

#if CONF_SCHED_RR == 1
    /* Round robin*/
    #define _schedule_next_task() _sched_pick_next_rr()
#elif CONF_SCHED_PRR == 1
    /* Priority round robin */
    #define _schedule_next_task() _sched_pick_next_prr()
#else
    #error "No scheduling algorithm defined"
#endif

void _add_event_to_event_list();

#endif // _SCHED_H_