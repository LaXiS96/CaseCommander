#include <libopencm3/stm32/gpio.h>
#include <libopencm3/stm32/rcc.h>

#include <FreeRTOS.h>
#include <task.h>

#include "commander.h"
#include "tacho.h"
#include "usb.h"
#include <trace.h>

static void blinkTask(void *arg)
{
    (void)arg;

    for (;;) {
        gpio_toggle(GPIOC, GPIO13);
        vTaskDelay(pdMS_TO_TICKS(500));
    }
}

static void readTachoTask(void *arg)
{
    (void)arg;

    for (;;) {
        // if (tachoValues.ch3.rpm == 0) {
        //     usbWriteString("zero here\n");
        // }
        usbWriteString(cc_sprintf("debug before1: %d\n", debugStatus));
        debugStatus = 0;
        usbWriteString(cc_sprintf("debug before2: %d\n", debugStatus));
        usbWriteString(cc_sprintf(
            "\n1:%d,%d\n2:%d,%d\n3:%d,%d\n4:%d,%d\n",
            tachoValues.ch1.rpm,
            tachoValues.ch1.noSignal,
            tachoValues.ch2.rpm,
            tachoValues.ch2.noSignal,
            tachoValues.ch3.rpm,
            tachoValues.ch3.noSignal,
            tachoValues.ch4.rpm,
            tachoValues.ch4.noSignal));

        usbWriteString(cc_sprintf("debug after  : %d\n", debugStatus));

        vTaskDelay(pdMS_TO_TICKS(2000));
    }
}

static void testTask(void *arg)
{
    (void)arg;

    char buf[128];
    buf[sizeof(buf) - 1] = '\0';
}

int main(void)
{
    rcc_clock_setup_in_hse_8mhz_out_72mhz();

    traceInit();

    xTaskCreate(testTask, "test", 100, NULL, configMAX_PRIORITIES - 1, NULL);
    vTaskStartScheduler();

    for (;;)
        ;

    rcc_periph_clock_enable(RCC_GPIOC);
    gpio_set_mode(GPIOC, GPIO_MODE_OUTPUT_2_MHZ, GPIO_CNF_OUTPUT_PUSHPULL, GPIO13);

    usbReenumerate();
    usbInit();
    commanderInit();
    tachoInit();

    xTaskCreate(blinkTask, "blinkTask", 100, NULL, configMAX_PRIORITIES - 1, NULL);

    // TODO implement hardfault handler and freertos stack overflow checking
    xTaskCreate(readTachoTask, "readTachoTask", 1024, NULL, configMAX_PRIORITIES - 1, NULL);

    vTaskStartScheduler();

    for (;;)
        ;
    return 0;
}

void hard_fault_handler(void)
{
    // TODO write better HardFault handling
    tracePrintLine("hard_fault_handler");
    for (;;)
        ;
}

void mem_manage_handler(void)
{
    tracePrintLine("mem_manage_handler");
    for (;;)
        ;
}

void bus_fault_handler(void)
{
    tracePrintLine("bus_fault_handler");
    for (;;)
        ;
}

void usage_fault_handler(void)
{
    tracePrintLine("usage_fault_handler");
    for (;;)
        ;
}
