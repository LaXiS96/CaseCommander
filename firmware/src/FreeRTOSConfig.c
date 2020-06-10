#include <stdio.h>

#include <FreeRTOS.h>
#include <task.h>

#include <trace.h>

void vAssertCalled(const char *pcFile, unsigned long ulLine)
{
    char buf[256];

    snprintf(buf, sizeof(buf), "vAssertCalled at %s on line %lu", pcFile, ulLine);
    tracePrintLine(buf);

    taskDISABLE_INTERRUPTS();
    for (;;)
        ;
}

void vApplicationStackOverflowHook(TaskHandle_t xTask, char *pcTaskName)
{
    (void)xTask;

    char buf[256];

    snprintf(buf, sizeof(buf), "vApplicationStackOverflowHook task %s", pcTaskName);
    tracePrintLine(buf);

    taskDISABLE_INTERRUPTS();
    for (;;)
        ;
}
