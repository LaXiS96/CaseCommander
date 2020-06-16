#include <tacho.h>

typedef volatile struct tachoChannelRawValues_t {
    bool     firstEdge;
    uint16_t currentValue;
    uint16_t lastValue;
    uint8_t  overflowCount;
} tachoChannelRawValues_t;

typedef volatile struct tachoRawValues_t {
    tachoChannelRawValues_t ch1;
    tachoChannelRawValues_t ch2;
    tachoChannelRawValues_t ch3;
    tachoChannelRawValues_t ch4;
} tachoRawValues_t;

static tachoRawValues_t rawValues = {};

void tim2_isr(void)
{
    // Channel 1 interrupt
    if (timer_get_flag(TIM2, TIM_SR_CC1IF)) {
        rawValues.ch1.overflowCount = 0;

        if (rawValues.ch1.firstEdge) {
            rawValues.ch1.firstEdge = false;
        } else {
            rawValues.ch1.lastValue    = rawValues.ch1.currentValue;
            rawValues.ch1.currentValue = TIM_CCR1(TIM2);
            rawValues.ch1.firstEdge    = true;
        }

        timer_clear_flag(TIM2, TIM_SR_CC1IF);
    }

    // Channel 2 interrupt
    if (timer_get_flag(TIM2, TIM_SR_CC2IF)) {
        rawValues.ch2.overflowCount = 0;

        if (rawValues.ch2.firstEdge) {
            rawValues.ch2.firstEdge = false;
        } else {
            rawValues.ch2.lastValue    = rawValues.ch2.currentValue;
            rawValues.ch2.currentValue = TIM_CCR2(TIM2);
            rawValues.ch2.firstEdge    = true;
        }

        timer_clear_flag(TIM2, TIM_SR_CC2IF);
    }

    // Channel 3 interrupt
    if (timer_get_flag(TIM2, TIM_SR_CC3IF)) {
        rawValues.ch3.overflowCount = 0;

        if (rawValues.ch3.firstEdge) {
            rawValues.ch3.firstEdge = false;
        } else {
            rawValues.ch3.lastValue    = rawValues.ch3.currentValue;
            rawValues.ch3.currentValue = TIM_CCR3(TIM2);
            rawValues.ch3.firstEdge    = true;
        }

        timer_clear_flag(TIM2, TIM_SR_CC3IF);
    }

    // Channel 4 interrupt
    if (timer_get_flag(TIM2, TIM_SR_CC4IF)) {
        rawValues.ch4.overflowCount = 0;

        if (rawValues.ch4.firstEdge) {
            rawValues.ch4.firstEdge = false;
        } else {
            rawValues.ch4.lastValue    = rawValues.ch4.currentValue;
            rawValues.ch4.currentValue = TIM_CCR4(TIM2);
            rawValues.ch4.firstEdge    = true;
        }

        timer_clear_flag(TIM2, TIM_SR_CC4IF);
    }

    // Counter overflow interrupt
    if (timer_get_flag(TIM2, TIM_SR_UIF)) {
        if (++(rawValues.ch1.overflowCount) > 254)
            (rawValues.ch1.overflowCount)--;

        if (++(rawValues.ch2.overflowCount) > 254)
            (rawValues.ch2.overflowCount)--;

        if (++(rawValues.ch3.overflowCount) > 254)
            (rawValues.ch3.overflowCount)--;

        if (++(rawValues.ch4.overflowCount) > 254)
            (rawValues.ch4.overflowCount)--;

        timer_clear_flag(TIM2, TIM_SR_UIF);
    }
}

void tachoInit(void)
{
    // Enable and reset timer 2 peripheral
    rcc_periph_clock_enable(RCC_TIM2);
    rcc_periph_reset_pulse(RST_TIM2);

    // Internal clock, edge triggered, upcounting
    timer_set_mode(TIM2, TIM_CR1_CKD_CK_INT, TIM_CR1_CMS_EDGE, TIM_CR1_DIR_UP);
    // Internal clock prescaler (72MHz/720 = 100kHz sampling rate)
    timer_set_prescaler(TIM2, 720);

    // Enable input channels (PA0 to PA3)
    timer_ic_set_input(TIM2, TIM_IC1, TIM_IC_IN_TI1);
    timer_ic_set_input(TIM2, TIM_IC2, TIM_IC_IN_TI2);
    timer_ic_set_input(TIM2, TIM_IC3, TIM_IC_IN_TI3);
    timer_ic_set_input(TIM2, TIM_IC4, TIM_IC_IN_TI4);

    // Digital filter: 8 values
    timer_ic_set_filter(TIM2, TIM_IC1, TIM_IC_CK_INT_N_8);
    timer_ic_set_filter(TIM2, TIM_IC2, TIM_IC_CK_INT_N_8);
    timer_ic_set_filter(TIM2, TIM_IC3, TIM_IC_CK_INT_N_8);
    timer_ic_set_filter(TIM2, TIM_IC4, TIM_IC_CK_INT_N_8);

    // Enable input channels
    timer_ic_enable(TIM2, TIM_IC1);
    timer_ic_enable(TIM2, TIM_IC2);
    timer_ic_enable(TIM2, TIM_IC3);
    timer_ic_enable(TIM2, TIM_IC4);

    // Enable interrupts
    nvic_enable_irq(NVIC_TIM2_IRQ);
    timer_enable_irq(
        TIM2, TIM_DIER_CC1IE | TIM_DIER_CC2IE | TIM_DIER_CC3IE | TIM_DIER_CC4IE | TIM_DIER_UIE);

    // Start timer
    timer_enable_counter(TIM2);
}

tachoValues_t tachoGetValues(void)
{
    tachoValues_t values = {};

    if (rawValues.ch1.overflowCount >= 2) {
        values.ch1.noSignal = true;
        values.ch1.rpm      = 0;
    } else {
        values.ch1.noSignal = false;
        values.ch1.rpm      = 10000000 /
                         (rawValues.ch1.currentValue - rawValues.ch1.lastValue +
                          (rawValues.ch1.currentValue < rawValues.ch1.lastValue ? 65536 : 0)) *
                         60 / 100;
    }

    if (rawValues.ch2.overflowCount >= 2) {
        values.ch2.noSignal = true;
        values.ch2.rpm      = 0;
    } else {
        values.ch2.noSignal = false;
        values.ch2.rpm      = 10000000 /
                         (rawValues.ch2.currentValue - rawValues.ch2.lastValue +
                          (rawValues.ch2.currentValue < rawValues.ch2.lastValue ? 65536 : 0)) *
                         60 / 100;
    }

    if (rawValues.ch3.overflowCount >= 2) {
        values.ch3.noSignal = true;
        values.ch3.rpm      = 0;
    } else {
        values.ch3.noSignal = false;
        values.ch3.rpm      = 10000000 /
                         (rawValues.ch3.currentValue - rawValues.ch3.lastValue +
                          (rawValues.ch3.currentValue < rawValues.ch3.lastValue ? 65536 : 0)) *
                         60 / 100;
    }

    if (rawValues.ch4.overflowCount >= 2) {
        values.ch4.noSignal = true;
        values.ch4.rpm      = 0;
    } else {
        values.ch4.noSignal = false;
        values.ch4.rpm      = 10000000 /
                         (rawValues.ch4.currentValue - rawValues.ch4.lastValue +
                          (rawValues.ch4.currentValue < rawValues.ch4.lastValue ? 65536 : 0)) *
                         60 / 100;
    }

    return values;
}
