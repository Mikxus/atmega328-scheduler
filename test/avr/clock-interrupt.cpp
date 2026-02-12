#include <stdio.h>
#include <string.h>
#include <avr/sleep.h>
#include <avr/io.h>
#include <kernel/kernel.h>

int main(void)
{
    kernel_init();
    while(true){
        printf("uptime: %lu\n", get_ms());
    }
}