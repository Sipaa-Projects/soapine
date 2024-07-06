#include <lib/firmware.h>
#include <lib/term.h>
#include <trdparty/npf.h>
#include <stdio.h>
#include <stdarg.h>

#define PRINTF_BUF_SIZE 2048

void printf(char *format, ...) {
    char buf[PRINTF_BUF_SIZE];

    va_list args;
    va_start(args, buf);
    npf_vsnprintf(buf, PRINTF_BUF_SIZE, format, args);
    va_end(args);

    term_write(buf);
}

void vsnprintf(char *buf, int bufsz, char *format, va_list args)
{
    npf_vsnprintf(buf, bufsz, format, args);
}

char getchar()
{
    return firmware_console_getchar();
}