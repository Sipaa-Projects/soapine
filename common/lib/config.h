/*
 * @file common/lib/config.h
 * @brief Configuration file parser - header
 * 
 * Copyright (C) 2024-present Sipaa Projects & the Soapine contributors
 *
 * This piece of software is released under the terms of the MIT License
 */

#pragma once

#include <stdbool.h>

typedef enum config_valtype {
    INVALID, // An invalid type: "Hello
    STRING, // A string literal: "Hello World"
    BOOL, // Booleans: true & false
    DECIMAL, // A decimal: 0, 1, 2, 3, 4...
    HEXADECIMAL, // A hexadecimal: 0x1, 0xA, 0xDEADBEEF...
    HEX_COLOR, // A hexadecimal color in RRGGBB format : #FFFFFF (white), #000000 (black), #0000FF (blue)
} config_valtype_t;

typedef struct module {
    char *comment;
    char *path;
    char *cmdline;
} module_t;

typedef struct menu_entry {
    char name[512];
    char protocol[64];
    char comment[2048];
    char resolution[14]; // #####x#####x##
    char image_path[2048];
    char cmdline[4096];
    struct menu_entry *next;
} menu_entry_t;

typedef struct config_declaration {
    char name[2048];
    char *value_str;
    unsigned int value;
    config_valtype_t type;
} config_declaration_t;

extern bool config_parsing_errors_present;

#define CONFIG_NOT_FOUND (char *)0xDEADBEEF

config_declaration_t *config_get_value(char *name);
menu_entry_t *config_get_menu_root();
int config_get_mentry_count();
void config_init(char *cfg);