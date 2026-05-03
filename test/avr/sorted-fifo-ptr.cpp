#include <avr/sleep.h>
#include <kernel/drivers/data_types/sorted_fifo_ptr.h>
#include <kernel/task.h>
#include "tools/unittest.h"

struct test {
    uint8_t value;
};

struct test testA;
struct test testB;
struct test testC;
struct test testD;

#define FIFO_SIZE 3

int main(void)
{
    testA.value = 20;
    testB.value = 30;
    testC.value = 40;
    testD.value = 10;

    sorted_fifo_ptr_t<volatile test, uint8_t, &test::value> sfifo;

    volatile struct test* buffer[FIFO_SIZE];

    init_unittest();
    initialize_uart();


    sfifo.init(buffer, FIFO_SIZE);

    // add items to fifo
    sfifo.enqueue(&testA);
    sfifo.enqueue(&testB);
    sfifo.enqueue(&testC);

    expect_equal(3, sfifo.get_used_size(), "FIFO should be full");
    expect_equal(1, sfifo.enqueue(&testD), "Enqueue should fail when FIFO is full");
    expect_equal(40, buffer[0]->value, "buffer[0] should be highest value");
    expect_equal(30, buffer[1]->value, "buffer[1] should be middle value");
    expect_equal(20, buffer[2]->value, "buffer[2] should be lowest value");

    // remove one item & add another 
    expect_equal(40, sfifo.dequeue()->value, "Dequeue should return first item (40)");
    expect_equal(sfifo.get_used_size(), 2, "FIFO should have two items after dequeue");
    expect_equal(sfifo.enqueue(&testD), 0, "Enqueue should succeed after dequeue");
    expect_equal(sfifo.get_used_size(), 3, "FIFO should be full again");

    // check if contents are correct
    expect_equal(30, buffer[0]->value, "buffer[0] should be 30");
    expect_equal(20, buffer[1]->value, "buffer[1] should be 20");
    expect_equal(10, buffer[2]->value, "buffer[2] should be 10");

    expect_equal(30, sfifo.dequeue()->value, "Dequeue should return second item (30)");
    expect_equal(20, sfifo.dequeue()->value, "Dequeue should return third item (20)");
    expect_equal(10, sfifo.dequeue()->value, "Dequeue should return fourth item (10)");

    exit_unittest();
}