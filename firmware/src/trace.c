#include <trace.h>

void traceInit(void)
{
    // Following RM0008 § 31.14.2 and § 31.17.10

    // Enable trace subsystem
    SCS_DEMCR |= SCS_DEMCR_TRCENA;

    // Set port protocol to SWO NRZ (UART-like)
    TPIU_SPPR = TPIU_SPPR_ASYNC_NRZ;

    // Set async clock
    TPIU_ACPR = 72000000 / 2000000 - 1;

    // Disable formatter and flush control
    TPIU_FFCR &= ~TPIU_FFCR_ENFCONT;

    // Enable trace IO in async mode
    DBGMCU_CR |= DBGMCU_CR_TRACE_IOEN | DBGMCU_CR_TRACE_MODE_ASYNC;

    // Unlock ITM registers access
    ITM_LAR = 0xC5ACCE55;

    // Enable ITM with ATB ID 1
    ITM_TCR |= (1 << 16) | ITM_TCR_ITMENA;

    // Enable Stimulus port 0
    ITM_TER[0] = 1;
}
