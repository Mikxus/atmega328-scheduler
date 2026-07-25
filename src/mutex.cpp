/**
 * @file mutex.cpp
 * @brief  Non recursive mutex with priority inheritance
 */
#include <kernel/kernel.h>
#include <kernel/atomic.h>
#include <kernel/mutex.h>
#include "task_utils.h"
#include "event_priv.h"
#include "drivers/scheduling/sched.h"

void mtx_init(mutex_t *mtx)
{
    ATOMIC_BLOCK() {
    mtx->owner = nullptr;
    mtx->mtx_event.type = EVENT_MUTEX;
    #if SCHEDULER_HAS_PRIORITIES == 1
    mtx->base_priority = 0;
    #endif
    }
}

kernel_errno_t mtx_lock(mutex_t *mtx)
{
    task_data_t* task = get_current_task();

    ATOMIC_BLOCK() {
        if (mtx->owner == task)
            return MUTEX_ERR_RECURSIVE_LOCK;

        if (mtx->owner == nullptr) {
            mtx->owner = task;
            #if SCHEDULER_HAS_PRIORITIES == 1
            mtx->base_priority = mtx->owner->priority;
            #endif
            return KERNEL_OK;
        }

        #if SCHEDULER_HAS_PRIORITIES == 1

        _event_block_task(task, &mtx->mtx_event);

        /* priority inheritance */
        if (mtx->owner->priority < task->priority) {
            mtx->owner->priority = task->priority; // boost priority
        }
        #endif
        yield();
    }
    return KERNEL_OK;
}

kernel_errno_t mtx_release(mutex_t *mtx)
{
    ATOMIC_GUARD();
    if (mtx->owner != get_current_task()) {
        return MUTEX_ERR_NOT_OWNER;
    }

    #if SCHEDULER_HAS_PRIORITIES == 1
    mtx->owner->priority = mtx->base_priority; 
    mtx->owner = _event_unblock_highest_prio(&mtx->mtx_event);

    if (mtx->owner == nullptr)
        return KERNEL_OK;

    mtx->base_priority = mtx->owner->priority;
    if (get_current_task()->priority < mtx->owner->priority)
        yield();

    #endif
    return KERNEL_OK;
}