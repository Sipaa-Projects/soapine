/*
 * @file common/libc/stdio.h
 * @brief C Library: Standard input/output - header
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

#include <stdarg.h>

#define GETCHAR_F10 256
#define GETCHAR_CURSOR_LEFT 257
#define GETCHAR_CURSOR_RIGHT 258
#define GETCHAR_CURSOR_UP 259
#define GETCHAR_CURSOR_DOWN 260
#define GETCHAR_DELETE 261
#define GETCHAR_END 262
#define GETCHAR_HOME 263
#define GETCHAR_PGUP 264
#define GETCHAR_PGDOWN 265
#define GETCHAR_ESC 266
#define GETCHAR_ENTER 267
#define GETCHAR_BACKSPACE 268
#define GETCHAR_TAB 269

void printf(char *format, ...);
void vsnprintf(char *buf, int bufsz, char *format, va_list args);
void snprintf(char *buf, int bufsz, char *format, ...);
int  getchar();
