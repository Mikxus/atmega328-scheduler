#include <kernel/drivers/synchronization/mutex.h>
#include "../scheduling/sched.h"

void mtx_init(mutex_t *mtx)
{
    ATOMIC_BLOCK() {
    mtx->owner = nullptr;
    mtx->mtx_event.type = EVENT_MUTEX;
    #if SCHEDULER_HAS_PRIORITIES == 1
    mtx->priority = 0;
    mtx->fifo.init(mtx->mtx_event.task_arr, CONF_EVENT_MAX_TASK_CNT);
    #endif
    }
}

kernel_errno_t mtx_lock(mutex_t *mtx)
{
    ATOMIC_GUARD();
    if (mtx->owner == get_current_task())
        return MUTEX_ERR_RECURSIVE_LOCK;

    while (1)
    {
        ATOMIC_BLOCK() {
            if (mtx->owner == nullptr || mtx->owner == get_current_task()) {
                mtx->owner = get_current_task();
                #if SCHEDULER_HAS_PRIORITIES == 1
                mtx->priority = mtx->owner->priority;
                #endif
                return KERNEL_OK;
            }

            #if SCHEDULER_HAS_PRIORITIES == 1
            /* mtx is locked */
            _sched_lists.ready_list.remove(get_current_task());
            mtx->fifo.enqueue(get_current_task());

		    /* priority inheritance */
            if (mtx->owner->priority < get_current_task()->priority) {
                mtx->priority = mtx->owner->priority;
                mtx->owner->priority = get_current_task()->priority;
			}
            #endif
        }
        yield();
    }
}

kernel_errno_t mtx_try_lock(mutex_t *mtx)
{
    ATOMIC_GUARD();

    if (mtx->owner == get_current_task())
        return MUTEX_ERR_RECURSIVE_LOCK;

    if (mtx->owner != nullptr)
        return MUTEX_ERR_LOCKED;
    
    #if SCHEDULER_HAS_PRIORITIES == 1
    if (mtx->fifo.get_used_size() != 0)
        return MUTEX_ERR_LOCKED;
    #endif

    mtx->owner = get_current_task();
    return KERNEL_OK;
}

kernel_errno_t mtx_release(mutex_t *mtx)
{
    ATOMIC_GUARD();
    if (mtx->owner != get_current_task()) {
        return MUTEX_ERR_NOT_OWNER;
    }

    #if SCHEDULER_HAS_PRIORITIES == 1
    mtx->owner->priority = mtx->priority; 
    #endif
    mtx->owner = nullptr;

    #if SCHEDULER_HAS_PRIORITIES == 1
    if (mtx->fifo.get_used_size() == 0)
        return KERNEL_OK;
    
    /* Get next highest priority owner */
    mtx->owner = mtx->fifo.dequeue();
    mtx->priority = mtx->owner->priority;
    _sched_lists.ready_list.add_tail(mtx->owner);
    #endif
    return KERNEL_OK;
}