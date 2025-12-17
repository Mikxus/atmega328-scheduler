#ifndef _SCHED_H_
#define _SCHED_H_
#include "src/task.h"

void schedule_round_robin(void);


#if CONF_SCHEDULER_ALGO_ROUND_ROBIN == 1
    #define _schedule_next_task() schedule_round_robin()
#else
    #error "No scheduling algorithm defined"
#endif

#endif // _SCHED_H_