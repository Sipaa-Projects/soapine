#pragma once

#include <stddef.h>

void abort(bool allow_menu, char *msg, ...);
void *malloc(size_t size);
void free(void *ptr);