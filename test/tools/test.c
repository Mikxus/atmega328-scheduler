#include "test.h"

avr_t *init_avr(const char *elf_name)
{
    elf_firmware_t firmware = {0};

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
