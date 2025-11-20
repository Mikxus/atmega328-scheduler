/*
 *  test-context-save.c
 *  Test if saving CPU context on task switch works correctly
 */
#include "tools/test.h"
#include "tools/cpu.h"

#include "sim_elf.h"
#include "sim_avr.h"

/**
 * @brief Register values to check register reading
 */
uint8_t register_values_before[32] = {0};
uint8_t register_values_after[32] = {0};

int main(int argc, char *argv[])
{
    avr_t *avr = NULL;
    int state = 0;
    
    if (validate_args(argc, argv)){
        return 1;
    }

    avr = init_avr(argv[1], argv[2], atoi(argv[3]));
    run_avr_ms(avr, 2000);

    for (int i = 0; i < 31; i++)
    {
        if (register_values_before[i] != read_register(avr, i))
        {
            ERROR("cpu register:%2d value is incorrect: 0x%x, should be 0x%x ",
                 i,
                 read_register(avr, i),
                 register_values_before[i]);
            state = 1;
        }
    }
    return state;
}