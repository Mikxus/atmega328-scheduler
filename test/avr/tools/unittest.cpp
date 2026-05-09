#include "unittest.h"

uint8_t *unittest_exit_result = (uint8_t *) RAMEND;
uint8_t unittest_state = 0;

void init_unittest(void)
{
    // Set fail state, in case avr crashes before exit_unittest is called
    ATOMIC_GUARD() {
        *unittest_exit_result = 1;
    }
}

__attribute__((noreturn)) void exit_unittest(void)
{
    ATOMIC_GUARD() {
        *unittest_exit_result = unittest_state;
    }
    
    cli();
    sleep_cpu();
    while (1);
}

void expect_equal(int expected, int actual, const char* message)
{
    if (expected != actual) {
        printf("FAIL: %s. Expected %d, got %d\n", message, expected, actual);
        ATOMIC_GUARD() {
            unittest_state++;
        }
    } else {
        printf("PASS: %s\n", message);
    }
    uart0_flush();
    return;
}

void expect(int cond, const char* message)
{
    if (cond == 0) {
        printf("FAIL: %s\n", message);
        ATOMIC_GUARD() {
            unittest_state++;
        }
    } else {
        printf("PASS: %s\n", message);
    }
    uart0_flush();
    return;
}

void fail_test(const char* message)
{
    ATOMIC_GUARD() {
        unittest_state++;
    }
    
    if (message == nullptr) 
        return;

    printf("FAIL: %s\n", message);
    uart0_flush();
    return;
}