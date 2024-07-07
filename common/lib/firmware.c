#include <lib/firmware.h>

#include <efi.h>
#include <efilib.h>
#include <string.h>
#include <stdio.h>

// NOTE: When using GNU-EFI, it is IMPORTANT to use 'uefi_call_wrapper', as you may encounter problems without using it.

EFI_STATUS status;

unsigned long long firmware_get_last_error()
{
    return status;
}

int firmware_console_getchar()
{
    UINTN EventIndex;
    EFI_INPUT_KEY Key;

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