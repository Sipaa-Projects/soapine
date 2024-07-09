/*
 * @file common/lib/main.c
 * @brief Entry point for UEFI platforms.
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

#include <efi.h>
#include <efilib.h>

#include <lib/config.h>
#include <lib/firmware.h>
#include <lib/term.h>
#include <console.h>
#include <menu.h>
#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>


void config_initialize() {

    EFI_FILE* file = firmware_open(NULL, "\\soapine.cfg");

    if (   (file = firmware_open(NULL, "\\soapine.cfg")) == NULL
        && (file = firmware_open(NULL, "\\EFI\\soapine.cfg")) == NULL
        && (file = firmware_open(NULL, "\\EFI\\BOOT\\soapine.cfg")) == NULL
        && (file = firmware_open(NULL, "\\soapine\\soapine.cfg")) == NULL){
        printf("No configuration file found! Please refer to the Soapine documentation.\n");
        printf("Dropping to a console...\n\n");

        console(0);
    } else {
        char buffer[4096];
        firmware_read(file, 4096, buffer);

        config_init(buffer);

        if (config_parsing_errors_present != 0 || config_get_menu_root() == NULL)
        {
            printf("\n");
            printf("Configuration parser completed with errors!\n");
            printf("Please take a shot of the messages and correct your config file according to the parser messages.\n\n");
            printf("Dropping to console...\n");

            console(0);
        }

        firmware_close(file);
    }
}

EFI_STATUS EFIAPI efi_main (EFI_HANDLE ImageHandle, EFI_SYSTEM_TABLE *SystemTable)
{
    InitializeLib(ImageHandle, SystemTable);
    term_init(FBTERM);

    EFI_STATUS s = firmware_init();
    if (s != EFI_SUCCESS)
        abort(false, "Failed to init fio: %x", s);
    
    config_initialize();

    menu(true);

    while (1) ;;
    return EFI_SUCCESS;
}