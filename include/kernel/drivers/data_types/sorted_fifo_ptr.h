/**
 * @file sorted_fifo_ptr.h
 * @brief Sorted fifo implementation
 *      Keeps fifo ordered to some value eq, priority
 *      Higher priority gets precedence
 */
#ifndef _SORTED_FIFO_H_
#define _SORTED_FIFO_H_

#include <stddef.h>
#include <stdint.h>

/**
 * @brief Sorted fifo of pointers, Sorts by data's value_ptr  
 * @note returns the highest value_ptr's ptr   
 */
template<typename T, typename valueType, valueType T::*value_ptr>
class sorted_fifo_ptr_t
{
    T** data;
    uint8_t size;
    uint8_t count;

public:
    sorted_fifo_ptr_t() : data(nullptr), size(0), count(0) {}

    void init(T** buf, uint8_t n)
    {
        data = buf;
        size = n;
        count = 0;
    }

    /**
     * @brief Get is full  
     * @note   
     * @retval Bool
     */
    bool is_full() const { return count == size; }

    /**
     * @brief Get used size  
     * @note   
     * @retval 
     */
    uint8_t get_used_size() const { return count; }

    valueType get_highest_value() {
        if (data == nullptr)
            return 0;
        
        if (count == 0)
            return 0;

        return data[0]->*value_ptr;
    }

    /**
     * @brief Insert pointer & keep array sorted
     * @note   
     * @param  item: 
     * @retval 0 on success, 1 on failure
     */
    bool enqueue(T* item)
    {
        if (data == nullptr)
            return 1;

        if (count == size)
            return 1;

        uint8_t insert_idx = count;
        while (insert_idx > 0 && ((data[insert_idx - 1]->*value_ptr) < (item->*value_ptr)))
        {
            data[insert_idx] = data[insert_idx - 1];
            --insert_idx;
        }
        data[insert_idx] = item;
        ++count;
        return 0;
    }

    /**
     * @brief Remove and return highest-priority pointer  
     * @note   Or nullptr if empty
     * @retval 
     */
    T* dequeue()
    {
        if (count == 0)
            return nullptr;

        T* item = data[0];
        for (uint8_t i = 1; i < count; ++i)
            data[i - 1] = data[i];

        --count;
        return item;
    }
};

#endif

