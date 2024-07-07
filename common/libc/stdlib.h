#pragma once

#include <stdbool.h>
#include <stddef.h>

void abort(bool allow_menu, char *msg, ...);
void *malloc(size_t size);
void free(void *ptr);
int atoi(char *str);
void usleep(int us);
void sleep(int s);