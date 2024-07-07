#include <efi.h>
#include <efilib.h>

#include <lib/config.h>
#include <lib/term.h>
#include <libc/fileio.h>
#include <menu.h>
#include <stdio.h>

EFI_STATUS EFIAPI efi_main (EFI_HANDLE ImageHandle, EFI_SYSTEM_TABLE *SystemTable)
{
    InitializeLib(ImageHandle, SystemTable);
    term_init(FBTERM);

	char *config = 
        "MENU_HEADERBAR_BG=#FFFFFF\n"
        "\n"
        "menu_entry \"Windows\" {\n"
        "   PROTOCOL=\"chainload\"\n"
        "   IMAGE_PATH=\"EFI\\Microsoft\\Boot\\bootmgfw.efi\"\n"
        "};\n"
        "\n"
        "menu_entry \"Bimbows\" {\n"
        "   PROTOCOL=\"bimbows\"\n"
        "   IMAGE_PATH=\"bimkern.exe\"\n"
        "};\n";
	config_init(config);

	if (config_parsing_errors_present != 0)
	{
        printf("\n");
		printf("Configuration parser completed with errors!\n");
        printf("Please take a shot of the messages and correct your config file according to the parser messages.\n\n");
        printf("System halted\n");

        asm("cli");
        while (1)
            asm("hlt");
	}

    /**EFI_STATUS s = init_fio(ImageHandle);
    if (s != EFI_SUCCESS)
        printf("Failed to init fio: %x", s);

    EFI_FILE* file = open(NULL, "test\\hello.txt");

    if(file == NULL){
        printf("No file :cry:");
        close(file);
    } else {
        char buffer[1024];
        read(file, 10, buffer);
        printf("File contents: %s",buffer);
        close(file);
    }**/

	//menu(true);

    while (1) ;;
    return EFI_SUCCESS;
}