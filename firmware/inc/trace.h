#ifndef CASECOMMANDER_TRACE_H
#define CASECOMMANDER_TRACE_H

#include <libopencm3/cm3/itm.h>
#include <libopencm3/cm3/scs.h>
#include <libopencm3/cm3/tpiu.h>
#include <libopencm3/stm32/dbgmcu.h>

#define CC_TRACE_CLOCK        2000000
#define CC_TRACE_CONSOLE_STIM 0

void traceInit(void);
void traceWriteChar(const char chr);
void traceWriteString(const char *str, uint32_t len);
void tracePrint(const char *str);
void tracePrintLine(const char *str);

#endif
