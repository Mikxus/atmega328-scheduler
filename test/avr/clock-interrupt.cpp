#include <stdio.h>
#include <avr/io.h>
#include <avr/sleep.h>
#include <kernel/kernel.h>
#include <kernel/drivers/clock.h>

int main(void)
{
    kernel_init();
    while(true){
        printf("uptime: %lu\n", get_ms());
    }
}