/*
 * @file common/drv/fb.c
 * @brief Stage 3 GOP framebuffer driver - code
 * 
 * Copyright (C) 2024-present Sipaa Projects & the Soapine contributors
 *
 * This piece of software is released under the terms of the MIT License
 */

#include <drv/fb.h>

#include <efi.h>
#include <efilib.h>

framebuffer_t fb0;

EFI_GRAPHICS_OUTPUT_PROTOCOL *gop;
EFI_STATUS gop_status;
EFI_GUID gop_guid = EFI_GRAPHICS_OUTPUT_PROTOCOL_GUID;
EFI_GRAPHICS_OUTPUT_MODE_INFORMATION *currentMode;
UINTN SizeOfInfo, numModes, nativeMode;

framebuffer_t *fb_get() { return &fb0; }

void fb_clear() {
    for (int i = 0; i < gop->Mode->FrameBufferSize; i++)
        ((uint32_t *)gop->Mode->FrameBufferBase)[i] = 0;
}

uint32_t fb_getpixel(int x, int y) {
    return *((uint32_t*)(gop->Mode->FrameBufferBase + 4 * gop->Mode->Info->PixelsPerScanLine * y + 4 * x));
}

void fb_plotpixel(int x, int y, uint32_t color) {
   *((uint32_t*)(gop->Mode->FrameBufferBase + 4 * gop->Mode->Info->PixelsPerScanLine * y + 4 * x)) = color;
}

bool fb_init(uint32_t preferred_width,
             uint32_t preferred_height,
             uint16_t preferred_bpp)
{
    gop_status = uefi_call_wrapper(gBS->LocateProtocol, 3, &gop_guid, NULL, (void**)&gop);
    if (EFI_ERROR(gop_status)) {
        Print(L"drv/fb: gBS.LocateProtocol EFI_STATUS: %d\n", gop_status);
        return false;
    }

    gop_status = uefi_call_wrapper(gop->QueryMode, 4, gop, gop->Mode==NULL?0:gop->Mode->Mode, &SizeOfInfo, &currentMode);
    // this is needed to get the current video mode
    if (gop_status == EFI_NOT_STARTED)
        gop_status = uefi_call_wrapper(gop->SetMode, 2, gop, 0);
    
    if(EFI_ERROR(gop_status)) {
        Print(L"drv/fb: gop.QueryMode EFI_STATUS: %d\n", gop_status);
        return false;
    } else {
        nativeMode = gop->Mode->Mode;
        numModes = gop->Mode->MaxMode;
    }

    if (   preferred_width  == -1
        && preferred_height == -1
        && preferred_bpp    == -1)
        goto goodMode;

    if (   preferred_width  == gop->Mode->Info->HorizontalResolution
        && preferred_height == gop->Mode->Info->VerticalResolution
        && preferred_bpp    == 32)
        goto goodMode;

    for (int i = 0; i < numModes; i++) {
        EFI_GRAPHICS_OUTPUT_MODE_INFORMATION *mode;
        gop_status = uefi_call_wrapper(gop->QueryMode, 4, gop, i, &SizeOfInfo, &mode);
        
        if (   preferred_width  == mode->HorizontalResolution
            && preferred_height == mode->VerticalResolution
            && preferred_bpp    == 32)
        {
            gop_status = uefi_call_wrapper(gop->SetMode, 2, gop, i);
            if(EFI_ERROR(gop_status)) {
                Print(L"drv/fb: gop.SetMode EFI_STATUS: %d\n", gop_status);
                return false;
            }
            goto goodMode;
        }
    }

goodMode:
    switch (gop->Mode->Info->PixelFormat) {
        case PixelBlueGreenRedReserved8BitPerColor:
            fb0.bpp = 32;
            fb0.red_mask_size = 8;
            fb0.red_mask_shift = 16;
            fb0.green_mask_size = 8;
            fb0.green_mask_shift = 8;
            fb0.blue_mask_size = 8;
            fb0.blue_mask_shift = 0;
            break;
        case PixelRedGreenBlueReserved8BitPerColor:
            fb0.bpp = 32;
            fb0.red_mask_size = 8;
            fb0.red_mask_shift = 0;
            fb0.green_mask_size = 8;
            fb0.green_mask_shift = 8;
            fb0.blue_mask_size = 8;
            fb0.blue_mask_shift = 16;
            break;
    }

    fb0.address = gop->Mode->FrameBufferBase;
    fb0.width   = gop->Mode->Info->HorizontalResolution;
    fb0.height  = gop->Mode->Info->VerticalResolution;
    fb0.pitch   = gop->Mode->Info->PixelsPerScanLine * (fb0.bpp / 8);

    return true;
}