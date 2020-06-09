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

int main(void)
{
    rcc_clock_setup_in_hse_8mhz_out_72mhz();

    traceInit();

    for (;;) {
        // TODO convert into tracePutChar and make up something sensible
        ITM_STIM8(0) = 'A';

        for (int i = 0; i < 20000000; i++)
            __asm__("nop");
    }

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
