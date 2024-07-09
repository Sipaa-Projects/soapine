/*
 * @file common/libc/stdlib.h
 * @brief C Library: Standard library - header
 * 
 * Copyright (C) 2024-present Sipaa Projects & the Soapine contributors
 *
 * This piece of software is released under the terms of the MIT License
 */

#pragma once

#include <stdbool.h>
#include <stddef.h>

void abort(bool allow_menu, char *msg, ...);
void *malloc(size_t size);
void free(void *ptr);
int atoi(char *str);
void usleep(int us);
void sleep(int s);