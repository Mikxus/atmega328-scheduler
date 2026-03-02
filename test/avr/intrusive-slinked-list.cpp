#include "tools/unittest.h"
#include <kernel/errno.h>
#include <kernel/kernel.h>
#include <kernel/drivers/data_types/intrusive_slinked_list.h>

struct data
{
    slinked_list_node<data> list_node;
    uint8_t numbers;
};

// Create linked list of data
slinked_list<data, &data::list_node> data_list;

struct data a, b, c, d;

int main(void)
{
    kernel_errno_t errno = KERNEL_OK;
    kernel_init();
    init_unittest();

    errno = data_list.add_tail(&a);
    expect(errno == KERNEL_OK, "LIST add a");
    expect(data_list.get_head() == &a, "head equals &a");

    errno = data_list.add_tail(&b);
    expect(errno == KERNEL_OK, "LIST add b");
    expect(data_list.get_head() == &a, "head equals &a");
    expect(data_list.get_next(&a) == &b, "next is b");

    // Remove A
    errno = data_list.remove(&a);
    expect(errno == KERNEL_OK, "Remove A");
    expect(data_list.get_head() == &b, "after A remove B is head");
    expect(data_list.get_next(&a) == nullptr, "A points to nothing");
    expect(data_list.get_next(&b) == nullptr, "B points to nothing");

    // add A back
    // list B, A
    errno = data_list.add_tail(&a);
    expect(errno == KERNEL_OK, "List add a");
    
    // add c
    // list B, A, C
    errno = data_list.add_tail(&c);
    expect(errno == KERNEL_OK, "List add c");

    // remove C
    // B, A
    errno = data_list.remove(&c);
    expect(errno == KERNEL_OK, "list remove c");
    expect(data_list.get_head() == &b, "list head is b");
    expect(data_list.get_next(data_list.get_head()) == &a, "list next is a");
    expect(data_list.get_next(&a) == nullptr, "Third item is nullptr");

    // add D
    // B, A, D
    errno = data_list.add_tail(&d);
    expect(errno == KERNEL_OK, "list add d");

    // remove A
    // B, D
    errno = data_list.remove(&a);
    expect(errno == KERNEL_OK, "list remove a");
    expect(data_list.get_head() == &b, "list head is b");
    expect(data_list.get_next(data_list.get_head()) == &d, "list next is d");

    /* Validate errors */
    // remove item which isn't in the list
    errno = data_list.remove(&a);
    expect(errno == KERNEL_ERR_NOT_FOUND, "A is not in the list");

    // test invalid input
    errno = data_list.remove(nullptr);
    expect(errno == KERNEL_ERR_INVALID_PARAMETER, "test invalid input");

    // Clear list
    errno = data_list.remove(&b);
    expect(errno == KERNEL_OK, "list remove b");
    errno = data_list.remove(&d);
    expect(errno == KERNEL_OK, "list remove d");

    errno = data_list.remove(&d);
    expect(errno == KERNEL_ERR_NOT_FOUND, "Test removing from empty list");
    
    exit_unittest();
    while (1) {}
}