#include "util.h"

static char printBuf[256];
 
const char * cc_sprintf(const char *format, ...) {
    // TODO this will cause issues with multithreading... printBuf is not synchronized
    // maybe move this wrapper/functionality to usbWriteFormat
    va_list args;
    va_start(args, format);
    vsnprintf(printBuf, sizeof(printBuf), format, args);
    va_end(args);

    return printBuf;
}
