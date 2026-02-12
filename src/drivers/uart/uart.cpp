#include <kernel/drivers/uart/uart.h>

void initialize_uart(void)
{
    uint8_t sreg = SREG;
    cli();
    uart_init(BAUD_CALC(CONF_UART_BAUD_RATE));

    #ifdef USART0_U2X_SPEED
    uart_set_U2X();
    #endif

    /* Attach UART to stdin and stdout */
    stdout = &uart0_io;
    stdin = &uart0_io;

    SREG = sreg;
    return;
}