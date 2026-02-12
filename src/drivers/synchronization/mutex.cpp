#include <kernel/drivers/synchronization/mutex.h>

void mtx_init(mutex_t *mtx)
{
    ATOMIC_BLOCK() {
    mtx->owner = nullptr;
    mtx->mtx_event.type = EVENT_MUTEX;
    mtx->mtx_event.next_node = nullptr;
    mtx->mtx_event.task_list = nullptr;
    }
}

kernel_errno_t mtx_lock(mutex_t *mtx)
{
    while (1)
    {
        ATOMIC_BLOCK() {
            if (mtx->owner == nullptr) {
                mtx->owner = get_current_task();
                return KERNEL_OK;
            }

            if (mtx->owner == get_current_task())
                return MUTEX_ERR_RECURSIVE_LOCK;
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

    mtx->owner = c_task;
    return KERNEL_OK;
}

kernel_errno_t mtx_release(mutex_t *mtx)
{
    ATOMIC_GUARD();
    if (mtx->owner != get_current_task()) {
        return MUTEX_ERR_NOT_OWNER;
    }

    mtx->owner = nullptr;
    return KERNEL_OK;
}