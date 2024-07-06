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
    "{\"user\": \"johndoe\", \"admin\": false, \"uid\": 1000,\n  "
    "\"groups\": [\"users\", \"wheel\", \"audio\", \"video\"]}";;
	config_init(config);

	if (config_initialized == 0)
	{
		printf("Configuration initialization failed: %s\n", config_parser_result);
	}
	else
	{
		printf("Config file parsed successfully!\n");
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