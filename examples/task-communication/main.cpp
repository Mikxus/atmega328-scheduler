#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <avr/pgmspace.h>

#include <kernel/kernel.h>
#include <kernel/drivers/uart.h>
#include <kernel/drivers/gpio.h>
#include <kernel/drivers/clock.h>
#include <kernel/ipc/msg_que.h>

#define PIN_1 PB4
#define PIN_2 PB3
#define FIFO_SIZE 2
#define BLINK_STACK_SIZE 100

/* Control task defines*/
#define INPUT_BUFF_SIZE 25
#define CONTROL_STACK_SIZE 200
#define RECV_TIMEOUT_MS 20

struct blink_msg {
    uint8_t count       = 0;
    uint8_t blink_freq  = 0;
};

struct blink_msg task_1_buffer[FIFO_SIZE];
struct blink_msg task_2_buffer[FIFO_SIZE];

msg_que_t<struct blink_msg> msg1;
msg_que_t<struct blink_msg> msg2;

task_data_t blink1;
task_data_t blink2;
task_data_t control;

uint8_t blink_stack1[BLINK_STACK_SIZE];
uint8_t blink_stack2[BLINK_STACK_SIZE];
uint8_t control_stack[CONTROL_STACK_SIZE];

void blink_task(msg_que_t<struct blink_msg> *msg_que, io_port port, uint8_t pin)
{
    blink_msg message;
    uint32_t blink_ms       = 0; 
    uint32_t last_blink_ms  = 0;
    set_gpio_mode(port, pin, OUTPUT);

    while (1) {
        msg_que->dequeue(message);

        printf_P(PSTR("%S: blinking %d times at %d freq\n"),
            get_current_task()->name, message.count, message.blink_freq);

        blink_ms = 1000 / (message.blink_freq * 2) ;
        message.count *= 2;

        while (message.count != 0) {
            if (get_ms() - last_blink_ms > blink_ms) {
                write_gpio(port, pin, read_gpio(port, pin) ^ 1);
                last_blink_ms = get_ms();
                message.count -= 1;
            }
        }
    }
}

void print_help()
{
    printf_P(PSTR("Usage\n\
blink (1 | 2) (count) (frequency) | blink 1 or 2 led count times at frequency\n\
stats                             | Show scheduler stats\n\
help                              | Show help message\n"));
    return;
}

void print_cmd()
{
    task_data_t* task_arr[] = {&blink1, &blink2, &control};

    printf_P(PSTR("\n%-10s| %-5s| %3s| %-9s| %-10s\n"), 
        "Name", "State", "Pri", "Stack", "Exec (ms)");
    printf_P(PSTR("----------------------------------------------\n"));

    for (uint8_t i = 0; i < 3; i++)
    {
        char state_char;
        switch (task_arr[i]->state) {
            case READY: state_char = 'R'; break;
            case RUNNING: state_char = 'E'; break;
            case BLOCKED: state_char = 'B'; break;
            case STOPPED: state_char = 'S'; break;
            case SLEEP: state_char = 'Z'; break;
            default: state_char = 'U'; break;
        }

        printf_P(PSTR("%-10S| %-5c| %3d| %3d /%4d| %10lu\n"),
            task_arr[i]->name,
            state_char,
            task_arr[i]->priority,
            get_task_stack_usage(task_arr[i]),
            get_task_stack_size(task_arr[i]),
            task_arr[i]->exec_time_us / 1000);
    }
    printf_P(PSTR("\n"));
}

void blink_cmd(char *arr)
{
    struct blink_msg msg;

    char *token = strtok_P(arr, PSTR(" "));
    if (token == NULL) return;

    token = strtok_P(NULL, PSTR(" "));
    if (token == NULL) return;
    uint8_t id = atoi(token);

    token = strtok_P(NULL, PSTR(" "));
    if (token == NULL) return;
    uint8_t count = atoi(token);

    token = strtok_P(NULL, PSTR(" "));
    if (token == NULL) return;
    uint16_t freq = atoi(token);

    msg.count = count;
    msg.blink_freq = freq;

    if (id == 1) {
        if (!msg1.is_full())
            msg1.enqueue(msg);
    } else if (id == 2) {
        if (!msg2.is_full())
            msg2.enqueue(msg);
    } else {
        printf_P(PSTR("Invalid ID\n"));
    }

    printf_P(PSTR("Sending command: blink ID: %u COUNT: %u FREQ: %u\n"),
        id, count, freq);
}

void control_task(void)
{
    char command[INPUT_BUFF_SIZE] = {0};
    uint32_t last_recv_ms = 0;
    uint8_t index = 0;

    while (1) 
    {
        if (uart_AvailableBytes() == 0) {
            continue;
        }

        index = 0;
        last_recv_ms = get_ms();
        while (get_ms() - last_recv_ms < RECV_TIMEOUT_MS) {
            if (uart_AvailableBytes() == 0)
                continue;

            last_recv_ms = get_ms();

            if (index >= INPUT_BUFF_SIZE - 2) {
                break;
            }

            char c = uart_getc();

            if (c != '\n' && c != '\r') {
                command[index] = c;
                index += 1;
            }
        }

        command[index] = '\0';
        if (strcmp_P(command, PSTR("stats")) == 0) {
            print_cmd();
            continue;
        }

        if (strncmp_P(command, PSTR("blink"), 5) == 0) {
            blink_cmd(command);
            continue;
        }

        print_help();
    }
}

int main(void)
{
    kernel_init();

    msg1.init(task_1_buffer, FIFO_SIZE);
    msg2.init(task_2_buffer, FIFO_SIZE);

    create_task(blink1, blink_stack1, BLINK_STACK_SIZE,
        PSTR("blink 1"), 1, 1, blink_task, &msg1, IO_PORTB, (uint8_t) PIN_1);

    create_task(blink2, blink_stack2, BLINK_STACK_SIZE,
        PSTR("blink 2"), 1, 1, blink_task, &msg2, IO_PORTB, (uint8_t) PIN_2);
    
    create_task(control, control_stack, CONTROL_STACK_SIZE,
        PSTR("control"), 1, 1, control_task);
   
    sei();

    printf_P(PSTR("starting\n"));
    uart0_flush();
    kernel_start();
    while(1){};
}