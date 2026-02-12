#include <avr/io.h>
#include <avr/cpufunc.h>
#include <kernel/kernel.h>
#include <kernel/errno.h>
#include <kernel/drivers/uart/uart.h>
#include <kernel/drivers/synchronization/mutex.h>

#define STACK_SIZE 128
#define FIFO_SIZE 5

void task_1(void);
void task_2(void);

task_data_t task1, task2;
uint8_t stack1[STACK_SIZE];
uint8_t stack2[STACK_SIZE];
mutex_t mtx;

int main(void)
{
    cli();
    kernel_init();
    DDRB |= (1 << DDB4) | (1 << DDB3);

    mtx_init(&mtx); 

    create_task(
        task1,
        stack1,
        sizeof(stack1),
        "task1",
        1,
        1,
        &task_1);

    create_task(
        task2,
        stack2,
        sizeof(stack2),
        "task1",
        1,
        1,
        &task_2);

    sei();
    kernel_start();
}

void task_1(void)
{
    while (1)
    {
        if (mtx_try_lock(&mtx) != KERNEL_OK)
            continue;

        PORTB |= (1u << PB4);

        for (uint8_t i = 0; i < 255; i++)
            _NOP();

        PORTB &= ~(1 << PB4);
        if (mtx_release(&mtx) == MUTEX_ERR_NOT_OWNER)
        {
            printf("MUTEX_ERR_NOT_OWNER");
            uart0_flush();
        }
    }
}

void task_2(void)
{
    while (1) 
    {
        if (mtx_try_lock(&mtx) != KERNEL_OK)
            continue;

        PORTB |= (1u << PB3);

        for (uint8_t i = 0; i < 255; i++)
            _NOP();

        PORTB &= ~(1 << PB3);
        mtx_release(&mtx);
    }
}