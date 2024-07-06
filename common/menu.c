#include <drv/fb.h>
#include <lib/term.h>
#include <bootloader-info.h>
#include <menu.h>
#include <stdio.h>

#define HEADERBAR_HEIGHT 3

void render_headerbar() {
    framebuffer_t *fb = fb_get();

    term_set_color(false, 16, 16, 16);
    for (int i = 0; i < (fb->width / 8); i++)
        for (int j = 0; j < HEADERBAR_HEIGHT; j++) {
            term_set_cursor(false, i, j);
            term_write(" ");
        }

    term_set_cursor(false, HEADERBAR_HEIGHT / 2 + 1, HEADERBAR_HEIGHT / 2);
    term_set_color(true, 0, 206, 255);

    printf("Soapine %d", SOAPINE_VERSION_YEAR);
}

__attribute__((noreturn)) void menu(bool first_run)
{
    render_headerbar();

    while (1)
        ;;
}