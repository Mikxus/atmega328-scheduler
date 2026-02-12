#include <stdio.h>
#include <avr/sleep.h>
#include <kernel/kernel.h>

int main(void)
{
    kernel_init();

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