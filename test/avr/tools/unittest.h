#ifndef _UNITTEST_H_
#define _UNITTEST_H_

#include <stdio.h>
#include <stdint.h>
#include <avr/io.h>
#include <avr/sleep.h>

#include "src/drivers/uart/uart.h"

extern uint8_t *unittest_exit_result;
extern uint8_t unittest_state;

/**
 * @brief initialize unittest return value
 * 
 */
void init_unittest(void);
__attribute__((noreturn)) void exit_unittest(void);

void expect_equal(int expected, int actual, const char* message);

#endif // _UNITTEST_H_
