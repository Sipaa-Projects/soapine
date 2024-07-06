#pragma once

#include <stdarg.h>

void printf(char *format, ...);
void vsnprintf(char *buf, int bufsz, char *format, va_list args);
char getchar();