#include "tacho.h"
// TODO change include quotes to <> everywhere

volatile uint32_t debugStatus = 0;

tachoValues_t tachoValues = {};

typedef volatile struct tachoChannelRawValues_t {
    bool     firstEdge;
    uint16_t lastValue;
    uint8_t  overflowCount;
} tachoChannelRawValues_t;

typedef volatile struct tachoRawValues_t {
    tachoChannelRawValues_t ch1;
    tachoChannelRawValues_t ch2;
    tachoChannelRawValues_t ch3;
    tachoChannelRawValues_t ch4;
} tachoRawValues_t;

void tim2_isr(void)
{
    static tachoRawValues_t rawValues = {};

    const uint32_t interruptFlags[] = {
        TIM_SR_CC1IF,
        TIM_SR_CC2IF,
        TIM_SR_CC3IF,
        TIM_SR_CC4IF,
    };
    volatile uint32_t *const ccRegistersPtr[] = {
        &TIM_CCR1(TIM2),
        &TIM_CCR2(TIM2),
        &TIM_CCR3(TIM2),
        &TIM_CCR4(TIM2),
    };
    static tachoChannelRawValues_t *channelRawValuesPtr[] = {
        (void *)&rawValues,
        (void *)&rawValues + sizeof(tachoChannelRawValues_t) * 1,
        (void *)&rawValues + sizeof(tachoChannelRawValues_t) * 2,
        (void *)&rawValues + sizeof(tachoChannelRawValues_t) * 3,
    };
    static tachoChannelValues_t *channelValuesPtr[] = {
        (void *)&tachoValues,
        (void *)&tachoValues + sizeof(tachoChannelValues_t) * 1,
        (void *)&tachoValues + sizeof(tachoChannelValues_t) * 2,
        (void *)&tachoValues + sizeof(tachoChannelValues_t) * 3,
    };

    for (uint8_t i = 0; i < 4; i++) {
        if (timer_get_flag(TIM2, interruptFlags[i])) {
            channelRawValuesPtr[i]->overflowCount = 0;
            channelValuesPtr[i]->noSignal         = false;

            // Skip the first edge and sample on the next, this solves the periodical jumps in RPMs
            // caused by the jitter in N-S/S-N pole pass timing by only considering N-N or S-S pole
            // passes
            if (channelRawValuesPtr[i]->firstEdge) {
                channelRawValuesPtr[i]->firstEdge = false;
            } else {
                debugStatus = 1;
                // TODO whytf does this drop samples? while the split version just below doesn't
                channelValuesPtr[i]->rpm =
                    10000000 / (*ccRegistersPtr[i] - channelRawValuesPtr[i]->lastValue) * 60 / 100;
                // channelValuesPtr[i]->rpm = *ccRegistersPtr[i] -
                // channelRawValuesPtr[i]->lastValue; channelValuesPtr[i]->rpm = 10000000 /
                // channelValuesPtr[i]->rpm * 60 / 100;
                channelRawValuesPtr[i]->lastValue = *ccRegistersPtr[i];
                channelRawValuesPtr[i]->firstEdge = true;
            }

            timer_clear_flag(TIM2, interruptFlags[i]);
        }
        if (timer_get_flag(TIM2, TIM_SR_UIF)) {
            // If the timer overflowed at least twice, the fan was disconnected
            if (++(channelRawValuesPtr[i]->overflowCount) >= 2) {
                debugStatus = 2;
                channelValuesPtr[i]->rpm      = 0;
                channelValuesPtr[i]->noSignal = true;
                (channelRawValuesPtr[i]->overflowCount)--;
            }
        }
    }

    timer_clear_flag(TIM2, TIM_SR_UIF);
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

    // Enable input channel 1 (PA0) to IC input 1
    timer_ic_set_input(TIM2, TIM_IC1, TIM_IC_IN_TI1);
    timer_ic_set_input(TIM2, TIM_IC2, TIM_IC_IN_TI2);
    timer_ic_set_input(TIM2, TIM_IC3, TIM_IC_IN_TI3);
    timer_ic_set_input(TIM2, TIM_IC4, TIM_IC_IN_TI4);

    // Filter channel 1: 8 values
    timer_ic_set_filter(TIM2, TIM_IC1, TIM_IC_CK_INT_N_8);
    timer_ic_set_filter(TIM2, TIM_IC2, TIM_IC_CK_INT_N_8);
    timer_ic_set_filter(TIM2, TIM_IC3, TIM_IC_CK_INT_N_8);
    timer_ic_set_filter(TIM2, TIM_IC4, TIM_IC_CK_INT_N_8);

    // Enable input channel 1
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
