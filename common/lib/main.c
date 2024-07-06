#include <efi.h>
#include <efilib.h>

#include <lib/term.h>
#include <libc/fileio.h>
#include <menu.h>
#include <stdio.h>

EFI_STATUS EFIAPI efi_main (EFI_HANDLE ImageHandle, EFI_SYSTEM_TABLE *SystemTable)
{
    InitializeLib(ImageHandle, SystemTable);
    term_init(FBTERM);
    EFI_STATUS s = init_fio(ImageHandle);
    if (s != EFI_SUCCESS)
        printf("Failed to init fio: %x", s);

    EFI_FILE* file = open(NULL, "hello.txt");
	
    if(file == NULL){
        printf("No file :cry:");
        close(file);
    } else {
        char buffer[1024];
        read(file, 10, buffer);
        printf("File contents: %s",buffer);
        close(file);
    }

	//menu(true);

    while (1) ;;
    return EFI_SUCCESS;
}