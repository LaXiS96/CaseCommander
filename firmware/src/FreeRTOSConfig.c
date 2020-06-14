#include <stdio.h>

#include <FreeRTOS.h>
#include <task.h>

void vApplicationStackOverflowHook(TaskHandle_t xTask, char *pcTaskName);
void vApplicationMallocFailedHook(void);

void vAssertCalled(const char *pcFile, unsigned long ulLine)
{
    printf("vAssertCalled at %s on line %lu", pcFile, ulLine);

    taskDISABLE_INTERRUPTS();

    __asm__("bkpt #0");

    for (;;)
        ;
}

void vApplicationStackOverflowHook(TaskHandle_t xTask, char *pcTaskName)
{
    (void)xTask;

    printf("vApplicationStackOverflowHook task %s", pcTaskName);

    taskDISABLE_INTERRUPTS();

    __asm__("bkpt #0");

    for (;;)
        ;
}

void vApplicationMallocFailedHook(void)
{
    printf("vApplicationMallocFailedHook");

    taskDISABLE_INTERRUPTS();

    __asm__("bkpt #0");

    for (;;)
        ;
}
