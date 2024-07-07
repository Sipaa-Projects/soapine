#pragma once

#include <lib/term.h>
#include <stdbool.h>

unsigned long long firmware_get_last_error();

int  firmware_console_getchar();
void firmware_console_print(char *text);
void firmware_console_set_cursor(bool visible, int x, int y);
void firmware_console_clear();
void firmware_console_reset_color(terminal_colorreset_type_t t);
void firmware_console_set_color(bool is_fg, int r, int g, int b); // Both VGA text mode & UEFI ConOut doesn't support True Color text.

void firmware_usleep(int usec);
void firmware_sleep(int sec);

void *firmware_malloc(int size);
void firmware_free(void *ptr);

void firmware_reboot_system();
void firmware_shutdown_system();