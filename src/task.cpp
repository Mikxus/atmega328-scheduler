#include "task.h"

task_data_t volatile *c_task = nullptr;
task_data_t volatile *task_list_head = nullptr;

bool _create_init_task()
{

}

bool create_task(
    task_data_t &task,
    volatile uint8_t *stack_array,
    const uint16_t stack_size,
    const char *name,
    const uint8_t priority,
    const uint8_t slice_ms,
    const void *(entry)(void))
{
    uint8_t sreg = SREG;
    cli();

    #if SCHEDULER_TASK_TRACK_STATISTICS == 1
    task.priority = priority;
    task.time_slice_ms = slice_ms;
    task.exec_time_us = 0;
    task.exec_time_overflow_count = 0;
    task.exec_start_time_us = 0;
    #endif
    task.next_node = nullptr;
    task.stack.memory_ptr = stack_array;
    task.stack.size = stack_size;
    task.state = READY;

    // Initialize linked list
    task.next_node = nullptr;

    // copy name
    if (strlen(name) >= SCHEDULER_TASK_NAME_MAX_LENGTH)
        return 1;

    strcpy(task.name, name);

    // initialize stack pointer to third byte? so [0 - 1] can have entry's pc 
    task.cpu_state.sp = reinterpret_cast<volatile uint16_t>(&stack_array);

    // Set entry to stack
    task.stack.memory_ptr[0] = reinterpret_cast<volatile uint16_t>(entry);

    _add_task(&task);

    /* if init task update c_task */
    if (c_task == nullptr)
        c_task = _get_head_task();

    SREG = sreg;
    return 0;
}


task_data_t volatile *_get_head_task()
{
    return task_list_head;
}

task_data_t volatile *_get_tail()
{
    return _find_preceding_task(nullptr);
}

task_data_t volatile *_get_next_task(task_data_t volatile *task) 
{
    if (task == nullptr)
        return nullptr;

    return task->next_node;
}

/**
 * @brief Finds the preceding node of the target
 * 
 * @param target_node 
 * @return task_data_t* 
 */
task_data_t volatile *_find_preceding_task(task_data_t volatile* target_node)
{
    task_data_t volatile *seek_head = _get_head_task();

    if (seek_head == nullptr)
        return seek_head;

    while (seek_head != target_node && seek_head != nullptr)
    {
        seek_head = _get_next_task(seek_head);
    }
    return seek_head;
}

task_data_t volatile *_fing_task(task_data_t volatile *target_node)
{
    task_data_t volatile *seek_node = _get_head_task();

    if (seek_node == target_node)
        return seek_node;

    while (seek_node != nullptr)
    {
        if (seek_node == target_node)
            break;
        
        seek_node = _get_next_task(seek_node);
    }

    return seek_node;
}

void _add_task(task_data_t volatile *new_node)
{
    task_data_t volatile *ptr;

    if (_get_head_task() == nullptr) {
        task_list_head = new_node;
        return;
    }

    ptr = _get_tail();
    ptr->next_node = new_node;
    return;
}