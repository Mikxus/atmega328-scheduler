/* 
 * test-ram-read.c
 * tests reading from avr's sram
 */
#include <stddef.h>
#include <stdio.h> 
#include <stdlib.h>
#include <string.h>

#include "tools/test.h"
#include "sim_elf.h"
#include "sim_avr.h"
#include "sim_core.h"
#include "sim_hex.h"

const uint8_t identifier[] = {0xDE, 0xAD, 0xBE, 0xEF};

int main(int argc, char *argv[]) {

    avr_t *avr = NULL;
    uint8_t ram_start_buffer [sizeof(identifier)];
    uint8_t ram_end_buffer [sizeof(identifier)];

    if (validate_args(argc, argv))
    {
        return 1;
    }

    avr = init_avr(argv[1], argv[2], atoi(argv[3]));

    run_avr_ms(avr, 2, 1);

    memcpy(&ram_start_buffer[0], avr->data, sizeof(identifier));
    memcpy(&ram_end_buffer[0], avr->data + avr->ramend - sizeof(identifier), sizeof(identifier));

    if (memcmp(identifier, ram_start_buffer, sizeof(identifier)) != 0)
    {
        ERROR("Failed to read from start of sram");
        ERROR("Received: %04x insted of %04x", ram_start_buffer, identifier);
        dump_avr_core(avr);
        return 1;
    }

    if (memcmp(identifier, ram_end_buffer, sizeof(identifier)) != 0)
    {
        ERROR("Failed to read from end of sram");
        ERROR("Received: %04x insted of %04x", ram_end_buffer, identifier);
        return 1;
    }
    return 0;
}