#include <lib/term.h>
#include <bootloader-info.h>
#include <console.h>
#include <stdio.h>

void console(int clear_term) {
    if (clear_term > 0) {
        term_clear();
        term_set_cursor(true, 0, 0);
    }
    else
    {
        term_set_cursor(true, -1, -1);
    }
    
    printf("Soapine %s\n", SOAPINE_VERSION);
    printf("Copyright (C) 2024-present Sipaa Projects and the Soapine contributors.\n\n");

    printf("> ");

    while (1)
    {
        int c = getchar();

        if (c < 256)
            printf("%c", c);
        else {
            if (c == GETCHAR_ESC)
                break;
        }
    }
}