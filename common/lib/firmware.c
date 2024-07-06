#include <lib/firmware.h>

#include <efi.h>
#include <efilib.h>
#include <string.h>

// Here is firmware-dependant functions.
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