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
        ERROR("MCU name too long");
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

bool run_avr_ms(avr_t *avr, const unsigned long timeout_ms)
{
    avr_cycle_count_t timeout = (avr->frequency / 1000) * timeout_ms; 

    while (avr->cycle < timeout)
    {
        avr_run(avr);

        if (avr->state == cpu_Stopped || avr->state == cpu_Crashed)
        {
            ERROR("Avr failed\r\n");
            return 1;
        }

        if (avr->state == cpu_Done) break;
    }
    return 0;
}

bool test_uart_receive(avr_t *avr, const char *expected, const unsigned long timeout_ms)
{
    struct uart_receive_buffer buffer;

    buffer.buffer = calloc(sizeof(char), 2048);
    buffer.size = sizeof(char) * 2048;
    buffer.index = 0;
    if (!buffer.buffer)
    {
        ERROR("Failed to allocate uart receive buffer of %d bytes", sizeof(char) * 2048);
        return 1;
    } 

    if (!avr)
    {
        ERROR("AVR not initialized");
        return 1;
    }

    avr_irq_register_notify(
        avr_io_getirq(avr, AVR_IOCTL_UART_GETIRQ('0'), UART_IRQ_OUTPUT),
        uart_receive_cb,
        &buffer
    );

    run_avr_ms(avr, timeout_ms);

    buffer.buffer[buffer.index + 1] = '\0';
    if (strcmp(buffer.buffer, expected) != 0)
    {
        ERROR("Expected: lenght %d\n'%s' \ngot: lenght %d\n'%s'",
            strlen(expected),
            expected,
            strlen(buffer.buffer),
            buffer.buffer
        );
        return 1;
    }
    return 0;
}