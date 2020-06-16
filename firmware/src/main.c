#include <libopencm3/cm3/scb.h>
#include <libopencm3/stm32/gpio.h>
#include <libopencm3/stm32/rcc.h>

#include <FreeRTOS.h>
#include <task.h>

#include <commander.h>
#include <led.h>
#include <tacho.h>
#include <trace.h>
#include <usb.h>

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
        tachoValues_t values = tachoGetValues();
        printf(
            "\n1:%d %d\n2:%d %d\n3:%d %d\n4:%d %d\n",
            values.ch1.rpm,
            values.ch1.noSignal,
            values.ch2.rpm,
            values.ch2.noSignal,
            values.ch3.rpm,
            values.ch3.noSignal,
            values.ch4.rpm,
            values.ch4.noSignal);

        vTaskDelay(pdMS_TO_TICKS(1000));
    }
}

static void testTask(void *arg)
{
    (void)arg;

    static char buf[] =
        "012345678901234567890123456789012345678901234567890123456789012\n";

    for (;;) {
        usbWrite(buf, sizeof(buf) - 1);
        vTaskDelay(pdMS_TO_TICKS(3000));
    }
}

int main(void)
{
    rcc_clock_setup_in_hse_8mhz_out_72mhz();

    traceInit();

    // rcc_periph_clock_enable(RCC_GPIOC);
    // gpio_set_mode(GPIOC, GPIO_MODE_OUTPUT_2_MHZ, GPIO_CNF_OUTPUT_PUSHPULL, GPIO13);

    usbReenumerate();
    usbInit();
    // commanderInit();
    tachoInit();
    // ledInit();

    // xTaskCreate(
    //     blinkTask, "blinkTask", configMINIMAL_STACK_SIZE, NULL, configMAX_PRIORITIES - 1, NULL);

    // xTaskCreate(
    //     readTachoTask,
    //     "readTachoTask",
    //     configMINIMAL_STACK_SIZE,
    //     NULL,
    //     configMAX_PRIORITIES - 1,
    //     NULL);

    xTaskCreate(testTask, "test", configMINIMAL_STACK_SIZE, NULL, configMAX_PRIORITIES - 1, NULL);

    vTaskStartScheduler();

    for (;;)
        ;
    return 0;
}
