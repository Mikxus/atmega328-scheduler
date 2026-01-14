#include "src/scheduler.h"
#include "src/task.h"
#include "src/drivers/timer/timer.h"

void task_0(void);
void task_1(void);
void task_2(void);

task_data_t task0, task1, task2;
uint8_t stack0[100];
uint8_t stack1[100];
uint8_t stack2[100];

int main(void)
{
    DDRB = (1 << DDB4) | (1 << DDB3); // Set PB4 and PB3 as output

    cli();
    init_scheduler();

    create_task(
        task0,
        stack0,
        sizeof(stack0),
        "task_0",
        1,
        1,
        &task_0);

    create_task(
        task1,
        stack1,
        sizeof(stack1),
        "task_1",
        1,
        1,
        &task_1);

    create_task(
        task2,
        stack2,
        sizeof(stack2),
        "task_2",
        1,
        1,
        &task_2);

    sei();
    start_scheduler();
}

void task_0(void) 
{
    printf("Tasks created succesfully\n");
    uint32_t last_time = get_ms();
    while(1){
        if (get_ms() - last_time >= 1000) {
            printf("Main loop alive: %lu ms, %lu ms\n", get_ms(), get_us() / 1000);
            printf("%s: exec time: %lu ms, overflows: %u stack: %hu bytes\n", task0.name,(unsigned long) task0.exec_time_us / 1000 , task0.exec_time_overflow_count, _get_task_stack_usage(&task0));
            printf("%s: exec time: %lu ms, overflows: %u stack: %hu bytes\n", task1.name, (unsigned long) task1.exec_time_us / 1000, task1.exec_time_overflow_count, _get_task_stack_usage(&task1));
            printf("%s: exec time: %lu ms, overflows: %u stack: %hu bytes\n", task2.name, (unsigned long) task2.exec_time_us / 1000, task2.exec_time_overflow_count, _get_task_stack_usage(&task2));
            printf("Total time: %lu ms\n", (task0.exec_time_us + task1.exec_time_us + task2.exec_time_us) / 1000);
            last_time = get_ms();
        }
    }
}

void task_1(void) 
{
    while(1) {
        PORTB |= (1 << PB4); // Toggle pin PB4
        PORTB &= ~(1 << PB4);
    }
}

void task_2(void) 
{
    while(1) {
        PORTB |= (1 << PB3); // Toggle pin PB3
        PORTB &= ~(1 << PB3);
    }
}