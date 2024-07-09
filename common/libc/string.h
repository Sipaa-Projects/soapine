/*
 * @file common/libc/string.h
 * @brief C Library: String management library - header
 * 
 * Copyright (C) 2024 Sipaa Projects and the Soapine contributors
 *
 * This file is part of the Soapine bootloader
 * 
 * Soapine is free software: you can redistribute it and/or modify it
 * under the terms of the GNU General Public License as published by the Free
 * Software Foundation, either version 3 of the License, or (at your option)
 * any later version.
 *
 * Soapine is distributed in the hope that it will be useful, but WITHOUT
 * ANY WARRANTY; without even the implied warranty of  MERCHANTABILITY or
 * FITNESS FOR A PARTICULAR PURPOSE. See the GNU General Public License for
 * more details.
 *
 * You should have received a copy of the GNU General Public License along with
 * Soapine.  If not, see <http://www.gnu.org/licenses/>.
 */

#pragma once

#include <efi.h>
#include <efilib.h>

int mbtowc (wchar_t * __pwc, const char *s, size_t n);
int wctomb (char *s, wchar_t u);
size_t mbstowcs (wchar_t *__pwcs, const char *__s, size_t __n);
size_t wcstombs (char *__s, const wchar_t *__pwcs, size_t __n);
int strlen(char *str);
int strncmp(const char *s1, const char *s2, size_t n);
int strcmp(const char *s1, const char *s2);
char *strncpy(char *dest, const char *src, size_t n);
char *strcpy(char *dest, const char *src);
char *strtok(char *str, const char *delim);
char *strchr(const char *str, int c);
char *strdup(char *str);
void _memset(void *ptr, int v, size_t s);
long strtol(const char *str, char **endptr, int base) ;

#define memset(ptr, v, s) _memset(ptr, v, s);