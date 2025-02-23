/*
 * @file common/libc/stdio.c
 * @brief C Library: Standard input/output - code
 * 
 * Copyright (C) 2024-present Sipaa Projects & the Soapine contributors
 *
 * This piece of software is released under the terms of the MIT License
 */

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

void snprintf(char *buf, int bufsz, char *format, ...) {
    va_list args;
    va_start(args, buf);
    npf_vsnprintf(buf, PRINTF_BUF_SIZE, format, args);
    va_end(args);
}

void vsnprintf(char *buf, int bufsz, char *format, va_list args)
{
    npf_vsnprintf(buf, bufsz, format, args);
}

int getchar()
{
    return firmware_console_getchar();
}