#include <trace.h>

int _write(int fd, char *ptr, int len);

int _write(int fd, char *ptr, int len)
{
    (void)fd;

    traceWriteString(ptr, len);
    return len;
}
