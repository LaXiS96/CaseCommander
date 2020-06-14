#include <led.h>

void ledInit(void)
{
    rcc_periph_clock_enable(RCC_GPIOA);
    gpio_set_mode(
        GPIOA,
        GPIO_MODE_OUTPUT_50_MHZ,
        GPIO_CNF_OUTPUT_ALTFN_PUSHPULL,
        GPIO_SPI1_MOSI | GPIO_SPI1_SCK);

    rcc_periph_clock_enable(RCC_SPI1);
    rcc_periph_reset_pulse(RST_SPI1);

    spi_enable_software_slave_management(SPI1);
    spi_set_nss_high(SPI1);
    spi_set_master_mode(SPI1);
    spi_set_baudrate_prescaler(SPI1, SPI_CR1_BR_FPCLK_DIV_256);
    spi_set_clock_polarity_1(SPI1);
    spi_set_clock_phase_1(SPI1);
    spi_enable(SPI1);

    uint32_t i;

    // Start frame
    for (i = 0; i < 4; i++)
        spi_send(SPI1, 0);

    for (i = 0; i < 58; i++) {
        spi_send(SPI1, 0b11100000 | 1);         // Global brightness (5bit)
        spi_send(SPI1, (i % 3 == 0) ? 255 : 0); // Blue
        spi_send(SPI1, (i % 3 == 1) ? 255 : 0); // Green
        spi_send(SPI1, (i % 3 == 2) ? 255 : 0); // Red
    }

    // End frame
    for (i = 0; i < 4; i++)
        spi_send(SPI1, 1);
}
