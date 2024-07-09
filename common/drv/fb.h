/*
 * @file common/drv/fb.h
 * @brief Stage 3 GOP framebuffer driver - header
 * 
 * Copyright (C) 2024-present Sipaa Projects & the Soapine contributors
 *
 * This piece of software is released under the terms of the MIT License
 */

#pragma once

#include <efi.h>
#include <stdbool.h>

typedef struct framebuffer
{
    uint64_t address;
    uint32_t width;
    uint32_t height;
    uint32_t pitch;
    uint16_t bpp;
    uint8_t  red_mask_size;
    uint8_t  red_mask_shift;
    uint8_t  green_mask_size;
    uint8_t  green_mask_shift;
    uint8_t  blue_mask_size;
    uint8_t  blue_mask_shift;
} framebuffer_t;

framebuffer_t *fb_get();
void fb_clear();
uint32_t fb_getpixel(int x, int y);
void fb_plotpixel(int x, int y, uint32_t color);
bool fb_init(uint32_t preferred_width, uint32_t preferred_height, uint16_t preferred_bpp);