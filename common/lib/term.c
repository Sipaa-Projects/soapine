#include <drv/fb.h>
#include <lib/libc.h>
#include <lib/term.h>
#include <lib/term_font.h>
#include <lib/st.h>

terminal_type_t term_type;

void dummy() {}

void term_write(char *str)
{
    if (term_type == FALLBACK)
        dummy(); //AsciiPrint(str); // UEFI uses Unicode characters.
    else if (term_type == FBTERM)
        for (int i = 0; i < strlen(str); i++)
            st_write(str[i]);
}

void term_init(terminal_type_t preferred,EFI_SYSTEM_TABLE *SystemTable)
{
    switch (preferred)
    {
    case FBTERM:
        if (!fb_init(-1, -1, -1)) { // Let the firmware automatically choose a mode for us.
            //Print(L"lib/term: fb_init fail. Using fallback.\n");
            term_init(FALLBACK, SystemTable); // Use a fallback terminal if it fails.
            return;
        }
        fb_clear();

        framebuffer_t *fb = fb_get();

        st_init((uint32_t *)fb->address, fb->width, fb->height, fb->pitch, (uint32_t)fb->bpp,
                fb->red_mask_size,   fb->red_mask_shift,
                fb->green_mask_size, fb->green_mask_shift,
                fb->blue_mask_size,  fb->blue_mask_shift,
                (uint32_t *)&font, sizeof(font));
        break;
    
    default:
        break;
    }

    term_type = preferred;
}
