#pragma once

#include <stdbool.h>

typedef struct module {
    char *comment;
    char *path;
    char *cmdline;
} module_t;

typedef struct menu_entry {
    char name[512];
    char protocol[64];
    char resolution[14]; // #####x#####x##
    char image_path[2048];
    char cmdline[4096];
    struct menu_entry *next;
} menu_entry_t;

typedef struct config_declaration {
    char name[2048];
    char value[2048];
} config_declaration_t;

extern bool config_parsing_errors_present;

#define CONFIG_NOT_FOUND (char *)0xDEADBEEF

char *config_get_value(char *name);
menu_entry_t *config_get_menu_root();
int config_get_mentry_count();
void config_init(char *cfg);