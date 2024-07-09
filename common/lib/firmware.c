/*
 * @file common/lib/firmware.c
 * @brief Firmware-specific functions - code
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

#include <string.h>
#include <stdio.h>

// NOTE: When using GNU-EFI, it is IMPORTANT to use 'uefi_call_wrapper', as you may encounter problems without using it.

EFI_STATUS status;
EFI_LOADED_IMAGE_PROTOCOL* loaded_image;
EFI_SIMPLE_FILE_SYSTEM_PROTOCOL* filesystem;

unsigned long long firmware_get_last_error()
{
    return status;
}

EFI_STATUS firmware_init(){
	EFI_STATUS s = uefi_call_wrapper(BS->LocateProtocol, 3, &gEfiLoadedImageProtocolGuid, NULL, (void**)&loaded_image);
    if(EFI_ERROR(s)) return s;
    printf("soapine: libc/fileio: Located ImageProtocol\n");
	s = uefi_call_wrapper(BS->LocateProtocol, 3, &gEfiSimpleFileSystemProtocolGuid, NULL, (void**)&filesystem);
    if(EFI_ERROR(s)) return s;
    printf("soapine: libc/fileio: Located SimpleFileSystemProtocol\n");

    return EFI_SUCCESS;
}

int firmware_console_getchar()
{
    UINTN EventIndex;
    EFI_INPUT_KEY Key;
    int cr_passed = 0;

    while (TRUE) {
        status = uefi_call_wrapper(gST->ConIn->ReadKeyStroke, 2, gST->ConIn, &Key);
        if (EFI_ERROR(status)) {
            continue;
        }

        switch (Key.ScanCode) {
            case SCAN_UP:
                return GETCHAR_CURSOR_UP;
            case SCAN_DOWN:
                return GETCHAR_CURSOR_DOWN;
            case SCAN_RIGHT:
                return GETCHAR_CURSOR_RIGHT;
            case SCAN_LEFT:
                return GETCHAR_CURSOR_LEFT;
            case SCAN_F10:
                return GETCHAR_F10;
            case SCAN_DELETE:
                return GETCHAR_DELETE;
            case SCAN_END:
                return GETCHAR_END;
            case SCAN_HOME:
                return GETCHAR_HOME;
            case SCAN_PAGE_UP:
                return GETCHAR_PGUP;
            case SCAN_PAGE_DOWN:
                return GETCHAR_PGDOWN;
        }

        if (Key.UnicodeChar == CHAR_CARRIAGE_RETURN) {
            return GETCHAR_ENTER;
        }

        if (Key.UnicodeChar == CHAR_BACKSPACE)
            return GETCHAR_BACKSPACE;

        if (Key.UnicodeChar == CHAR_TAB)
            return GETCHAR_TAB;

        if (Key.UnicodeChar == CHAR_NULL && Key.ScanCode == SCAN_ESC) {
            return GETCHAR_ESC;
        }

        char cs2;

        wctomb(&cs2, Key.UnicodeChar);

        return cs2;
    }

}

void *firmware_malloc(int size)
{
    void *ptr = NULL;
    status = uefi_call_wrapper(BS->AllocatePool, 3, EfiLoaderData, (UINTN)size, &ptr);
    if (EFI_ERROR(status))
        return NULL;
    return ptr;
}

void firmware_free(void *ptr)
{
    status = uefi_call_wrapper(BS->FreePool, 3, ptr);
}

void firmware_reboot_system()
{
    uefi_call_wrapper(RT->ResetSystem, 4, EfiResetCold, status, 0, NULL);
}

void firmware_shutdown_system()
{
    uefi_call_wrapper(RT->ResetSystem, 4, EfiResetShutdown, status, 0, NULL);
}

EFI_FILE* firmware_open(EFI_FILE* directory, char* path){
	EFI_FILE* new;
	if (directory == NULL){
		uefi_call_wrapper(filesystem->OpenVolume, 2, filesystem, &directory);
    }

    CHAR16 wpath[4096];
    mbstowcs(wpath, path, strlen(path));
    wpath[strlen(path)] = '\0';

	EFI_STATUS s = uefi_call_wrapper(directory->Open, 5, directory, &new, wpath, EFI_FILE_MODE_READ, 0);

	if (s != EFI_SUCCESS){
		return NULL;
	}
	return new;
}

EFI_STATUS firmware_close(EFI_FILE* file){
    return uefi_call_wrapper(file->Close, 1, file);
}

int firmware_get_size(EFI_FILE* file){
    int FileInfoSize;
	EFI_FILE_INFO* FileInfo;

    uefi_call_wrapper(file->GetInfo, 4, file, &gEfiFileInfoGuid, &FileInfoSize, NULL);

	uefi_call_wrapper(BS->AllocatePool, 3, EfiLoaderData, FileInfoSize, (void**)&FileInfo);
	uefi_call_wrapper(file->GetInfo, 4, file, &gEfiFileInfoGuid, &FileInfoSize, (void**)&FileInfo);

    int file_size = FileInfo->FileSize;

    uefi_call_wrapper(BS->FreePool, 1, FileInfo);

    return file_size;
}

EFI_STATUS firmware_read(EFI_FILE* file, int size, void* buffer){
    return uefi_call_wrapper(file->Read, 3, file, &size, buffer);
}
EFI_STATUS firmware_write(EFI_FILE* file, int size, void* buffer){
    return uefi_call_wrapper(file->Write, 3, file, &size, buffer);
}