#include <avr/sleep.h>
#include "src/drivers/data_types/fifo.h"
#include "src/scheduler.h"
#include "tools/unittest.h"

#define FIFO_SIZE 3

int main(void)
{
    fifo_t<uint8_t> fifo;
    uint8_t buffer[FIFO_SIZE];
    uint8_t temp = 0;

    init_unittest();
    initialize_uart();


    fifo.init(buffer, FIFO_SIZE);

    // add items to fifo
    fifo.enqueue(10);
    fifo.enqueue(20);
    fifo.enqueue(30);

    expect_equal(3, fifo.get_used_size(), "FIFO should be full");
    expect_equal(1, fifo.enqueue(40), "Enqueue should fail when FIFO is full");
    expect_equal(10, buffer[0], "buffer[0] should be 10");
    expect_equal(20, buffer[1], "buffer[1] should be 20");
    expect_equal(30, buffer[2], "buffer[2] should be 30");

    // remove one item & add another 
    expect_equal(10, fifo.dequeue(), "Dequeue should return first item (10)");
    expect_equal(fifo.get_used_size(), 2, "FIFO should have two items after dequeue");
    expect_equal(fifo.enqueue(40), 0, "Enqueue should succeed after dequeue");
    expect_equal(fifo.get_used_size(), 3, "FIFO should be full again");

    // check if contents are correct
    expect_equal(40, buffer[0], "buffer[0] should be 40");
    expect_equal(20, buffer[1], "buffer[1] should be 20");
    expect_equal(30, buffer[2], "buffer[2] should be 30");

    expect_equal(20, fifo.dequeue(), "Dequeue should return second item (20)");
    expect_equal(30, fifo.dequeue(), "Dequeue should return third item (30)");
    expect_equal(40, fifo.dequeue(), "Dequeue should return fourth item (40)");

    exit_unittest();
}