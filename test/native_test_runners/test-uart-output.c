/* 
 * test-uart-output.cpp
 * This file is used to test the uart output
 * 
 */
#include <stddef.h>
#include <stdio.h> 
#include <stdlib.h>

#include "tools/test.h"

#include "sim_elf.h"
#include "sim_avr.h"
#include "sim_core.h"
#include "sim_hex.h"

int main(int argc, char *argv[]) {

    avr_t *avr = NULL;

    if (validate_args(argc, argv))
    {
        return 1;
    }

    avr = init_avr(argv[1], argv[2], atoi(argv[3]));

    if (!avr)
    {
        return 1;
    }

    for (;;)
    {
        int state = avr_run(avr);
        if (state == cpu_Done || state == cpu_Crashed)
        {
            break;
        }
    }

    return 0;
}