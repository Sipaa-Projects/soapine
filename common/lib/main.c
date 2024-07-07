#include <efi.h>
#include <efilib.h>

#include <lib/config.h>
#include <lib/term.h>
#include <libc/fileio.h>
#include <console.h>
#include <menu.h>
#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>

void config_initialize() {

    EFI_FILE* file = open(NULL, "\\soapine.cfg");
    
    if (   (file = open(NULL, "\\soapine.cfg")) == NULL
        && (file = open(NULL, "\\EFI\\soapine.cfg")) == NULL
        && (file = open(NULL, "\\EFI\\BOOT\\soapine.cfg")) == NULL
        && (file = open(NULL, "\\soapine\\soapine.cfg")) == NULL){
        printf("No configuration file found! Please refer to the Soapine documentation.\n");
        printf("Dropping to a console...\n\n");
        
        console(0);
    } else {
        char buffer[4096];
        read(file, 4096, buffer);

        config_init(buffer);

        if (config_parsing_errors_present != 0 || config_get_menu_root() == NULL)
        {
            printf("\n");
            printf("Configuration parser completed with errors!\n");
            printf("Please take a shot of the messages and correct your config file according to the parser messages.\n\n");
            printf("Dropping to console...\n");

            console(0);
        }

        close(file);
    }
}

EFI_STATUS EFIAPI efi_main (EFI_HANDLE ImageHandle, EFI_SYSTEM_TABLE *SystemTable)
{
    InitializeLib(ImageHandle, SystemTable);
    term_init(FBTERM);

    EFI_STATUS s = init_fio(ImageHandle);
    if (s != EFI_SUCCESS)
        abort(false, "Failed to init fio: %x", s);
    
    config_initialize();

	/**char *config = 
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
        "};\n";**/

    /****/

	menu(true);

    while (1) ;;
    return EFI_SUCCESS;
}