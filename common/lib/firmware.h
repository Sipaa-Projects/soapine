#pragma once

#include <lib/term.h>
#include <stdbool.h>

char firmware_console_getchar();
void firmware_console_print(char *text);
void firmware_console_set_cursor(bool visible, int x, int y);
void firmware_console_clear();
void firmware_console_reset_color(terminal_colorreset_type_t t);
void firmware_console_set_color(bool is_fg, int r, int g, int b); // Both VGA text mode & UEFI ConOut doesn't support True Color text.

void firmware_usleep(int usec);
void firmware_sleep(int sec);