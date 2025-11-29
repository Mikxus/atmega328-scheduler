/*
 *  test-cpu-register-read.c
 *  Test reading registers
 */
#include "tools/test.h"
#include "tools/cpu.h"

#include "sim_elf.h"
#include "sim_avr.h"

/**
 * @brief Register values to check register reading
 * @note   
 */
const uint8_t register_values[32] =
{
    0XFF, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10,
    11, 12, 13, 14, 15, 16, 17 ,18, 19, 20,
    21, 22, 23, 24, 25 ,26, 27 ,28, 29, 30, 31
};

int main(int argc, char *argv[])
{
    avr_t *avr = NULL;
    int state = 0;
    
    if (validate_args(argc, argv)){
        return 1;
    }

    avr = init_avr(argv[1], argv[2], atoi(argv[3]));
    run_avr_ms(avr, 2000, 1);

    for (int i = 0; i < 32; i++)
    {
        if (register_values[i] != read_register(avr, i))
        {
            ERROR("cpu register:%2d value is incorrect: 0x%x, should be 0x%x ",
                 i,
                 read_register(avr, i),
                 register_values[i]);
            state = 1;
        }
    }
    return state;
}