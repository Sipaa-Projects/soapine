#include <drv/fb.h>
#include <lib/config.h>
#include <lib/term.h>
#include <bootloader-info.h>
#include <console.h>
#include <menu.h>
#include <stdio.h>
#include <stdlib.h>

#define HEADERBAR_HEIGHT 3
#define BRANDING_R 82 
#define BRANDING_G 255 
#define BRANDING_B 204

#define KEY_R 123 
#define KEY_G 82 
#define KEY_B 255

#define ENTRY_R 0 
#define ENTRY_G 255 
#define ENTRY_B 255

#define ENTRY_FG_R 0 
#define ENTRY_FG_G 157 
#define ENTRY_FG_B 255

#define ENTRY_BG_R 0 
#define ENTRY_BG_G 105 
#define ENTRY_BG_B 143

menu_entry_t *mentry_selected;
int mentry_count = 0;

void render_headerbar() {
    framebuffer_t *fb = fb_get();

    term_set_color(false, 16, 16, 16);
    for (int i = 0; i < (fb->width / 8); i++)
        for (int j = 0; j < HEADERBAR_HEIGHT; j++) {
            term_set_cursor(false, i, j);
            term_write(" ");
        }

    term_set_cursor(false, HEADERBAR_HEIGHT / 2 + 1, HEADERBAR_HEIGHT / 2);

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
}

void render_entries(int selected_entry) {
    menu_entry_t *centry = config_get_menu_root();
    int i = 0;

    while (centry) {
        term_set_cursor(false, 4, HEADERBAR_HEIGHT + 2 + i);
        
        if (selected_entry == i)
        {
            term_set_color(true, ENTRY_FG_R, ENTRY_FG_G, ENTRY_FG_B);
            term_set_color(false, ENTRY_BG_R, ENTRY_BG_G, ENTRY_BG_B);
        }

        printf("-> %s", centry->name);

        term_reset_color(ALL);

        i++;
        centry = centry->next;
    }
}

__attribute__((noreturn)) void menu(bool first_run)
{
    mentry_count = config_get_mentry_count();
    int selected_entry = 0;

render_full:
    term_clear();
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