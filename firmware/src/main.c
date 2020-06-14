#include <libopencm3/cm3/scb.h>
#include <libopencm3/stm32/gpio.h>
#include <libopencm3/stm32/rcc.h>

#include <FreeRTOS.h>
#include <task.h>

#include "commander.h"
#include "tacho.h"
#include "usb.h"
#include <led.h>
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
    // volatile char test[4];
    // test[0] = '1';
    // test[1] = '2';
    // test[2] = '3';
    // test[3] = '4';

    // volatile uint32_t test = 1 / 0;

    // volatile char buf[500];
    // for (uint16_t i = 0; i < sizeof(buf) - 1; i++)
    //     buf[i] = 'A';

    // buf[sizeof(buf) - 1] = '\0';

    // tracePrintLine(buf);

    for (;;)
        ;
}

int main(void)
{
    rcc_clock_setup_in_hse_8mhz_out_72mhz();

    traceInit();

    // rcc_periph_clock_enable(RCC_GPIOC);
    // gpio_set_mode(GPIOC, GPIO_MODE_OUTPUT_2_MHZ, GPIO_CNF_OUTPUT_PUSHPULL, GPIO13);

    // usbReenumerate();
    // usbInit();
    // commanderInit();
    // tachoInit();
    ledInit();

    // xTaskCreate(
    //     blinkTask, "blinkTask", configMINIMAL_STACK_SIZE, NULL, configMAX_PRIORITIES - 1, NULL);

    // xTaskCreate(readTachoTask, "readTachoTask", 1024, NULL, configMAX_PRIORITIES - 1, NULL);

    // vTaskStartScheduler();

    for (;;)
        ;
    return 0;
}
