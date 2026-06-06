#ifndef _FIFO_H_
#define _FIFO_H_

#include <stdlib.h>

template <typename T>
struct fifo_t
{
    T *buffer;
    uint8_t size;
    uint8_t count;
    uint8_t head;
    uint8_t tail;

    bool increment_head()
    {
        // if full
        if (get_used_size() == size)
            return 1;

        head = (head + 1) % size;
        count += 1;
        return 0;
    }

    bool increment_tail()
    {
        // empty
        if (get_used_size() == 0)
            return 1;

        tail = (tail + 1) % size;
        count -= 1;
        return 0;
    }

public:
    void init(T *buf, uint8_t n)
    {
        buffer = buf;
        size = n;
        head = 0;
        tail = 0;
        count = 0;
    }

    /**
     * @brief Get fifo max size
     * 
     * @return size_t 
     */
    int get_size()
    {
        return size;
    }

    /**
     * @brief Get count of items in fifo
     * 
     */
    int get_used_size()
    {
        return count;
    }

    bool is_full()
    {
        return get_used_size() == size;
    }

    /**
     * @brief Push item
     * 
     * @param item 
     * @return true if fifo is full
     * @return false 
     */
    bool enqueue(const T &item)
    {
        if (buffer == nullptr)
            return 1;
        
        if (get_used_size() >= size)
            return 1;

        buffer[head] = item;
        increment_head();
        return 0;
    }

    bool dequeue(T &output)
    {
        if (get_used_size() == 0)
            return 0; // idk nothing to return
        
        output = buffer[tail];
        increment_tail();
        return 0;
    }
};

#endif // _FIFO_H_