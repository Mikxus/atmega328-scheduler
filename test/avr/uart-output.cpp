#include <stdio.h>
#include <avr/sleep.h>
#include <avr/interrupt.h>
#include <kernel/kernel.h>
#include <kernel/drivers/uart.h>

int main(void)
{
    kernel_init();
    sei();

    printf("Hello World!\n");
    printf("Hello World!\n");
    printf("Hello World!\n");
    printf("Hello World!\n");
    printf("Hello World!\n");
    uart0_flush();
    
    cli();
    sleep_cpu();
    return 0;
}