/*
 *
 */
#ifndef TEST_H
#define TEST_H

#ifdef __cplusplus
extern "C" {
#endif

#include <stdio.h> 
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>

/* simavr build config */
#include "sim_core_config.h"

/*
 * simavr's makefile.common defines this
 *
 */
#ifndef CONFIG_PULL_UPS
#define CONFIG_PULL_UPS 1
#endif

#include "sim_elf.h"
#include "sim_avr.h"
#include "sim_core.h"
#include "sim_hex.h"
#include "avr_uart.h"

#include "debug.h"

struct uart_receive_buffer
{
    char *buffer;
    unsigned int size;
    unsigned int index;
};

int validate_args(const int argc, char *argv[]);

/**
 * @brief Initializes simavr for the given elf file  
 * @note   
 * @param  *elf_name: 
 * @param  *mcu:
 * @param  frequency:
 * @retval Returns a pointer to the initialized avr struct
 */
avr_t *init_avr(const char *elf_name, const char *mcu, const int frequency);


/**
 * @brief  Run avr until specified timeout
 * @note   
 * @param  *avr: 
 * @param  timeout_ms: 
 * @retval 1 if avr crashes or timeout reached 0 success
 */
bool run_avr_ms(avr_t *avr, const unsigned long timeout_ms);

/**
 * @brief  Run avr until an interrupt is triggered
 * @note   
 * @param  *avr: 
 * @param  timeout_ms: max ms to run avr 
 * @retval 1 if avr crashes or timeout.
 */
bool run_avr_until_interrupt(avr_t *avr, const unsigned long timeout_ms, bool volatile *interrupt_state);

/**
 * @brief Callback function for receiving chars from simulated uart
 * 
 * @param irq 
 * @param value char value
 * @param param pointer to struct uart_receive_buffer
 */
void uart_receive_cb(struct avr_irq_t *irq, uint32_t value, void *param);

bool test_uart_receive(avr_t *avr, const char *expected, const unsigned long timeout_cycles);

/**
 * @brief Create hex dump from address  
 * @note   
 * @param  *data: 
 * @param  data_len: 
 * @param  width: 
 * @param  *description: 
 * @retval None
 */
void hex_dump(const uint8_t *data, const uint64_t data_len,
     const uint8_t width, const char *description);
/**
 * Prints out a dump of the avr's cpu state
 */
void dump_avr_core(avr_t *avr);

#ifdef __cplusplus
}
#endif

#endif