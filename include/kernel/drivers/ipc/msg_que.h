/**
 * @file msq_que.h  
 * @note   
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
    fifo_t<T> fifo;
    event_t rx_event;
    event_t tx_event;

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

    void enqueue(const T &item)
    {
        bool err;
        ATOMIC_BLOCK() {
            if (fifo.is_full()) {
                _event_block_task(get_current_task(), &tx_event);
                yield();
            }

            if (!_is_event_empty(&rx_event)) {
                _event_unblock_highest_prio(&rx_event);
            }

            fifo.enqueue(item);
        }
        return;
    }

    void dequeue(T &output)
    {
        ATOMIC_BLOCK() {
            if (fifo.get_used_size() == 0) {
                _event_block_task(get_current_task(), &rx_event);
                yield();
            }

            if (!_is_event_empty(&tx_event)) {
                _event_unblock_highest_prio(&tx_event);
            }

            fifo.dequeue(output);
        }
        return;
    }
};

#endif // _MSG_QUE_H_