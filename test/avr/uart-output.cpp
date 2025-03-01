#include <stdio.h>
#include <avr/sleep.h>
#include "src/scheduler.h"

int main(void)
{
    _scheduler_init();

    printf("Hello World!\n");
    uart0_flush();
    
    cli();
    sleep_cpu();
    return 0;
}