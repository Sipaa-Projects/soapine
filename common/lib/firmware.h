/*
 * @file common/lib/firmware.h
 * @brief Firmware-specific functions - header
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

#pragma once

#include <efi.h>
#include <efilib.h>
#include <lib/term.h>
#include <stdbool.h>

EFI_STATUS firmware_init();
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
void  firmware_free(void *ptr);

void firmware_reboot_system();
void firmware_shutdown_system();

// TODO: It's a war crime to expose the UEFI API in headers.

EFI_FILE*  firmware_open(EFI_FILE* directory, char* path);
EFI_STATUS firmware_close(EFI_FILE* file);
int        firmware_get_size(EFI_FILE* file);
EFI_STATUS firmware_read(EFI_FILE* file, int size, void* buffer);
EFI_STATUS firmware_write(EFI_FILE* file, int size, void* buffer);