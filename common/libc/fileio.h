#ifndef __FILEIO_H__
#define __FILEIO_H__

#include <efi.h>
#include <efilib.h>


EFI_STATUS init_fio(EFI_HANDLE handle);

EFI_FILE* open(EFI_FILE* directory, char* path);
EFI_STATUS close(EFI_FILE* file);

UINTN get_size(EFI_FILE* file);

EFI_STATUS read(EFI_FILE* file, UINTN size, void* buffer);
EFI_STATUS write(EFI_FILE* file, UINTN size, void* buffer);

#endif // __FILEIO_H__