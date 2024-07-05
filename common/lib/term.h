#pragma once
 
typedef enum terminal_type {
    FALLBACK, // VGA text mode or UEFI console
    FBTERM,   // Simpterm
    NONE,     // No terminal
} terminal_type_t;

extern terminal_type_t term_type;

void term_init(terminal_type_t preferred);
void term_set_cursor_pos(int x, int y);
void term_set_color(char r, char g, char b); // Char goes up to 255, RGB values too.
void term_write(char *str);