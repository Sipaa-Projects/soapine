#include "st.h"

st_ctx ctx = {
        .fb_addr = 0,
        .fb_width = 0,
        .fb_height = 0,
        .fb_pitch = 0,
        .fb_bpp = 0,
        .fb_red_mask_size = 0,
        .fb_red_mask_shift = 0,
        .fb_green_mask_size = 0,
        .fb_green_mask_shift = 0,
        .fb_blue_mask_size = 0,
        .fb_blue_mask_shift = 0,

        .cur_x = 0,
        .cur_y = 0,
        .cur_visible = true,

        .color_bg = 0x000000,
        .color_fg = 0xFFFFFF,
        .uc_codepoint = 0,
        .uc_remaining = 0,

        .in_esc = false,
        .esc_type = 0,
        .esc_cur_arg = 0,
        .esc_ctrl_args = {},
};

//===============================Drawing functions===============================

void __st_plot_pixel(st_u32 x, st_u32 y, st_u32 color){
    if(x >= ctx.fb_width || y >= ctx.fb_height){
        return;
    }
    st_u32 transformed_color = 0;
    transformed_color |= (color & 0xFF) << ctx.fb_red_mask_shift;
    transformed_color |= ((color & 0xFF00) >> 8) << ctx.fb_green_mask_shift;
    transformed_color |= ((color & 0xFF0000) >> 16) << ctx.fb_blue_mask_shift;

    *(st_u32*)&((st_u8*)ctx.fb_addr)[y * ctx.fb_pitch + x * (ctx.fb_bpp / 8)] = transformed_color;
}

void __st_plot_glyph(st_u32 x, st_u32 y, st_u16 g, st_u32 color_fg, st_u32 color_bg){
    if(x >= ctx.fb_width/ctx.font_width || y >= ctx.fb_height/ctx.font_height || g >= ctx.font_glyph_count){
        return;
    }

    st_u8 width_bytes = ctx.font_width % 8 == 0 ? ctx.font_width / 8 : ((ctx.font_width / 8) + 1);

    //TODO: optimize this
    st_u8 *glyph = (st_u8*)(ctx.font_glyphs) + g * ctx.font_bytes_per_glyph;
    for(st_u32 i = 0; i < ctx.font_height; i++){
        // if you know how to make it less of a mess, please let me know
        for(st_u32 j = 0; j < ctx.font_width; j++){
            st_bool draw = (glyph[i * width_bytes + j / 8] >> (7 - j % 8)) & 1;
            __st_plot_pixel(x * ctx.font_width + j, y * ctx.font_height + i, draw ? color_fg : color_bg);
        }
    }
}

st_u16 __st_get_glyph(st_u64 c);

void __st_render_cursor(){
    if(ctx.cur_visible){
        for (st_u32 i = 0; i < ctx.font_width; i++) {
            for (st_u32 j = 0; j < ctx.font_height; j++) {
                __st_plot_pixel(ctx.cur_x * ctx.font_width + i, ctx.cur_y * ctx.font_height + j, ctx.color_fg);
            }
        }
    }
}

void __st_delete_cursor(){
    if(ctx.cur_visible){
        for (st_u32 i = 0; i < ctx.font_width; i++) {
            for (st_u32 j = 0; j < ctx.font_height; j++) {
                __st_plot_pixel(ctx.cur_x * ctx.font_width + i, ctx.cur_y * ctx.font_height + j, ctx.color_bg);
            }
        }
    }
}

void __st_clear(){
    for(st_u32 i = 0; i < ctx.fb_width * ctx.fb_height; i++){
        __st_plot_pixel(i % ctx.fb_width, i / ctx.fb_width, ctx.color_bg);
    }
    ctx.cur_x = 0;
    ctx.cur_y = 0;

    __st_render_cursor();
}

void __st_scroll(){
    if(ctx.cur_y >= (ctx.fb_height/ctx.font_height) - 1){
        st_u32 n = ctx.cur_y - ((ctx.fb_height/ctx.font_height) - 1);

        //TODO: readd scrolling
        //ctx.cur_y = (ctx.fb_height/ctx.font_height) - 1;
    }
    __st_render_cursor();
}

//===============================Table functions===============================

st_u16 __st_get_glyph(st_u64 c) {
    if (ctx.font_type == 1 && ctx.font_utbl != NULL) { // PSF1
        st_u16* table = (st_u16*)(ctx.font_utbl);
        st_u16 glyph_index = 0;
        st_u64 uc = 0;
        while ((st_u8*)table <= (st_u8*)ctx.font_addr + ctx.font_size) {
            uc = *table;
            if(uc == 0xffff){
                glyph_index++;
                table++;
                continue;
            } else{
                uc = *table;
            }
            if(uc == c) {
                return glyph_index;
            }
            table++;
        }
        return glyph_index;
    } else if (ctx.font_type == 2 && ctx.font_utbl != NULL) { // PSF2
        st_u8* table = (st_u8*)(ctx.font_utbl);
        st_u16 glyph_index = 0;
        st_u64 uc = 0;
        while (table <= (st_u8*)ctx.font_addr + ctx.font_size) {
            uc = *table;
            if(uc == 0xff){
                glyph_index++;
                table++;
                continue;
            } else if (*table & 128) {
                if((uc & 32) == 0 ) {
                    uc = ((table[0] & 0x1F)<<6)+(table[1] & 0x3F);
                    table++;
                } else
                if((uc & 16) == 0 ) {
                    uc = ((((table[0] & 0xF)<<6)+(table[1] & 0x3F))<<6)+(table[2] & 0x3F);
                    table+=2;
                } else if((uc & 8) == 0 ) {
                    uc = ((((((table[0] & 0x7)<<6)+(table[1] & 0x3F))<<6)+(table[2] & 0x3F))<<6)+(table[3] & 0x3F);
                    table+=3;
                } else {
                    uc = 0;
                }
            }
            if(uc == c) {
                return glyph_index;
            }
            table++;
        }
        return 0;
    }
    return (st_u32)c;
}
//================================Escape parsing================================
void __st_sgr(){

    //TODO:support more than just true color fg and bg
    if(ctx.esc_ctrl_args[0] == 0){
        ctx.color_fg = 0xAAAAAA;
        ctx.color_bg = 0x000000;
    } else if(ctx.esc_ctrl_args[0] == 1){
        ctx.color_fg = 0xFFFFFF;
    } else if(ctx.esc_ctrl_args[0] == 2){
        ctx.color_fg = 0xAAAAAA;
    } else if (ctx.esc_ctrl_args[0] == 7){
        st_u32 tmp = ctx.color_fg;
        ctx.color_fg = ctx.color_bg;
        ctx.color_bg = tmp;
    //Ignore 3-4 cuz they are impossible
    } else if(ctx.esc_ctrl_args[0] == 38 && ctx.esc_ctrl_args[1] == 2){
        st_u8 r = ctx.esc_ctrl_args[2];
        st_u8 g = ctx.esc_ctrl_args[3];
        st_u8 b = ctx.esc_ctrl_args[4];
        ctx.color_fg = (b << 16) | (g << 8) | r;
    } else if(ctx.esc_ctrl_args[0] == 48 && ctx.esc_ctrl_args[1] == 2){
        st_u8 r = ctx.esc_ctrl_args[2];
        st_u8 g = ctx.esc_ctrl_args[3];
        st_u8 b = ctx.esc_ctrl_args[4];
        ctx.color_bg = (b << 16) | (g << 8) | r;
    }
}

void __st_clear_ctrl_args(){
    for(st_u32 i = 0; i < ANSI_MAX_ARGS - 1; i++){
        ctx.esc_ctrl_args[i] = 0;
    }
}

void __st_eparse( char c){
    switch (c) {
        case '[':
            ctx.esc_type = 1;
            return;
        case 'c':
            __st_clear();
            break;
    }
    ctx.in_esc = false;
}
void __st_eparse_ctrl(char c){
    if (c >= '0' && c <= '9') {
        ctx.esc_ctrl_args[ctx.esc_cur_arg] *= 10;
        ctx.esc_ctrl_args[ctx.esc_cur_arg] += c - '0';
        return;
    }

    switch (c) {
        case ';':
            if (ctx.esc_cur_arg == ANSI_MAX_ARGS - 1) {
                return;
            }
            ctx.esc_cur_arg++;
            return;
        case 'E':
            __st_delete_cursor();
            ctx.cur_y += ctx.esc_ctrl_args[0];
            ctx.cur_x = 0;
            __st_scroll();
            break;
        case 'F':
            __st_delete_cursor();
            ctx.cur_y -= ctx.esc_ctrl_args[0];
            ctx.cur_x = 0;
            __st_scroll();
            break;
        case 'G':
            __st_delete_cursor();
            ctx.cur_x = ctx.esc_ctrl_args[0];
            __st_render_cursor();
            break;
        case 'f': // same as 'H'
        case 'H':
            __st_delete_cursor();
            ctx.cur_x = ctx.esc_ctrl_args[0];
            ctx.cur_y = ctx.esc_ctrl_args[1];
            __st_render_cursor();
            break;
        case 'J':
            if(ctx.esc_ctrl_args[0] != 2){
                break;
            }
            __st_clear();
            break;
        case 'm':
            __st_sgr();
            break;
    }

    ctx.in_esc = false;
}

void (*__st_eparse_tbl[])(char) = {
    __st_eparse,
    __st_eparse_ctrl
};


//===============================Public functions===============================

void st_write(st_u8 c){

    if (ctx.in_esc) {
        __st_eparse_tbl[ctx.esc_type](c);
        return;
    }

    //This lump of code stitches UNICODE characters together from UTF-8 multy-byte characters.
    if (ctx.uc_remaining > 0) {
        if ((c & 0xc0) != 0x80) {
            ctx.uc_remaining = 0;
        } else {
            ctx.uc_remaining--;
            ctx.uc_codepoint |= (st_u64)(c & 0x3f) << (6 * ctx.uc_remaining);
            if (ctx.uc_remaining != 0) {
                return;
            }
        }
    } else {
        ctx.uc_codepoint = c;
    }

    //This lump of code detects UTF-8 multy-byte characters, and sets the ctx.uc_remaining variable to how many bytes are remaining.
    if (c >= 0xc0 && c <= 0xf7) {
        if (c >= 0xc0 && c <= 0xdf) {
            ctx.uc_remaining = 1;
            ctx.uc_codepoint = (st_u64)(c & 0x1f) << 6;
        } else if (c >= 0xe0 && c <= 0xef) {
            ctx.uc_remaining = 2;
            ctx.uc_codepoint = (st_u64)(c & 0x0f) << (6 * 2);
        } else if (c >= 0xf0 && c <= 0xf7) {
            ctx.uc_remaining = 3;
            ctx.uc_codepoint = (st_u64)(c & 0x07) << (6 * 3);
        }
        return;
    }

    switch(ctx.uc_codepoint){
        case 0x00:
        case 0x7f:
            return; //ignore
        case '\v':  //treat vertical tabs as a newline
        case '\n':
            newline:
            __st_delete_cursor();
            ctx.cur_x = 0;
            ctx.cur_y++;
            __st_scroll();
            break;
        case '\b':
            ctx.cur_x--;
            break;
        case '\r':
            __st_delete_cursor();
            ctx.cur_x = 0;
            break;
        case '\f':
            __st_clear();
            break;
        case '\t':
            __st_delete_cursor();
            ctx.cur_x += ctx.cur_x % ST_TAB_WIDTH;
            break;
        case '\e':
            ctx.in_esc = true;
            ctx.esc_cur_arg = 0;
            __st_clear_ctrl_args();
            ctx.esc_type = 0;
            break;
        default:
            __st_plot_glyph(ctx.cur_x, ctx.cur_y, __st_get_glyph(ctx.uc_codepoint), ctx.color_fg, ctx.color_bg);

            ctx.cur_x++;
            if(ctx.cur_x >= ctx.fb_width/ctx.font_width) goto newline;
            __st_render_cursor();
            break;
    }
}

void st_init(st_u32* fb_addr, st_u32 fb_width, st_u32 fb_height, st_u32 fb_pitch,
               st_u32 fb_bpp, st_u8 fb_red_mask_size, st_u8 fb_red_mask_shift, 
               st_u8 fb_green_mask_size, st_u8 fb_green_mask_shift, st_u8 fb_blue_mask_size, st_u8 fb_blue_mask_shift,

               st_u32* font_data, st_u32 font_size){
    ctx.fb_addr = fb_addr,
    ctx.fb_width = fb_width,
    ctx.fb_height = fb_height,
    ctx.fb_pitch = fb_pitch,
    ctx.fb_bpp = fb_bpp,
    ctx.fb_red_mask_size = fb_red_mask_size,
    ctx.fb_red_mask_shift = fb_red_mask_shift,
    ctx.fb_green_mask_size = fb_green_mask_size,
    ctx.fb_green_mask_shift = fb_green_mask_shift,
    ctx.fb_blue_mask_size = fb_blue_mask_size,
    ctx.fb_blue_mask_shift = fb_blue_mask_shift,

    //interpret the font data
    ctx.font_addr = font_data;
    ctx.font_size = font_size;

    __st_clear_ctrl_args();

    if((*(st_u16*)font_data) == PSF1_MAGIC){
        ctx.font_type = 1;
        ctx.font_glyphs = (st_u32*)((st_u8*)font_data + sizeof(psf1_header));
        ctx.font_glyph_count = ((psf1_header*)font_data)->mode & PSF1_MODE_512 ? 512 : 256;
        ctx.font_width = 8;
        ctx.font_height = ctx.font_bytes_per_glyph = ((psf1_header*)font_data)->char_size;
        ctx.font_utbl = ((psf1_header*)font_data)->mode & (PSF1_MODE_HASTABLE | PSF1_MODE_SEQ) ?
            (st_u32*)((st_u8*)ctx.font_glyphs + ctx.font_bytes_per_glyph * ctx.font_glyph_count) 
            : NULL;
    }else if((*(st_u32*)font_data) == PSF2_MAGIC){
        ctx.font_type = 2;
        ctx.font_glyphs = (st_u32*)((st_u8*)font_data + ((psf2_header*)font_data)->header_size);
        ctx.font_glyph_count = ((psf2_header*)font_data)->glyph_count;
        ctx.font_width = ((psf2_header*)font_data)->font_width;
        ctx.font_height = ((psf2_header*)font_data)->font_height;
        ctx.font_bytes_per_glyph = ((ctx.font_width / 8) + 1) * ctx.font_height;
        ctx.font_utbl = ((psf2_header*)font_data)->flags & (PSF2_FLAG_UC) ?
            (st_u32*)((st_u8*)ctx.font_glyphs + ctx.font_bytes_per_glyph * ctx.font_glyph_count) 
            : NULL;
    }

    __st_render_cursor();
}