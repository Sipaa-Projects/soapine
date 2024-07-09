/*
 * @file common/libc/stdio.h
 * @brief C Library: Standard input/output - header
 * 
 * Copyright (C) 2024-present Sipaa Projects & the Soapine contributors
 *
 * This piece of software is released under the terms of the MIT License
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
