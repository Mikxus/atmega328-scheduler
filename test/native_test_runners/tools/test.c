#include "test.h"

int validate_args(const int argc, char *argv[])
{
    if (argc != 4)
    {
        printf("Usage: %s <.hex file> <mcu type> <frequency>\n", argv[0]);
        printf("num of args: %d\n", argc);
        return 1;
    }
    return 0;
}

avr_t *init_avr(const char *elf_name, const char *mcu, const int frequency)
{
    elf_firmware_t firmware = {0};

    firmware.frequency = frequency;

    if (sizeof(firmware.mmcu) <= strlen(mcu))
    {
        printf("MCU name too long\n");
        return NULL;
    }

    strcpy(firmware.mmcu, mcu);

    sim_setup_firmware(elf_name, 0, &firmware, "test");
    avr_t *avr = avr_make_mcu_by_name(firmware.mmcu);

    if (!avr)
    {
        return avr;
    }
    
    avr_init(avr);
    avr_load_firmware(avr, &firmware);
    return avr;
}
