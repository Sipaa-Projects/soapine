/*
 * @file common/console.c
 * @brief Console applet - code
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
#include <bootloader-info.h>
#include <console.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

char *readline()
{
    char *buf = malloc(1024);
    memset(buf, 0, 1024);
    
    int buf_pos = 0;

    while (1)
    {
        int c = getchar();

        if (c < 256) {
            printf("%c", c);
            buf[buf_pos] = c;
            goto end;
        }
        if (c == GETCHAR_BACKSPACE)
        {
            if (buf_pos > 0) {
                printf("\b \b");
                buf_pos--;
            }
            continue;
        }
        else if (c == GETCHAR_ENTER)
        {
            printf("\n");
            buf[buf_pos] = '\0';
            return buf;
        }
end:
        buf_pos++;
    }

    return buf;
}

void help() {
    printf("Available commands:\n");
    printf("* shutdown (or poweroff): Shutdown the system\n");
    printf("* reboot (or poweroff -r): Reboot the system\n");
}

void console(int clear_term)
{
    if (clear_term > 0)
    {
        term_clear();
        term_set_cursor(true, 0, 0);
    }
    else
    {
        term_set_cursor(true, -1, -1);
    }

    printf("Soapine %s\n", SOAPINE_VERSION);
    printf("Copyright (C) 2024-present Sipaa Projects and the Soapine contributors.\n\n");

    while (1)
    {
        printf("> ");
        char *line = readline();

        if (strncmp(line, "shutdown", 8) == 0 || strncmp(line, "poweroff", 8) == 0)
            firmware_shutdown_system();
        if (strncmp(line, "reboot", 6) == 0 || strncmp(line, "poweroff -r", 11) == 0)
            firmware_reboot_system();
        
        free(line);
    }
}