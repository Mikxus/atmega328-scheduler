#include <kernel/drivers/uart.h>

void initialize_uart(void)
{
    uint8_t sreg = SREG;
    cli();

    #ifdef USART0_U2X_SPEED
    uart_init(DOUBLE_BAUD_CALC(CONF_UART_BAUD_RATE));
    #else
    uart_init(baud_calc(CONF_UART_BAUD_RATE));
    #endif

    #ifdef USART0_U2X_SPEED
    uart_set_U2X();
    #endif

    /* Attach UART to stdin and stdout */
    stdout = &uart0_io;
    stdin = &uart0_io;

    SREG = sreg;
    return;
}