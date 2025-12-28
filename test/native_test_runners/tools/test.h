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
#include <inttypes.h>
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

#include "sim_avr.h"
#include "sim_elf.h"
#include "sim_gdb.h"
#include "sim_hex.h"
#include "avr_uart.h"
#include "sim_core.h"

#include "debug.h"
#include "elf.h"


struct uart_receive_buffer
{
    char *buffer;
    unsigned int size;
    unsigned int index;
};

int validate_args(const int argc, char *argv[]);

/**
 * @brief finds matching bytes in avr's ram
 * @note skips reading registers data[0 - 32]   
 * @param  *array: 
 * @param  size: 
 * @retval Retuns the offset to first matching byte relative to avr->data
 *      or 0 if not found
 */
uint16_t find_offset_off(avr_t *avr, uint8_t *array, uint16_t size);

/**
 * @brief  
 * @note   
 * @param  *avr: 
 * @param  index: 
 * @retval 
 */
uint8_t read_ram(avr_t *avr, uint16_t index);

/**
 * @brief Initializes simavr for the given elf file  
 * @note   
 * @param  *elf_name: 
 * @param  *mcu:
 * @param  frequency:
 * @retval Returns a pointer to the initialized avr struct
 */
avr_t *init_avr(const char *elf_name,
    const char *mcu,
    const int frequency);

/**
 * @brief Reads unittest result from avr's ram
 * 
 * @return uint8_t 
 */
uint8_t unittest_result(avr_t *avr);

/**
 * @brief Enter gdb debug
 * @note   
 * @param  *avr: 
 * @param  port: gdb server port
 * @retval None
 */
void enter_gdb_debug(avr_t *avr, const int port);

/**
 * @brief Run avr until the specified cycles  
 * @note  Or avr crashes, or indicates Cpu_done
 * @param  *avr: 
 * @param  timeout_us: 
 * @param  timeout_fatal: 1 if reaching timeout is fatal  
 * @retval 1 if avr crashed or timeout reached 0 success 
 */
bool run_avr_cycles(avr_t *avr,
    avr_cycle_count_t cycles,
    const bool timeout_fatal);

/**
 * @brief Run avr until specified timeout (us)  
 * @note  Or avr crashes, or indicates Cpu_done
 * @param  *avr: 
 * @param  timeout_us: 
 * @param  timeout_fatal: 1 if reaching timeout is fatal  
 * @retval 1 if avr crashed or timeout reached 0 success 
 */
bool run_avr_us(avr_t *avr,
    const uint64_t timeout_us,
    bool timeout_fatal);

/**
 * @brief  Run avr until specified timeout (ms)
 * @note  Or avr crashes, or indicates Cpu_done
 * @param  *avr: 
 * @param  timeout_ms: 
 * @param  timeout_fatal: 1 if reaching timeout is fatal  
 * @retval 1 if avr crashes or timeout reached 0 success
 */
bool run_avr_ms(avr_t *avr,
    const unsigned long timeout_ms,
    bool timeout_fatal);

/**
 * @brief  Run avr until an interrupt is triggered
 * @note   
 * @param  *avr: 
 * @param  timeout_ms: max ms to run avr 
 * @retval 1 if avr crashes or timeout.
 */
bool run_avr_until_interrupt(avr_t *avr,
    const unsigned long timeout_ms,
    bool volatile *interrupt_state);

/**
 * @brief Callback function for receiving chars from simulated uart
 * 
 * @param irq 
 * @param value char value
 * @param param pointer to struct uart_receive_buffer
 */
void uart_receive_cb(struct avr_irq_t *irq, uint32_t value, void *param);

bool test_uart_receive(avr_t *avr,
    const char *expected,
    const unsigned long timeout_cycles);

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