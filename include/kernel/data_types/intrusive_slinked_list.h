/**
 * @file intrusive_slinked_list.h
 *
 * @brief Simple intrusive singly linked list implementation.
 * 
 * @note Functions are not ATOMIC. Callee must ensure atomicity if needed.
 *
 * @details Usage:
 * 
 * @code {.c}
 *      struct my_data {
 *          intrusive_slinked_list_node<my_data> list_node;
 *          # own data after this
 *      }
 * 
 *      # List head & accessor: 
 *      intrusive_slinked_list<my_data, &my_data::list_node> my_list;
 * 
 *      # Adding & removing to the list
 *      # where obj is my_data type
 *      my_list.add_tail(&obj);
 *      my_list.remove(&obj); 
 * @endcode
 */
#ifndef _INTRUSIVE_SLINKED_LIST_H_
#define _INTRUSIVE_SLINKED_LIST_H_

#include <kernel/errno.h>

/**
 * @brief Intrusive slinked list node.
 */
template <typename T>
struct intrusive_slinked_list_node
{
    T* next_node = nullptr;
};

/**
 * @brief Intrusive slinkend list accessor.   
 */
template <typename T, intrusive_slinked_list_node<T> T::*node_ptr>
class intrusive_slinked_list
{
protected:
    T* head;

    void _set_next(T* node, T* next_value) {
        (node->*node_ptr).next_node = next_value;
    }

public:
    intrusive_slinked_list() : head(nullptr) {}

    /**
     * @brief Get the head node
     * 
     * @return T* or nullptr if no head
     */
    T* get_head() const {return head;}

    /**
     * @brief Get the next node
     * 
     * @param node 
     * @return T* or nullptr if no next
     */
    T* get_next(T* node) const {
        if (node == nullptr)
            return nullptr;
        
        return (node->*node_ptr).next_node; 
    }

    /**
     * @brief Insert new_node after node
     * @note New_node must not be already linked
     *
     * @param  node: T*
     * @param  new_node: T* 
     * @retval kernel_erno_t:
     *          - KERNEL_OK 
     *          - KERNEL_ERR_NOT_EMPTY   (new_node is linked to other nodes)
     *          - KERNEL_ERR_INVALID_PARAMETER 
     */
    kernel_errno_t insert(T* node, T* new_node) {
        T* tmp_ptr = nullptr;

        if (!node || !new_node)
            return KERNEL_ERR_INVALID_PARAMETER;

        if (get_next(new_node) != nullptr)
            return KERNEL_ERR_NOT_EMPTY;
        
        tmp_ptr = get_next(node);
        _set_next(node, new_node);
        _set_next(new_node, tmp_ptr);

        return KERNEL_OK;
    }

    /**
     * @brief Add node to tail
     * @note new_node must not have nodes linked to it.
     * 
     * @param new_node 
     * @return kernel_errno_t:
     *          - KERNEL_OK
     *          - KERNEL_ERR_INVALID_PARAMETER
     */
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

    /**
     * @brief Remove node from the list
     * 
     * @param node 
     * @return kernel_errno_t:
     *          KERNEL_ERR_INVALID_PARAMETER
     *          KERNEL_ERR_NOT_FOUND
     *          KERNEL_OK
     */
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