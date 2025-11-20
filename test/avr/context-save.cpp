#include <stdio.h>
#include <avr/sleep.h>
#include <avr/io.h>
#include "src/scheduler.h"

int main(void)
{
    _scheduler_init();


    cli();
    sleep_cpu();
}