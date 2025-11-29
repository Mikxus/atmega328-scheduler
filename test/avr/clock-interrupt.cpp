#include <stdio.h>
#include <string.h>
#include <avr/sleep.h>
#include <avr/io.h>
#include "src/scheduler.h"

int main(void)
{
    _scheduler_init();
    while(true){
        printf("uptime: %lu\n", get_ms());
    }
}