/*
 * @file common/lib/term.h
 * @brief Terminal manager - header
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
#include <stdbool.h>
 
typedef enum terminal_type {
    FALLBACK, // VGA text mode or UEFI console
    FBTERM,   // Simpterm
    NONE,     // No terminal
} terminal_type_t;

typedef enum terminal_colorreset_type {
    ALL,      // FG & BG
    BGONLY,   // Bg only
    FGONLY,     // Fg only
} terminal_colorreset_type_t;

extern terminal_type_t term_type;

#define TERM_BLACK   0x00
#define TERM_BLUE    0x01
#define TERM_GREEN   0x02
#define TERM_CYAN            (TERM_BLUE | TERM_GREEN)
#define TERM_RED     0x04
#define TERM_MAGENTA         (TERM_BLUE | TERM_RED)
#define TERM_BROWN           (TERM_GREEN | TERM_RED)
#define TERM_LIGHTGRAY       (TERM_BLUE | TERM_GREEN | TERM_RED)
#define TERM_BRIGHT  0x08
#define TERM_DARKGRAY        (TERM_BRIGHT)
#define TERM_LIGHTBLUE       (TERM_BLUE | TERM_BRIGHT)
#define TERM_LIGHTGREEN      (TERM_GREEN | TERM_BRIGHT)
#define TERM_LIGHTCYAN       (TERM_CYAN | TERM_BRIGHT)
#define TERM_LIGHTRED        (TERM_RED | TERM_BRIGHT)
#define TERM_LIGHTMAGENTA    (TERM_MAGENTA | TERM_BRIGHT)
#define TERM_YELLOW          (TERM_BROWN | TERM_BRIGHT)
#define TERM_WHITE           (TERM_BLUE | TERM_GREEN | TERM_RED | TERM_BRIGHT)

void term_init(terminal_type_t preferred);
void term_set_cursor(bool enabled, int x, int y);
void term_set_color(bool is_fg, int r, int g, int b);
void term_reset_color(terminal_colorreset_type_t t);
void term_write(char *str);
void term_clear();