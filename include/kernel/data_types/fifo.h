/**
 * @brief Simple template fifo implementation.
 * @note Is **not atomic**.  
 */
#ifndef _FIFO_H_
#define _FIFO_H_

#include <stdint.h>
#include <kernel/errno.h>

template <typename T>
struct fifo_t
{
private:
    T *buffer       = nullptr;
    uint8_t size    = 0;
    uint8_t count   = 0;
    uint8_t head    = 0;
    uint8_t tail    = 0;

    bool increment_head()
    {
        if (get_used_size() == size)
            return 1;

        head = (head + 1) % size;
        count += 1;
        return 0;
    }

    bool increment_tail()
    {
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
     */
    int get_size()
    {
        return size;
    }

    /**
     * @brief Get count of items in fifo
     */
    int get_used_size()
    {
        return count;
    }

    /**
     * @brief Get boolean wether fifo is full  
     * @note   
     * @retval 1 if full
     */
    bool is_full()
    {
        return get_used_size() == size;
    }

    /**
     * @brief Add item  
     * @param  &item: 
     * @retval kernel_errno_t
     *          - KERNEL_OK
     *          - KERNEL_ERR_FULL
     *          - KERNEL_ERR_INVALID_PARAMETER
     */
    kernel_errno_t enqueue(const T &item)
    {
        if (buffer == nullptr)
            return KERNEL_ERR_INVALID_PARAMETER;
        
        if (get_used_size() >= size)
            return KERNEL_ERR_FULL;

        buffer[head] = item;
        increment_head();
        return KERNEL_OK;
    }

    /**
     * @brief Dequeue item  
     * @note Return value indicates wether dequeue was succesfull
     * @param  &output: 
     * @retval  kernel_errno_t:
     *          - KERNEL_OK
     *          - KERNEL_ERR_EMPTY
     */
    kernel_errno_t dequeue(T &output)
    {
        if (get_used_size() == 0)
            return KERNEL_ERR_EMPTY;
        
        output = buffer[tail];
        increment_tail();
        return KERNEL_OK;
    }
};

#endif // _FIFO_H_