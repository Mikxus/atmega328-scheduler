#include "task.h"

task_data_t * volatile c_task = nullptr;
task_data_t * volatile task_list_head = nullptr;


uint16_t _get_task_stack_size(task_data_t *task)
{
    return (size_t) task->stack.size;
}

uint16_t _get_task_stack_usage(task_data_t *task)
{
    // stack top - sp
    return (uint16_t) &task->stack.memory_ptr[task->stack.size - 1] - task->cpu_state.sp;
}

task_data_t* _get_head_task()
{
    return task_list_head;
}

task_data_t* _get_tail()
{
    return _find_preceding_task(nullptr);
}

task_data_t* _get_next_task(task_data_t* task) 
{
    if (task == nullptr)
        return nullptr;

    return task->next_node;
}

task_data_t* _find_preceding_task(task_data_t* target_node)
{
    task_data_t *seek_head = _get_head_task();

    if (seek_head == nullptr)
        return seek_head;

    while (seek_head->next_node != target_node && seek_head != nullptr)
    {
        seek_head = _get_next_task(seek_head);
    }
    return seek_head;
}

task_data_t* _find_task(task_data_t* target_node)  // Fixed typo: _fing_task
{
    task_data_t* seek_node = _get_head_task();

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

void _add_task(task_data_t* new_node)
{
    task_data_t* ptr;

    if (_get_head_task() == nullptr) {
        task_list_head = new_node;
        return;
    }

    ptr = _get_tail();
    ptr->next_node = new_node;
    return;
}