#include <lib/firmware.h>

#include <efi.h>
#include <efilib.h>
#include <string.h>

// NOTE: When using GNU-EFI, it is IMPORTANT to use 'uefi_call_wrapper', as you may encounter problems without using it.

EFI_STATUS status;

unsigned long long firmware_get_last_error()
{
    return status;
}

char firmware_console_getchar()
{
    UINTN EventIndex;
    EFI_INPUT_KEY Key;
    uefi_call_wrapper(gBS->WaitForEvent, 3, 1, &gST->ConIn->WaitForKey, &EventIndex);
    uefi_call_wrapper(gST->ConIn->ReadKeyStroke, 2, gST->ConIn, &Key);

    wchar_t cs = Key.UnicodeChar;
    char cs2;

    wctomb(&cs2, cs);

    return cs2;
}

void *firmware_malloc(int size)
{
    void *ptr = NULL;
    status = uefi_call_wrapper(BS->AllocatePool, 3, EfiLoaderData, (UINTN)size, ptr);
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