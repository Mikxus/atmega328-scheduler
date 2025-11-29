#include "uart.h"

void initialize_uart(void)
{
    uint8_t sreg = SREG;
    cli();
    uart_init(BAUD_CALC(SCHEDULER_UART_BAUD_RATE));

    /* Attach UART to stdin and stdout */
    stdout = &uart0_io;
    stdin = &uart0_io;

    SREG = sreg;
    return;
}