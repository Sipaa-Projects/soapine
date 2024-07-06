#include <drv/fb.h>
#include <lib/term.h>
#include <lib/term_font.h>
#include <lib/st.h>
#include <efi.h>
#include <efilib.h>
#include <stdio.h>
#include <string.h>

#define BG_R 6
#define BG_G 6
#define BG_B 6

#define FG_R 255
#define FG_G 255
#define FG_B 255

terminal_type_t term_type;

void dummy() {}

extern void _st_clear();

void term_clear() {
    if (term_type == FALLBACK) {
        gST->ConOut->ClearScreen(gST->ConOut);
    }
    else if (term_type == FBTERM)
        _st_clear();
}

void term_write(char *str)
{
    if (term_type == FALLBACK) {
        wchar_t wc[2048];
        mbstowcs(wc, str, strlen(str));
        Print(wc); // UEFI uses Unicode characters.
    }
    else if (term_type == FBTERM)
        for (int i = 0; i < strlen(str); i++)
            st_write(str[i]);
}

void term_init(terminal_type_t preferred)
{
    switch (preferred)
    {
    case FBTERM:
        if (!fb_init(-1, -1, -1)) { // Let the firmware automatically choose a mode for us.
            //Print(L"lib/term: fb_init fail. Using fallback.\n");
            term_init(FALLBACK); // Use a fallback terminal if it fails.
            return;
        }
        fb_clear();

        framebuffer_t *fb = fb_get();

        st_init((uint32_t *)fb->address, fb->width, fb->height, fb->pitch, (uint32_t)fb->bpp,
                fb->red_mask_size,   fb->red_mask_shift,
                fb->green_mask_size, fb->green_mask_shift,
                fb->blue_mask_size,  fb->blue_mask_shift,
                (uint32_t *)&font, sizeof(font));
        ctx.color_bg = (BG_B << 16) | (BG_G << 8) | BG_R;
        ctx.color_fg = (FG_B << 16) | (FG_G << 8) | FG_R;
        break;
    
    default:
        break;
    }

    term_type = preferred;
}

void term_set_cursor(bool enabled, int x, int y) {
    if (term_type == FBTERM) {
        ctx.cur_visible = enabled;
        ctx.cur_x = x;
        ctx.cur_y = y;
    } else if (term_type == FALLBACK) {
        uefi_call_wrapper(gST->ConOut->EnableCursor, 2, gST->ConOut, enabled);
        uefi_call_wrapper(gST->ConOut->SetCursorPosition, 3, gST->ConOut, x, y);
    }
}

void term_reset_color(terminal_colorreset_type_t t) {
    if (term_type == FBTERM) {
        if (t == FGONLY) {
            ctx.color_fg = (FG_B << 16) | (FG_G << 8) | FG_R;
        } else if (t == BGONLY) {
            ctx.color_bg = (BG_B << 16) | (BG_G << 8) | BG_R;
        } else {
            ctx.color_bg = (BG_B << 16) | (BG_G << 8) | BG_R;
            ctx.color_bg = (BG_B << 16) | (BG_G << 8) | BG_R;
        }
    } else if (term_type == FALLBACK) {
        if (t == FGONLY) {
            uefi_call_wrapper(gST->ConOut->SetAttribute, 2, gST->ConOut, EFI_WHITE);
        } else if (t == BGONLY) {
            uefi_call_wrapper(gST->ConOut->SetAttribute, 2, gST->ConOut, EFI_BACKGROUND_BLACK);
        } else {
            uefi_call_wrapper(gST->ConOut->SetAttribute, 2, gST->ConOut, EFI_BACKGROUND_BLACK);
            uefi_call_wrapper(gST->ConOut->SetAttribute, 2, gST->ConOut, EFI_WHITE);
        }
    }
}

void term_set_color(bool is_fg, int r, int g, int b) {
    if (term_type == FBTERM)
    {
        if (is_fg)
            ctx.color_fg = (b << 16) | (g << 8) | r;
        else
            ctx.color_bg = (b << 16) | (g << 8) | r;
    }
}