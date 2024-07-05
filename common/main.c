#include <efi.h>
#include <efilib.h>

#include <lib/term.h>

EFI_STATUS EFIAPI efi_main (EFI_HANDLE ImageHandle, EFI_SYSTEM_TABLE *SystemTable)
{
    InitializeLib(ImageHandle, SystemTable);
    term_init(FBTERM);

    term_write("Hello World! Café (to test accents)\n");

    while (1) ;;
    return EFI_SUCCESS;
}