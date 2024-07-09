/*
 * @file common/libc/stdlib.c
 * @brief C Library: Standard library - code
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
#include <menu.h>
#include <stdarg.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

void abort(bool allow_menu, char *msg, ...) // This isn't exactly POSIX but is needed for the user to know what happened.
{
    char buf[2048];
    va_list vl;
    va_start(vl, msg);
    vsnprintf(buf, 2048, msg, vl);

    term_reset_color(ALL);
    term_clear();
    term_set_cursor(false, 0, 0);
    term_set_color(true, 255, 0, 0);
    printf("ABORT");
    term_reset_color(ALL);
    printf(": %s\n\n", buf);

    if (!allow_menu)
    {
        printf("Press any key to reboot the system");
        getchar();
        firmware_reboot_system();
    }
    else
    {
        printf("Press any key to go back to the menu");
        getchar();
        menu(false);
    }
}

void *malloc(size_t size)
{
    void *ptr = firmware_malloc((int)size);
    if (!ptr)
        abort(false, "libc/stdlib: malloc: EFI_STATUS 0x%x", firmware_get_last_error());
    return ptr;
}

void free(void *ptr) { firmware_free(ptr); }

int atoi(char *p) {
    int result = 0;

    for (int i = 0; i < strlen(p); i++)
    {
        if (p[i] >= '0' && p[i] <= '9')
        {
            result *= 10;
            result += (int) (p[i]-'0');
        }
    }

    return result;
}

void usleep(int us) {
    firmware_usleep(us);
}

void sleep(int s) {
    firmware_sleep(s);
}