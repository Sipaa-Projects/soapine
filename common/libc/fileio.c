#include "fileio.h"
#include <libc/string.h>
#include <stdio.h>

EFI_LOADED_IMAGE_PROTOCOL* loaded_image;
EFI_SIMPLE_FILE_SYSTEM_PROTOCOL* filesystem;

EFI_STATUS init_fio(EFI_HANDLE handle){
	EFI_STATUS s = uefi_call_wrapper(BS->LocateProtocol, 3, &gEfiLoadedImageProtocolGuid, NULL, (void**)&loaded_image);
    if(EFI_ERROR(s)) return s;
    printf("soapine: libc/fileio: Located ImageProtocol\n");
	s = uefi_call_wrapper(BS->LocateProtocol, 3, &gEfiSimpleFileSystemProtocolGuid, NULL, (void**)&filesystem);
    if(EFI_ERROR(s)) return s;
    printf("soapine: libc/fileio: Located SimpleFileSystemProtocol\n");

    return EFI_SUCCESS;
}

EFI_FILE* open(EFI_FILE* directory, char* path){
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

EFI_STATUS close(EFI_FILE* file){
    return uefi_call_wrapper(file->Close, 1, file);
}

UINTN get_size(EFI_FILE* file){
    UINTN FileInfoSize;
	EFI_FILE_INFO* FileInfo;

    uefi_call_wrapper(file->GetInfo, 4, file, &gEfiFileInfoGuid, &FileInfoSize, NULL);

	uefi_call_wrapper(BS->AllocatePool, 3, EfiLoaderData, FileInfoSize, (void**)&FileInfo);
	uefi_call_wrapper(file->GetInfo, 4, file, &gEfiFileInfoGuid, &FileInfoSize, (void**)&FileInfo);

    UINTN file_size = FileInfo->FileSize;

    uefi_call_wrapper(BS->FreePool, 1, FileInfo);

    return file_size;
}

EFI_STATUS read(EFI_FILE* file, UINTN size, void* buffer){
    return uefi_call_wrapper(file->Read, 3, file, &size, buffer);
}
EFI_STATUS write(EFI_FILE* file, UINTN size, void* buffer){
    return uefi_call_wrapper(file->Write, 3, file, &size, buffer);
}