#include "fileio.h"
#include <libc/string.h>
#include <stdio.h>

EFI_LOADED_IMAGE_PROTOCOL* loaded_image;
EFI_SIMPLE_FILE_SYSTEM_PROTOCOL* filesystem;

EFI_STATUS init_fio(EFI_HANDLE handle){
	EFI_STATUS s = BS->LocateProtocol(handle, &gEfiLoadedImageProtocolGuid, (void**)&loaded_image);
    if(s != EFI_SUCCESS) return s;
	s = BS->LocateProtocol(loaded_image->DeviceHandle, &gEfiSimpleFileSystemProtocolGuid, (void**)&filesystem);
    if(s != EFI_SUCCESS) return s;
}

EFI_FILE* open(EFI_FILE* directory, char* path){
	EFI_FILE* new;

	if (directory == NULL){
		filesystem->OpenVolume(filesystem, &directory);
    }

    CHAR16 wpath[4096];
    mbstowcs(wpath, path, strlen(path));
    wpath[strlen(path)] = '\0';

	EFI_STATUS s = directory->Open(directory, &new, wpath, EFI_FILE_MODE_READ, 0);

	if (s != EFI_SUCCESS){
		return NULL;
	}
	return new;
}
EFI_STATUS close(EFI_FILE* file){
    return file->Close(file);
}

UINTN get_size(EFI_FILE* file){
    UINTN FileInfoSize;
	EFI_FILE_INFO* FileInfo;

    file->GetInfo(file, &gEfiFileInfoGuid, &FileInfoSize, NULL);

	BS->AllocatePool(EfiLoaderData, FileInfoSize, (void**)&FileInfo);
	file->GetInfo(file, &gEfiFileInfoGuid, &FileInfoSize, (void**)&FileInfo);

    UINTN file_size = FileInfo->FileSize;

    BS->FreePool(FileInfo);

    return file_size;
}

EFI_STATUS read(EFI_FILE* file, UINTN size, void* buffer){
    return file->Read(file, &size, buffer);
}
EFI_STATUS write(EFI_FILE* file, UINTN size, void* buffer){
    return file->Write(file, &size, buffer);
}