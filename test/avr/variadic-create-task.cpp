#include "tools/unittest.h"
#include "src/scheduler.h"
#include "src/drivers/synchronization/mutex.h"

void test_1(uint8_t *stack_ptr);
void test_2(uint8_t array[5]);

task_data_t task0, task1;
uint8_t stack0[100];
uint8_t stack1[100];
mutex_t mtx;

uint8_t rand_values[6] = {0, 2, 12, 116, 123};

int main(void)
{
    DDRB = (1 << DDB4) | (1 << DDB3); // Set PB4 and PB3 as output

    cli();
    init_scheduler();
    mtx_init(&mtx);
    init_unittest();

    create_task(
        task0,
        &stack0[0],
        sizeof(stack0),
        "test_1",
        1,
        1,
        &test_1,
        &stack0[0]);

    sei();
    start_scheduler();
}

// test uint8_t ptr passing 
void test_1(uint8_t * stack_ptr) 
{
    while (1) 
    {
        mtx_lock(&mtx);
        printf("%s: recv: stack_ptr %p\n", __func__, stack_ptr);
        uart0_flush();

        if (stack_ptr != &stack0[0]) {
            printf("%s: FAIL: %p != %p\n", __func__, stack_ptr, &stack0[0]);
            exit_unittest();
        }

        create_task(
            task1,
            &stack1[0],
            sizeof(stack1),
            "test_2",
            1,
            1,
            &test_2,
            rand_values);
        mtx_release(&mtx);
        yield();
    }
}

void test_2(uint8_t array[5]) 
{
    
    while (1)
    {
        mtx_lock(&mtx);
        printf("%s: recv: array ptr: %p\n", __func__, &array[0]);
        uart0_flush();

        if (&array[0] != &rand_values[0]) {
            printf("%s: FAIL: %p != %p\n", __func__, &array[0], &rand_values[0]);
            exit_unittest();
        }

        for (uint8_t i = 0; i < 5; i++)
        {
            printf("%d, ", array[i]);
        }
        printf("\n");

        exit_unittest();
        create_task(
            task0,
            &stack0[0],
            sizeof(stack0),
            "test_1",
            1,
            1,
            &test_2,
            rand_values);
        mtx_release(&mtx);
        yield();

    }
}