#include <efi.h>
#include <efilib.h>

#include <lib/term.h>
#include <menu.h>
#include <stdio.h>

EFI_STATUS EFIAPI efi_main (EFI_HANDLE ImageHandle, EFI_SYSTEM_TABLE *SystemTable)
{
    InitializeLib(ImageHandle, SystemTable);
    term_init(FBTERM, SystemTable);

	menu(true);

    while (1) ;;
    return EFI_SUCCESS;
}