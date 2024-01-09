#include "uart.h"

void initialize_uart(void)
{
    // Init code here
    uart_init(BAUD_CALC(SCHEDULER_UART_BAUD_RATE));

    /* Attach UART to stdin and stdout */
    stdout = &uart0_io;
    stdin = &uart0_io;

    sei();
    return;
}