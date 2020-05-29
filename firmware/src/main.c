#include <FreeRTOS.h>
#include <task.h>

#include <libopencm3/stm32/gpio.h>
#include <libopencm3/stm32/rcc.h>

static void myTask(void *args) {
    for (;;) {
        gpio_toggle(GPIOC, GPIO13);
        vTaskDelay(pdMS_TO_TICKS(1000));
    }
}

int main(void) {

    rcc_clock_setup_in_hse_8mhz_out_72mhz();

    rcc_periph_clock_enable(RCC_GPIOC);
    gpio_set_mode(GPIOC, GPIO_MODE_OUTPUT_2_MHZ, GPIO_CNF_OUTPUT_PUSHPULL,
                  GPIO13);

    xTaskCreate(myTask, "myTask", 100, NULL, configMAX_PRIORITIES - 1, NULL);
    vTaskStartScheduler();

    for (;;)
        ;
    return 0;
}
