/*
 * @file common/menu.c
 * @brief Menu applet - code
 * 
 * Copyright (C) 2024-present Sipaa Projects & the Soapine contributors
 *
 * This piece of software is released under the terms of the MIT License
 */

#include <drv/fb.h>
#include <lib/config.h>
#include <lib/term.h>
#include <bootloader-info.h>
#include <console.h>
#include <menu.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define BRANDING_R 82 
#define BRANDING_G 255 
#define BRANDING_B 204

#define KEY_R 123 
#define KEY_G 82 
#define KEY_B 255

#define ENTRY_R 0 
#define ENTRY_G 255 
#define ENTRY_B 255

#define ENTRY_FG_R 255
#define ENTRY_FG_G 157 
#define ENTRY_FG_B 0

#define ENTRY_BG_R 0 
#define ENTRY_BG_G 0 
#define ENTRY_BG_B 0

menu_entry_t *mentry_selected;
int mentry_count = 0;
term_dimensions_t td;

char *prefix = ">";
char *suffix = "<";

int headerbar_height = 3;

void render_headerbar() {

    if (term_type == FBTERM)
    {
        headerbar_height = 3;

        term_set_color(false, 16, 16, 16);
        for (int i = 0; i < td.width; i++)
            for (int j = 0; j < headerbar_height; j++) {
                term_set_cursor(false, i, j);
                term_write(" ");
            }

        term_set_cursor(false, headerbar_height / 2 + 1, headerbar_height / 2);
    } else {
        headerbar_height = 2;

        term_set_color(false, 16, 16, 16);
        for (int i = 0; i < td.width; i++) {
            term_set_cursor(false, i, 1);
            term_write("-");
        }

        term_set_cursor(false, 0, 0);
    }

    term_set_color(true, BRANDING_R, BRANDING_G, BRANDING_B);
    printf("Soapine %s", SOAPINE_VERSION);
    term_reset_color(FGONLY);

    // Branding - help separator
    printf(" - ");

    // ARROWS
    term_set_color(true, KEY_R, KEY_G, KEY_B);
    printf("ARROWS", SOAPINE_VERSION);
    term_reset_color(FGONLY);

    printf(": Navigate, ");

    // ENTER
    term_set_color(true, KEY_R, KEY_G, KEY_B);
    printf("ENTER", SOAPINE_VERSION);
    term_reset_color(FGONLY);

    printf(": Boot, ");

    // C
    term_set_color(true, KEY_R, KEY_G, KEY_B);
    printf("C", SOAPINE_VERSION);
    term_reset_color(FGONLY);

    printf(": Console\n");

    term_reset_color(ALL);
}

void render_entries(int selected_entry) {
    menu_entry_t *centry = config_get_menu_root();
    int i = 0;

    while (centry) {
        for (int i2 = 0; i2 < td.width; i2++)
        {
            term_set_cursor(false, i2, headerbar_height + 2 + i);
            term_write(" ");
        }

        term_set_cursor(false, 4, headerbar_height + 2 + i);
        if (selected_entry == i)
        {
            term_set_color(true, ENTRY_FG_R, ENTRY_FG_G, ENTRY_FG_B);
            term_set_color(false, ENTRY_BG_R, ENTRY_BG_G, ENTRY_BG_B);

            if (!strncmp(centry->comment, "\0", 1))
                printf("%s %s %s", prefix, centry->name, suffix);
            else {
                printf("%s %s (%s) %s", prefix, centry->name, centry->comment, suffix);
            }
        } else {
            printf("  %s  ", centry->name);
        }

        term_reset_color(ALL);

        i++;
        centry = centry->next;
    }
}

__attribute__((noreturn)) void menu(bool first_run)
{
    mentry_count = config_get_mentry_count();
    int selected_entry = 0;

    config_declaration_t *config_prefix = config_get_value("INTERFACE_SELECTED_PREFIX");
    if (config_prefix != CONFIG_NOT_FOUND && config_prefix->type == STRING)
        prefix = config_prefix->value_str;

    config_declaration_t *config_suffix = config_get_value("INTERFACE_SELECTED_SUFFIX");
    if (config_suffix != CONFIG_NOT_FOUND && config_suffix->type == STRING)
        suffix = config_suffix->value_str;
    
render_full:
    td = term_get_dimensions();
    
    term_clear();
    term_reset_color(ALL);
    render_headerbar();

render:
    render_entries(selected_entry);
    
    while (1)
    { 
        int c = getchar();
        //printf("%d", c);
        //printf("| Char: %c, Code: %d | ", c, c);
        switch ((int)c)
        {
            case GETCHAR_CURSOR_UP:
                if (selected_entry == 0)
                {
                    selected_entry = mentry_count - 1;
                    goto render;
                }
                selected_entry--;
                goto render;
            case GETCHAR_CURSOR_DOWN:
                if (selected_entry ==  mentry_count - 1)
                {
                    selected_entry = 0;
                    goto render;
                }
                selected_entry++;
                goto render;
            case GETCHAR_CURSOR_RIGHT:
            case GETCHAR_ENTER:
            case ' ':
                abort(true, "Booting isn't supported\n");
                break;
            case 'C':
            case 'c':
                console(1);
                goto render_full;
        }
    }
}