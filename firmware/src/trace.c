#include <trace.h>

void traceInit(void)
{
    /**
     * Summary of subsystems:
     * - DWT (Data Watchpoint Trigger): can be used to obtain various types of trace data straight
     * from the core
     * - ITM (Instrumentation Trace Macrocell): allows the application to directly send custom trace
     * data via up to 32 asynchronous UART-like channels (stimulus ports)
     * - TPIU (Trace Port Interface Unit): handles output protocol of trace data
     * - DBGMCU: controls debug behaviour (like choosing to keep peripherals running while the core
     * is halted) and controls trace pin assignment
     *
     * See:
     * - STM32F1 Reference Manual (RM0008) §31
     * - ARMv7-M Architecture Reference Manual §C1
     * - http://infocenter.arm.com/help/index.jsp?topic=/com.arm.doc.ddi0314h/index.html
     */

    // Enable trace subsystem
    SCS_DEMCR |= SCS_DEMCR_TRCENA;

    // Set port protocol to SWO NRZ (UART-like)
    TPIU_SPPR = TPIU_SPPR_ASYNC_NRZ;

    // Set async clock prescaler (CLKIN / CLKOUT - 1)
    TPIU_ACPR = 72000000 / CC_TRACE_CLOCK - 1;

    // Disable formatter and flush control
    TPIU_FFCR &= ~TPIU_FFCR_ENFCONT;

    // Enable trace IO in async mode
    DBGMCU_CR &= ~DBGMCU_CR_TRACE_MODE_MASK;
    DBGMCU_CR |= DBGMCU_CR_TRACE_IOEN | DBGMCU_CR_TRACE_MODE_ASYNC;

    // Unlock write access to ITM_TCR
    ITM_LAR = 0xC5ACCE55;

    // Enable ITM with ATB ID 1 (identifier for multi-source trace)
    ITM_TCR &= ~ITM_TCR_TRACE_BUS_ID_MASK;
    ITM_TCR |= (1 << 16) | ITM_TCR_ITMENA;

    // Enable Stimulus ports
    ITM_TER[CC_TRACE_CONSOLE_STIM] = 1;
}

void traceWriteChar(const char chr)
{
    // Wait until stimulus port is ready for new data
    while (!(ITM_STIM8(CC_TRACE_CONSOLE_STIM) & ITM_STIM_FIFOREADY))
        ;

    ITM_STIM8(CC_TRACE_CONSOLE_STIM) = chr;
}

void traceWriteString(const char *str, uint32_t len)
{
    for (uint32_t i = 0; i < len; i++) {
        traceWriteChar(str[i]);
    }
}

void tracePrint(const char *str)
{
    const char *chr = str;

    do {
        traceWriteChar(*chr);
    } while (*(++chr) != 0);
}

void tracePrintLine(const char *str)
{
    tracePrint(str);
    traceWriteChar('\n');
}
