/**
 * @file msq_que.h  
 * @brief   
 * @retval None
 */

#ifndef _MSG_QUE_H_
#define _MSG_QUE_H_

#include <stdlib.h>
#include <kernel/kernel.h>
#include <kernel/event.h>
#include "../src/event_priv.h"
#include <kernel/drivers/data_types/fifo.h>

template <typename T>
struct msg_que_t
{
    fifo_t<T> fifo = {0};
    event_t rx_event = {0};
    event_t tx_event = {0};

public:
    void init(T* buffer, uint8_t size)
    {
        fifo.init(buffer, size);
        rx_event.type = EVENT_MSG_QUE;
        tx_event.type = EVENT_MSG_QUE;
        return;
    }

    uint8_t get_size()
    {
        int size;
        ATOMIC_BLOCK() {
            size = fifo.get_size();
        }
        return size;
    }

    uint8_t get_used_size()
    {
        uint8_t size;
        ATOMIC_BLOCK() {
            size = fifo.get_used_size();
        }
        return size; 
    }

    bool is_full()
    {
        bool value;
        ATOMIC_BLOCK() {
            value = fifo.is_full();
        }
        return value;
    }

    /**
     * @brief Enqueue from isr  
     * @note  If yield_pend is set to true you must use yield
     *        at the end of isr.
     * 
     * 
     * @param  &item: 
     * @retval  KERNEL_OK
     *          KERNEL_ERR_FULL
     */
    kernel_errno_t enqueueISR(const T &item, bool &yield_pend) {
        task_data_t* unblocked;

        if (fifo.is_full()) {
            return KERNEL_ERR_FULL;
        }

        fifo.enqueue(item);

        if (!_is_event_empty(&rx_event)) {
            unblocked = _event_unblock_highest_prio(&rx_event);

            if (unblocked->priority > get_current_task()->priority) {
                yield_pend = true;
            }
        }

        return KERNEL_OK;
    }

    /**
     * @brief Dequeue from isr  
     * @note  If yield_pend is set to true you must use yield
     *        at the end of isr.
     * 
     * 
     * @param  &item: 
     * @retval  KERNEL_OK
     *          KERNEL_ERR_EMPTY
     */
    kernel_errno_t dequeueISR(T &output, bool &yield_pend) {
        task_data_t* unblocked;

        if (fifo.get_used_size() == 0) {
            return KERNEL_ERR_EMPTY;
        }

        fifo.dequeue(output);

        if (!_is_event_empty(&tx_event)) {
            unblocked = _event_unblock_highest_prio(&tx_event);

            if (unblocked->priority > get_current_task()->priority) {
                yield_pend = true;
            }
        }

        return KERNEL_OK;
    }

    /**
     * @brief Enqueue new message into the que
     * @note   Blocks if message que is full
     * @param  &item: 
     * @retval None
     */
    void enqueue(const T &item)
    {
        task_data_t* unblocked = nullptr; 
        task_data_t* ctask = get_current_task();

        ATOMIC_BLOCK() {
            while (true) {
                if (fifo.is_full()) {
                    _event_block_task(ctask, &tx_event);
                    yield();
                    /*
                     * Re-evaluate if fifo is full. 
                     * Higher priority task could've stolen our spot.
                     */
                    continue;
                }
                break;
            }

            fifo.enqueue(item);

            if (!_is_event_empty(&rx_event)) {
                unblocked = _event_unblock_highest_prio(&rx_event);

                if (unblocked->priority > ctask->priority)
                    yield();
            }
        }
        return;
    }

    /**
     * @brief   Dequeue message from the que
     * @note    Possible yield or block:
     *              - if message que is empty
     *              - Higher priority task becomes available
     *  
     * @param  &output: 
     * @retval None
     */
    void dequeue(T &output)
    {
        task_data_t* unblocked = nullptr; 
        task_data_t* ctask = get_current_task();

        ATOMIC_BLOCK() {
            while (true) {
                if (fifo.get_used_size() == 0) {
                    _event_block_task(ctask, &rx_event);
                    yield();
                    /*
                     * Re-evaluate if fifo is empty
                     * Higher priority task could've stolen our spot.
                     */
                    continue;
                }
                break;
            }

            fifo.dequeue(output);

            if (!_is_event_empty(&tx_event)) {
                unblocked = _event_unblock_highest_prio(&tx_event);

                if (unblocked->priority > ctask->priority)
                    yield();
            }
        }
        return;
    }
};

#endif // _MSG_QUE_H_