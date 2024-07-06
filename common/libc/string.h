#pragma once

#include <efi.h>
#include <efilib.h>

int mbtowc (wchar_t * __pwc, const char *s, size_t n);
int wctomb (char *s, wchar_t u);
size_t mbstowcs (wchar_t *__pwcs, const char *__s, size_t __n);
size_t wcstombs (char *__s, const wchar_t *__pwcs, size_t __n);
int strlen(char *str);