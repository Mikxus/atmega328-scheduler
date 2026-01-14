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

/**
 * @brief Exit unittest & prepare result to simavr
 * @note   
 * @retval 
 */
__attribute__((noreturn)) void exit_unittest(void);

/**
 * @brief Test if expected equals actual
 * @note   
 * @param  expected: 
 * @param  actual: 
 * @param  message: 
 * @retval None
 */
void expect_equal(int expected, int actual, const char* message);

/**
 * @brief Test if statement is true
 * @note   
 * @param  cond: 0 fail, 1 pass
 * @param  message: 
 * @retval None
 */
void expect(int cond, const char* message);

#endif // _UNITTEST_H_
