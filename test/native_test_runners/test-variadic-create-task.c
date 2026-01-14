/* 
 * test-variadic-create-task.c
 * Test runner for variadic create_task function
 */
#include <stddef.h>
#include <stdio.h> 
#include <stdlib.h>

#include "tools/test.h"
#include "sim_elf.h"
#include "sim_avr.h"
#include "sim_core.h"

int main(int argc, char *argv[]) {

    avr_t *avr = NULL;

    if (validate_args(argc, argv))
    {
        return 1;
    }

    avr = init_avr(argv[1], argv[2], atoi(argv[3]));

    run_avr_ms(avr, 100, 1);

    return unittest_result(avr);
}