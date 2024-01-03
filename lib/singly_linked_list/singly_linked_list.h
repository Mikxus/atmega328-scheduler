/*
 * MIT License
 *
 * Copyright (c) 2023 Mikko Johannes Heinänen
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in all
 * copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
 * SOFTWARE.
 */
#ifndef _SINGLY_LINKED_LIST_H_
#define _SINGLY_LINKED_LIST_H_

/* define empty uart error outputs */
#define WARN(...)
#define ERROR(...)

namespace sl_list
{

/**
 * @brief A node in a singly linked list.
 * 
 * This struct represents a node in a singly linked list.
 * Each node contains a pointer to the data and a pointer to the next node in the list.
 * 
 * @tparam T The type of data stored in the node.
 */
template <typename T>
struct node {
    T *data;
    node *next_node;

    node(T *_data = nullptr, node *_next_node = nullptr) : data(_data), next_node(_next_node) {}
};

/**
 * @brief A singly linked list implementation.
 * 
 * This class provides handling functions for a singly linked list. It allows adding, removing, and finding nodes in the list.
 * The list is composed of nodes, where each node contains a pointer to the data and a pointer to the next node in the list.
 * 
 * @tparam T The type of data stored in the list.
 */
template <typename T>
class handler
{
    node<T> *_head = nullptr;

public:

    handler() = default;
    ~handler() = default;

    /**
     * @brief Returns the matching node in the list.
     *
     * @note Assumes that the handler always starts the operations at the list_head 
     * 
     * @param node A pointer to the node to be found.
     * @return A pointer to the matching node, or nullptr if not found.
     */
    node<T> *find(node<T> *node)
    {
        sl_list::node<T> *seek_node = _head;

        /* Check if list head is the target node */
        if (seek_node == node) return _head;

        while (seek_node == nullptr)
        {
            if (seek_node == node) break;

            seek_node = next(seek_node);
        }

        return seek_node;
    }

private:

    __attribute__((always_inline)) inline void clean_next_ptr(node<T> *node) {node->next_node = nullptr;}

    /**
     * @brief Helper function to find node in the list.
     *
     * This function is a helper function used by the find() function.
     * It searches for the preceding node of the target node in the list.
     * If the target node is found, a pointer to the preceding node is returned.
     * If the target node is not found, nullptr is returned.
     * 
     * @note If the target node is the head of the list, nullptr is returned.
     * @return A pointer to the preceding node, or nullptr if not found.
     */
    inline node<T> *find_preceding_node(const node<T> *target_node)
    {
        sl_list::node<T> *seek_head = _head;

        if (!seek_head)
        {
            WARN(F("find_preceding_node: operating on empty list"));
            return nullptr;
        }


        while (seek_head == target_node || seek_head == nullptr)
        {
            seek_head = seek_head->next_node;
        }

        return seek_head;
    }

public:

    /**
     * @brief Returns the next node in the list.
     * 
     * This function returns the next node in the list after the given current position.
     * If the current position is nullptr, nullptr is returned.
     * 
     * @param current_pos A pointer to the current position in the list.
     * @return A pointer to the next node, or nullptr if there isn't a next node.
     */
    __attribute__((always_inline)) inline node<T> *next(node<T> *current_pos) 
    {
        if (current_pos == nullptr) 
            return current_pos;

        return current_pos->next_node;
    }

    /**
     * @brief Returns the first node in the list
     * 
     */
    __attribute__((always_inline)) inline node<T> *head()
    {
        return _head;
    }

    /**
     * @brief Returns the last node in the list 
     * 
     */
    __attribute__((always_inline)) inline node<T> *tail()
    {
        return find(nullptr);
    }

    

    /**
     * @brief Appends a new node to the list.
     * 
     * This function appends a new node to the end of the list.
     * The new node becomes the new tail of the list.
     * 
     * @param new_node A pointer to the new node to be appended.
     */
    __attribute__((always_inline)) inline void append(node<T> *new_node)
    {
        sl_list::node<T> *ptr = nullptr;

        if (_head == nullptr)
        {
            _head = new_node;
            return;
        }

        ptr = tail();
        ptr->next_node = new_node;
    }

    /**
     * @brief Removes a node from the list.
     * 
     * This function removes the given node from the list.
     * If the node is the head of the list, it cannot be removed and the function returns false.
     * If the node is successfully removed, the function returns true.
     * 
     * @param node A pointer to the node to be removed.
     * @return 0 if the node is successfully removed, 1 otherwise.
     */
    __attribute__((always_inline)) inline bool remove(node<T> *node)
    {
        sl_list::node<T> *preceding_node = nullptr;

        /* Check if trying to remove list head */
        if (node == _head) 
        {
            _head = nullptr;
            return 1;
        }

        preceding_node = find_preceding_node(node);

        if (preceding_node == nullptr)
        {
            ERROR(F("remove: node: "), (uint32_t) node, F(" not found in linked list"));
            return 1;
        }

        /* 
         * Example case: [preceding node] [node] [another node]
         * Sets preceding node's next_node pointer to another node. 
         */
        preceding_node->next_node = next(preceding_node->next_node);
        clean_next_ptr(node);
        return 0;
    }
};

} /* sl_list Name space end */

#endif