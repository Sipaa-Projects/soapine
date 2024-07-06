#include <drv/fb.h>
#include <lib/term.h>
#include <bootloader-info.h>
#include <menu.h>
#include <stdio.h>
#include <libc/fileio.h>

#define HEADERBAR_HEIGHT 3
#define BRANDING_R 82 
#define BRANDING_G 255 
#define BRANDING_B 204

#define KEY_R 123 
#define KEY_G 82 
#define KEY_B 255

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

__attribute__((noreturn)) void menu(bool first_run)
{
    render_headerbar();

    term_set_cursor(true, 2, 2 + HEADERBAR_HEIGHT);
    while (1)
    {

    }
}