#include <stdio.h>
#include <avr/sleep.h>
#include "src/scheduler.h"

#define STRINGIZE(x) #x
#define STRINGIZE_VALUE_OF(x) STRINGIZE(x)

int main(void)
{
    _scheduler_init();

    printf("Hello World!\n");
    uart0_flush();
    sleep_cpu();
    return 0;
}