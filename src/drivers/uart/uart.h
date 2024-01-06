#ifndef _UART_H_
#define _UART_H_

#include <stdio.h>
#include <avr/io.h>
#include <avr/interrupt.h>
#include "lib/AVR-UART-LIB/usart.h"

#ifndef SCHEDULER_UART_BAUD_RATE
    #error "SCHEDULER_UART_BAUD_RATE not defined"
#endif

/**
 * @brief Initializes uart as a stdin & stdout output  
 * @note   
 * @retval None
 */
void initialize_uart();


#endif