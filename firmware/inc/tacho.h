#ifndef CASECOMMANDER_TACHO_H
#define CASECOMMANDER_TACHO_H

#include <libopencm3/cm3/nvic.h>
#include <libopencm3/stm32/gpio.h>
#include <libopencm3/stm32/rcc.h>
#include <libopencm3/stm32/timer.h>

typedef struct tachoChannelValues_t {
    uint16_t rpm;
    bool     noSignal;
} tachoChannelValues_t;

typedef struct tachoValues_t {
    tachoChannelValues_t ch1;
    tachoChannelValues_t ch2;
    tachoChannelValues_t ch3;
    tachoChannelValues_t ch4;
} tachoValues_t;

void          tachoInit(void);
tachoValues_t tachoGetValues(void);

#endif
