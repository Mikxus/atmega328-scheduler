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

uint16_t find_offset_off(avr_t *avr, uint8_t *array, uint16_t size)
{
    uint16_t count = 0;

    if (avr == NULL) return 0;
    INFO("avr-ioffset: %u", avr->io_offset);
    for (uint16_t i = avr->io_offset - 1; i < avr->ramend; i++)
    {
        if (avr->data[i] != array[count]) {
            count = 0;
        }

        count += 1;
        if (count == size)
            return i - size + 1;
    }
    WARN("find_offset_off: magic value not found");
    return 0;
}

uint8_t read_ram(avr_t *avr, uint16_t index)
{
    if (index > avr->ramend)
    {
        WARN("Outof bounds avr data read: index %u, ramend: %u", index, avr->ramend);
        return 0;
    }
    return avr->data[index];
}

avr_t *init_avr(const char *elf_name,
    const char *mcu,
    const int frequency)
{
    elf_firmware_t firmware = {};

    firmware.frequency = frequency;
    if (sizeof(firmware.mmcu) <= strlen(mcu))
    {
        ERROR("MCU name too long");
        return NULL;
    }

    strcpy(firmware.mmcu, mcu);

    sim_setup_firmware(elf_name, 0, &firmware, "test");

    if (!firmware.flash)
    {
        ERROR("Failed to load firmware");
        return NULL;
    }

    avr_t *avr = avr_make_mcu_by_name(firmware.mmcu);

    if (!avr)
    {
        ERROR("AVR '%s' not known", firmware.mmcu);
        return avr;
    }
    
    avr_init(avr);
    avr_load_firmware(avr, &firmware);
    return avr;
}

void enter_gdb_debug(avr_t *avr, const int port)
{
    avr->state = cpu_Stopped;
    avr->gdb_port = port;
    avr_gdb_init(avr);

    for (;;) {
        int state = avr_run(avr);
        if (state == cpu_Done || state == cpu_Crashed)
            break;
    } 
}

void uart_receive_cb(struct avr_irq_t *irq, uint32_t value, void *param)
{
    struct uart_receive_buffer *buf = param;

    if (!buf)
    {
        ERROR("Error failed to get buffer");
        return;
    }

    if (buf->size - 1 == buf->index) return;

    buf->buffer[buf->index] = (char) value;
    buf->index += 1;
    return;
}

bool run_avr_cycles(avr_t *avr,
    avr_cycle_count_t cycles,
    const bool timeout_fatal)
{
    cycles += avr->cycle;

    while (avr->cycle < cycles)
    {
        avr_run(avr);
        if (avr->state == cpu_Stopped || avr->state == cpu_Crashed)
        {
            ERROR("Avr failed\r\n");
            dump_avr_core(avr);
            return 1;
        }

        if (avr->state == cpu_Done) return 0;
    }

    if (timeout_fatal) {
        ERROR("avr timeout reached: %llu", cycles);
        dump_avr_core(avr);
        return 1;
    }

    return 0;
}

bool run_avr_us(avr_t *avr,
    const uint64_t timeout_us,
    const bool timeout_fatal)
{
    avr_cycle_count_t timeout_cycles = (avr->frequency / 1000000) * timeout_us;
    return run_avr_cycles(avr, timeout_cycles, timeout_fatal);
}

bool run_avr_ms(avr_t *avr,
    const unsigned long timeout_ms,
    const bool timeout_fatal)
{
    return run_avr_us(avr, timeout_ms * 1000, timeout_fatal);
}

bool run_avr_until_interrupt(avr_t *avr,
    const unsigned long timeout_ms,
    volatile bool *interrupt_state)
{
    avr_cycle_count_t timeout = avr->cycle + (avr->frequency / 1000) * timeout_ms; 

    while (avr->cycle < timeout)
    {
        avr_run(avr);

        if (*interrupt_state) return 0;

        if (avr->state == cpu_Stopped || avr->state == cpu_Crashed ||avr->state == cpu_Done )
        {
            ERROR("Avr failed\r\n");
            dump_avr_core(avr);
            return 1;
        }
    }

    ERROR("avr timeout reached: %lu", timeout_ms);
    dump_avr_core(avr);
    return 1;
}

bool test_uart_receive(avr_t *avr,
    const char *expected,
    const unsigned long timeout_ms)
{
    struct uart_receive_buffer buffer;
    buffer.buffer = calloc(sizeof(char), 2048);
    buffer.index = 0;

    if (!buffer.buffer)
    {
        ERROR("Failed to allocate uart receive buffer of %lu bytes", sizeof(char) * 2048);
        return 1;
    } 

    if (!avr)
    {
        ERROR("AVR not initialized");
        free(buffer.buffer);
        return 1;
    }

    avr_irq_register_notify(
        avr_io_getirq(avr, AVR_IOCTL_UART_GETIRQ('0'), UART_IRQ_OUTPUT),
        uart_receive_cb,
        &buffer
    );

    run_avr_ms(avr, timeout_ms, 1);

    buffer.buffer[buffer.index + 1] = '\0';
    if (strcmp(buffer.buffer, expected) != 0)
    {
        ERROR("Expected: lenght %d\n'%s' \ngot: lenght %d\n'%s'",
            strlen(expected),
            expected,
            strlen(buffer.buffer),
            buffer.buffer
        );
        free(buffer.buffer);
        return 1;
    }
    free(buffer.buffer);
    return 0;
}

void hex_dump(const uint8_t *data, const uint64_t data_len,
     const uint8_t width, const char *description)
{
    uint64_t index = 0;

    if (!data)
    {
        ERROR("Data is NULL\n");
        return;
    }

    INFO("Dumping %s", description);
    for (uint64_t row = 1 + (data_len / width); row != 0; row--)
    {
        uint64_t offset = 0;

        // print address
        printf("%04x: ", index);
        
        // print data
        for (uint64_t i = 0; i < width; i++)
        {
            printf("%02x ", data[index + offset]);

            if (index + offset == data_len) break;
            offset += 1;
        }

        offset = 0;

        // print ascii representation
        for (uint64_t i = 0; i < width; i++)
        {

            if (data[index + offset] >= 0x21 && data[index + offset] <= 0x7e)
            {
                printf("%c", data[index + offset]);
            }
            else
            {
                printf(".");
            }

            if (index + offset == data_len) break;
            offset += 1;
        }
        printf("\r\n");
        index += offset;
    }

    INFO("End of %s", description);
}

void dump_avr_core(avr_t *avr)
{
    if (!avr) return;
    INFO("Dumping AVR core");
    INFO("mcu: %s", avr->mmcu);
    INFO("frequency: %lu", (unsigned long) avr->frequency);
    INFO("cycle: %lu", (unsigned long) avr->cycle);
    INFO("pc: %#04x", (unsigned long) avr->pc);
    hex_dump(avr->data, avr->ramend, 20, "ram");
}