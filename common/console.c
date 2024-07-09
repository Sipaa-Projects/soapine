/*
 * @file common/console.c
 * @brief Console applet - code
 * 
 * Copyright (C) 2024-present Sipaa Projects & the Soapine contributors
 *
 * This piece of software is released under the terms of the MIT License
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
        else if (strncmp(line, "reboot", 6) == 0 || strncmp(line, "poweroff -r", 11) == 0)
            firmware_reboot_system();
        else if (strncmp(line, "init_fbterm", 11) == 0)
        {
            if (term_type == FBTERM)
            {
                printf("FBTERM is already your current terminal backend.\n");
                continue;
            }

            term_init(FBTERM);
            term_clear();
            if (term_type == FBTERM)
                printf("FBTERM is now your current terminal backend.\n");
            else
                printf("We couldn't set FBTERM as your current terminal backend.\n");
        } else if (strncmp(line, "exit", 4) == 0)
            break;
        
        free(line);
    }
}