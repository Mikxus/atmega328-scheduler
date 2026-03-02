/**
 * @file intrusive_slinked_list.h
 * @brief Simple intrusive singly linked list implementation
 * @note Pointers are volatile
 *      Functions are not ATOMIC. Callee must ensure atomicity if needed
 * 
 *      Usage:
 *      struct my_data {
 *          slinked_list_node<my_data> list_node;
 *          # own data after this
 *      }
 * 
 *      # List head & accessor: 
 *      slinked_list<my_data, &my_data::list_node> my_list;
 * 
 *      # Adding & removing to the list
 *      # where obj is my_data type
 *      my_list.add_tail(&obj);
 *      my_list.remove(&obj); 
 */
#ifndef _INTRUSIVE_SLINKED_LIST_H_
#define _INTRUSIVE_SLINKED_LIST_H_

#include <kernel/errno.h>

template <typename T>
struct slinked_list_node
{
    T* volatile next_node;
};

template <typename T, slinked_list_node<T> T::*node_ptr>
class slinked_list
{
    T* volatile head;

    void _set_next(T* node, T* next_value) {
        (node->*node_ptr).next_node = next_value;
    }

public:
    slinked_list() : head(nullptr) {}

    T* get_head() const {return head;}

    T* get_next(T* node) const {
        if (node == nullptr)
            return nullptr;
        
        return (node->*node_ptr).next_node; 
    }

    kernel_errno_t add_tail(T* new_node)
    {
        if (new_node == nullptr)
            return KERNEL_ERR_INVALID_PARAMETER;
        
        if (head == nullptr) {
            head = new_node;
            _set_next(new_node, nullptr);
            return KERNEL_OK;
        }

        T* tail = head;
        while (get_next(tail) != nullptr) {
            tail = get_next(tail);
        }

        // Add new node to tail
        _set_next(tail, new_node);
        // ensure last node points to nullptr
        _set_next(new_node, nullptr);
        return KERNEL_OK;
    }

    kernel_errno_t remove(T* node)
    {
        if (node == nullptr)
            return KERNEL_ERR_INVALID_PARAMETER;
        
        if (head == nullptr)
            return KERNEL_ERR_NOT_FOUND;

        if (head == node)
        {
            head = get_next(node);
            _set_next(node, nullptr);
            return KERNEL_OK;
        }
        
        T* prev_node = head;
        while (get_next(prev_node) != nullptr && get_next(prev_node) != node)
        {
            prev_node = get_next(prev_node);
        }

        if (get_next(prev_node) != node)
            return KERNEL_ERR_NOT_FOUND;

        _set_next(prev_node, get_next(node));
        _set_next(node, nullptr);
        return KERNEL_OK;
    }
};

#endif // _INTRUSIVE_SLINKED_LIST_H_