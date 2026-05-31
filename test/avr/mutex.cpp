#include <avr/io.h>
#include <avr/cpufunc.h>
#include <kernel/kernel.h>
#include <kernel/errno.h>
#include <kernel/drivers/uart/uart.h>
#include <kernel/drivers/gpio/gpio.h>
#include <kernel/drivers/synchronization/mutex.h>
#include "tools/unittest.h"

#define STACK_SIZE 200

void mutex_test(uint8_t pin_num);
void unittest_exit(void);

task_data_t task1, task2, task3;
uint8_t stack1[STACK_SIZE];
uint8_t stack2[STACK_SIZE];
uint8_t stack3[STACK_SIZE];
mutex_t mtx;

int main(void)
{
    cli();
    kernel_init();
    init_unittest();

    DDRB |= (1 << DDB4) | (1 << DDB3);

    mtx_init(&mtx); 

    create_task(
        task1,
        stack1,
        sizeof(stack1),
        PSTR("task1"),
        1,
        1,
        &mutex_test,
        (uint8_t) PB4);

    create_task(
        task2,
        stack2,
        sizeof(stack2),
        PSTR("task2"),
        1,
        1,
        &mutex_test,
        (uint8_t) PB3);

    create_task(
        task3,
        stack3,
        sizeof(stack2),
        PSTR("task3"),
        1,
        1,
        &unittest_exit);

    sei();
    kernel_start();
}

void mutex_test(uint8_t pin_num)
{
    kernel_errno_t retval = KERNEL_OK;

    while (1)
    {
        if ((retval = mtx_lock(&mtx)) != KERNEL_OK) {
            printf("%s: mtx_lock failed: %d\n",
                &get_current_task()->name[0], retval);
            fail_test("mtx_lock failed");
            return;
        }

        PORTB |= (1u << pin_num);

        for (uint8_t i = 0; i < 255; i++)
            _NOP();

        PORTB &= ~(1 << pin_num);
        if ((retval = mtx_release(&mtx)) != KERNEL_OK)
        {
            printf("%s: mtx_release failed: %d\n",
                &get_current_task()->name[0], retval);
            fail_test("mtx_release failed");
            while(1){}
        }
    }
}

void unittest_exit() 
{
    set_gpio_mode(IO_PORTB, PB2, INPUT);
    while (1) {
        if (read_gpio(IO_PORTB, PB2)) {
            printf("Exiting test\n");
            uart0_flush();
            exit_unittest();
        }

        yield();
    }
}