#include "tools/unittest.h"
#include <kernel/kernel.h>
#include <kernel/drivers/uart/uart.h>
#include <kernel/drivers/synchronization/mutex.h>

task_data_t task0, task1, task2, task3;
uint8_t stack0[100];
uint8_t stack1[100];
uint8_t stack2[150];
uint8_t stack3[150];
mutex_t mtx;

uint8_t test_count = 3;

/* Random values to test against */
uint8_t rand_values[5] = {0, 2, 12, 116, 123};
float float_value = 1234.02f;
double double_value = 2.2321;
uint64_t u64_value = UINT64_MAX - 1;
struct struct_array
{
    uint8_t array[23] = {
        5, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10,
        11, 12, 13, 14, 15, 16, 17, 18, 19, 20,
        21, 22};
};
struct struct_array pass_by_copy;

/* fn prototypes */
void test_status(uint8_t *count);
void test_1(uint8_t array[5], uint8_t value);
void test_3(float f_val, double d_val, struct struct_array struct_val);
void test_4(float f_val, double d_val, uint64_t u64_1, struct struct_array struct_val, uint8_t u8_1);

int main(void)
{
    cli();
    kernel_init();
    mtx_init(&mtx);
    init_unittest();

    create_task(
        task0,
        &stack0[0],
        sizeof(stack0),
        "test_mgr",
        1,
        1,
        &test_status,
        &test_count);

    create_task(
        task1,
        &stack1[0],
        sizeof(stack1),
        "test_1",
        1,
        1,
        &test_1,
        rand_values,
        rand_values[0]);

    create_task(
        task2,
        &stack2[0],
        sizeof(stack2),
        "test_3",
        1,
        1,
        &test_3,
        float_value,
        double_value,
        pass_by_copy);

    create_task(
        task3,
        &stack3[0],
        sizeof(stack3),
        "test_4",
        1,
        1,
        &test_4,
        float_value,
        double_value,
        u64_value,
        pass_by_copy,
        (uint8_t)u64_value);

    sei();
    uart0_flush();
    kernel_start();
}

void test_status(uint8_t *count)
{
    while (1)
    {
        mtx_lock(&mtx);
        if (*count == 0)
        {
            printf("All tests done!\n");
            uart0_flush();
            mtx_release(&mtx);
            exit_unittest();
        }
        mtx_release(&mtx);
        yield();
    }
}

/*
 * Test receiving array
 * The array decays in to a pointer
 */
void test_1(uint8_t array[5], uint8_t value)
{
    expect(&array[0] == &rand_values[0], "test_1: recv array");
    expect(value == rand_values[0], "test_1: recv u8 value");

    mtx_lock(&mtx);
    test_count -= 1;
    printf("%s: DONE\n", __func__);
    mtx_release(&mtx);
    while (1);
}

void test_3(float f_val, double d_val, struct struct_array struct_val)
{

    expect(f_val == float_value, "recv: float value");
    expect(d_val == double_value, "recv: double value");

    for (uint8_t i = 0; i < sizeof(struct_val.array) / sizeof(struct_val.array[0]); i++)
    {
        expect((struct_val.array[i] == pass_by_copy.array[i]), "Structure passed by copy");
    }

    mtx_lock(&mtx);
    test_count -= 1;
    printf("%s: DONE\n", __func__);
    mtx_release(&mtx);
    while (1);
}

void test_4(
    float f_val,                    /* r22-25 4 bytes, 0 padding */
    double d_val,                   /* r18-21 4 bytes, 0 padding */
    uint64_t u64_1,                 /* r10-17 8 bytes, 0 padding */
    struct struct_array struct_val, /* stack */
    uint8_t u8_1)                   /* stack */
{
    expect(f_val == float_value, "recv: float value");
    expect(d_val == double_value, "recv: double value");
    expect(u64_1 == (uint64_t)u64_value, "recv: u64 value");

    for (uint8_t i = 0; i < sizeof(struct_val.array) / sizeof(struct_val.array[0]); i++)
    {
        expect((struct_val.array[i] == pass_by_copy.array[i]), "Structure passed by copy");
    }

    expect(struct_val.array[0] == pass_by_copy.array[0], "recv: struct array value 0");
    expect(u8_1 == (uint8_t)u64_value, "recv: u8 value");

    mtx_lock(&mtx);
    test_count -= 1;
    printf("%s: DONE\n", __func__);
    mtx_release(&mtx);
    while (1);
}