#include "sched.h"
#include <avr/interrupt.h>
#include <avr/sleep.h>

#include "../../task_utils.h"
#include "../../kernel_priv.h"
#include <kernel/drivers/timer.h> 

scheduler_lists_t _sched_lists;

/**
 * @brief Pick next task round robin
 */
void _sched_pick_next_rr(void) 
{
    uint32_t last_exec_start_us = UINT32_MAX;
    task_data_t* node;
    task_data_t* result_node;
    node = _sched_lists.ready.get_head();

    if (node == nullptr) {
        disable_timer0_interrupt(tmr0_int_t::COMPB_INTERRUPT);
        do 
        {
            sei();
            asm volatile ("nop" ::: "memory");
            cli();
            node = _sched_lists.ready.get_head();
        } while (node == nullptr);

        enable_timer0_interrupt(tmr0_int_t::COMPB_INTERRUPT);
    }

    while (node != nullptr) {
        if (node->exec_start_time_us <= last_exec_start_us && node->state == READY) {
            last_exec_start_us = node->exec_start_time_us; 
            result_node = node;
        }
        
        node = _sched_lists.ready.get_next(node);
    }

    _c_task = result_node;
}

/**
 * @brief Picks task priority round robin
 * 
 * @details
 *      Scheduler's ready list is always sorted from high to lowest priority.
 *      Where the highest priority is always at start of the list e.g HEAD.
 *      When new task becomes unblocked, it will be inserted to the list
 *      followingly:
 *          - If it has higher priority than the highest task in the list:
 *              = It will be inserted at HEAD and will point to previously highest
 *              task.
 *          - If it has same priority:
 *              = It will be inserted behind the last task with the same priority
 * @par Scheduling: 
 *      We will always execute the highest priority task in the ready list.
 *      If there multiple tasks with the same priority, they will be scheduled
 *      as round robin. 
 * 
 * @note Higher priority task will block lower priority tasks until it get's removed
 *      from ready list.
 */
void _sched_pick_next_prr(void)
{
    task_data_t* head = _sched_lists.ready.get_head();

    if (head == nullptr) {
        /*
         * No ready tasks. Disable context switch and busy-wait till new tasks
         * are unblocked.
         * 
         * We do not use sleep here due to hardware quirk. According to the 
         * Atmega328P datasheet (sec 9.3 Idle mode) idle mode forces ADC
         * conversion if the ADC is enabled. Pair this with ADC interrupt
         * possibly being enabled will cause spurious loop of:
         *      triggering adc -> interrupt -> wakeup.
         *
         * Possibly completely mangling user's sampling rate.
         */
        disable_timer0_interrupt(tmr0_int_t::COMPB_INTERRUPT);

        do {
            sei();
            asm volatile ("nop" ::: "memory");
            cli();
            head = _sched_lists.ready.get_head();
        } while (head == nullptr);

        enable_timer0_interrupt(tmr0_int_t::COMPB_INTERRUPT);
    }

    if (
        _c_task != nullptr 
        && _c_task == head	/* previously executed task should be at head if  */
    ) {						/* it hasn't been unscheduled                     */
        /**
		 * TODO: Worst case O(n) to insert back. Remove is O(1) since we're
		 * removing the head.
         * will need to improve this.
		 * Thoughts:
		 * 
		 * 	-  ready list could for example keep track of the highest priority group's
		 * 	  tail position --> most cases O(1) insert.
		 * 			-	When previous task unblocks higher prioity --> 
		 * 				would lead to the Tailptr to different priority group's tail.
		 * 				so we would still need to search the list after the tail ptr)
		 * 
		 * - We could make ready list doubly linked.
		 * 		- O(1) remove and insert
		 * 		- Less logic to implement
		 * 		- Would 2x list node's memory usage 2b -> 4b
		 * 			Realistically this propably is accetable, since at most
		 * 			we'll have 1 to 6 tasks
		 * 
		 * - We could limit priority levels to 0-4 and have own list for each priority
		 *  and bitmask to tell which priority levels are occupied. Then the list 
		 *  would need to additionally keep track of tail. This would allow O(1)
		 * 	removing and adding.
		 */
        _sched_lists.ready.remove(head);
        
        _sched_lists.ready.add(head);
    }

    _c_task = _sched_lists.ready.get_head();
    return;
}