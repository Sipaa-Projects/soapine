/*
 * @file common/libc/stdio.c
 * @brief C Library: Standard input/output - code
 * 
 * Copyright (C) 2024 Sipaa Projects and the Soapine contributors
 *
 * This file is part of the Soapine bootloader
 * 
 * Soapine is free software: you can redistribute it and/or modify it
 * under the terms of the GNU General Public License as published by the Free
 * Software Foundation, either version 3 of the License, or (at your option)
 * any later version.
 *
 * Soapine is distributed in the hope that it will be useful, but WITHOUT
 * ANY WARRANTY; without even the implied warranty of  MERCHANTABILITY or
 * FITNESS FOR A PARTICULAR PURPOSE. See the GNU General Public License for
 * more details.
 *
 * You should have received a copy of the GNU General Public License along with
 * Soapine.  If not, see <http://www.gnu.org/licenses/>.
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