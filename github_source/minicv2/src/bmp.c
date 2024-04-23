/*
 * This file is part of the OpenMV project.
 *
 * Copyright (c) 2013-2021 Ibrahim Abdelkader <iabdalkader@openmv.io>
 * Copyright (c) 2013-2021 Kwabena W. Agyeman <kwagyeman@openmv.io>
 *
 * This work is licensed under the MIT license, see the file LICENSE for details.
 *
 * BMP reader/writer.
 */

#include "imlib.h"
#if defined(IMLIB_ENABLE_IMAGE_FILE_IO)

#include <stdlib.h>
// #include "py/obj.h"
// #include "py/runtime.h"

#include "xalloc.h"
#include "ff_wrapper.h"

// This function inits the geometry values of an image (opens file).
bool bmp_read_geometry(FIL *fp, image_t *img, const char *path, bmp_read_settings_t *rs)
{
    int is_ps_image = 0;
    read_byte_expect(fp, 'B');
    read_byte_expect(fp, 'M');

    uint32_t file_size;
    read_long(fp, &file_size);
    read_word_ignore(fp);
    read_word_ignore(fp);

    uint32_t header_size;
    read_long(fp, &header_size);
    if (file_size <= header_size) ff_file_corrupted(fp);

    uint32_t data_size = file_size - header_size;
    if (data_size % 4)
    {
        uint32_t tmp_uint32 = data_size - 2;
        if(tmp_uint32 % 4 == 0)
        {
            is_ps_image = 1;
        }
        else
        {
            ff_file_corrupted(fp);
        }
    }

    uint32_t header_type;
    read_long(fp, &header_type);
    if ((header_type != 40) // BITMAPINFOHEADER
    && (header_type != 52) // BITMAPV2INFOHEADER
    && (header_type != 56) // BITMAPV3INFOHEADER
    && (header_type != 108) // BITMAPV4HEADER
    && (header_type != 124)) ff_unsupported_format(fp); // BITMAPV5HEADER
    read_long(fp, (uint32_t*) &rs->bmp_w);
    read_long(fp, (uint32_t*) &rs->bmp_h);
    if ((rs->bmp_w == 0) || (rs->bmp_h == 0)) ff_file_corrupted(fp);
    img->w = abs(rs->bmp_w);
    img->h = abs(rs->bmp_h);

    read_word_expect(fp, 1);
    read_word(fp, &rs->bmp_bpp);
    if ((rs->bmp_bpp != 8) && (rs->bmp_bpp != 16) && (rs->bmp_bpp != 24)) {
        ff_unsupported_format(fp);
    }
    // img->pixfmt = (rs->bmp_bpp == 8) ? PIXFORMAT_GRAYSCALE : PIXFORMAT_RGB565;
    switch (rs->bmp_bpp)
    {
    case 8:
        img->pixfmt = PIXFORMAT_GRAYSCALE;
        break;
    case 16:
        img->pixfmt = PIXFORMAT_RGB565;
        break;
    case 24:
        img->pixfmt = PIXFORMAT_RGB888;
        break;
    }
    read_long(fp, &rs->bmp_fmt);
    if ((rs->bmp_fmt != 0) && (rs->bmp_fmt != 3)) ff_unsupported_format(fp);

    read_long_ignore(fp);
    read_long_ignore(fp);
    read_long_ignore(fp);
    read_long_ignore(fp);
    read_long_ignore(fp);

    if (rs->bmp_bpp == 8) {
        if (rs->bmp_fmt != 0) ff_unsupported_format(fp);
        if (header_type >= 52) { // Skip past the remaining BITMAPV2INFOHEADER bytes.
            for (int i = 0; i < 3; i++) read_long_ignore(fp);
        }
        if (header_type >= 56) { // Skip past the remaining BITMAPV3INFOHEADER bytes.
            for (int i = 0; i < 1; i++) read_long_ignore(fp);
        }
        if (header_type >= 108) { // Skip past the remaining BITMAPV4HEADER bytes.
            for (int i = 0; i < 13; i++) read_long_ignore(fp);
        }
        if (header_type >= 124) { // Skip past the remaining BITMAPV5HEADER bytes.
            for (int i = 0; i < 4; i++) read_long_ignore(fp);
        }
        // Color Table (1024 bytes)
        for (int i = 0; i < 256; i++) {
            read_long_expect(fp, ((i) << 16) | ((i) << 8) | i);
        }
    } else if (rs->bmp_bpp == 16) {
        if (rs->bmp_fmt != 3) ff_unsupported_format(fp);
        // Bit Masks (12 bytes)
        read_long_expect(fp, 0x1F << 11);
        read_long_expect(fp, 0x3F << 5);
        read_long_expect(fp, 0x1F);
        if (header_type >= 56) { // Skip past the remaining BITMAPV3INFOHEADER bytes.
            for (int i = 0; i < 1; i++) read_long_ignore(fp);
        }
        if (header_type >= 108) { // Skip past the remaining BITMAPV4HEADER bytes.
            for (int i = 0; i < 13; i++) read_long_ignore(fp);
        }
        if (header_type >= 124) { // Skip past the remaining BITMAPV5HEADER bytes.
            for (int i = 0; i < 4; i++) read_long_ignore(fp);
        }
    } else if (rs->bmp_bpp == 24) {
        if (rs->bmp_fmt == 3) {
            // Bit Masks (12 bytes)
            read_long_expect(fp, 0xFF << 16);
            read_long_expect(fp, 0xFF << 8);
            read_long_expect(fp, 0xFF);
        } else if (header_type >= 52) { // Skip past the remaining BITMAPV2INFOHEADER bytes.
            for (int i = 0; i < 3; i++) read_long_ignore(fp);
        }
        if (header_type >= 56) { // Skip past the remaining BITMAPV3INFOHEADER bytes.
            for (int i = 0; i < 1; i++) read_long_ignore(fp);
        }
        if (header_type >= 108) { // Skip past the remaining BITMAPV4HEADER bytes.
            for (int i = 0; i < 13; i++) read_long_ignore(fp);
        }
        if (header_type >= 124) { // Skip past the remaining BITMAPV5HEADER bytes.
            for (int i = 0; i < 4; i++) read_long_ignore(fp);
        }
    }

    rs->bmp_row_bytes = (((img->w * rs->bmp_bpp) + 31) / 32) * 4;
    if(is_ps_image)
    {
        if ((data_size - 2) != (rs->bmp_row_bytes * img->h)) ff_file_corrupted(fp);
    }
    else
    {
        if (data_size != (rs->bmp_row_bytes * img->h)) ff_file_corrupted(fp);
    }
    return (rs->bmp_h >= 0);
}

// This function reads the pixel values of an image.
void bmp_read_pixels(FIL *fp, image_t *img, int n_lines, bmp_read_settings_t *rs)
{
    if (rs->bmp_bpp == 8) {
        if ((rs->bmp_h < 0) && (rs->bmp_w >= 0) && (img->w == rs->bmp_row_bytes)) {
            read_data(fp, img->pixels, n_lines * img->w);
        } else {
            for (int i = 0; i < n_lines; i++) {
                for (int j = 0; j < rs->bmp_row_bytes; j++) {
                    uint8_t pixel;
                    read_byte(fp, &pixel);
                    if (j < img->w) {
                        if (rs->bmp_h < 0) { // vertical flip (BMP file perspective)
                            if (rs->bmp_w < 0) { // horizontal flip (BMP file perspective)
                                IM_SET_GS_PIXEL(img, (img->w-j-1), i, pixel);
                            } else {
                                IM_SET_GS_PIXEL(img, j, i, pixel);
                            }
                        } else {
                            if (rs->bmp_w < 0) {
                                IM_SET_GS_PIXEL(img, (img->w-j-1), (img->h-i-1), pixel);
                            } else {
                                IM_SET_GS_PIXEL(img, j, (img->h-i-1), pixel);
                            }
                        }
                    }
                }
            }
        }
    } else if (rs->bmp_bpp == 16) {
        for (int i = 0; i < n_lines; i++) {
            for (int j = 0, jj = rs->bmp_row_bytes / 2; j < jj; j++) {
                uint16_t pixel;
                read_word(fp, &pixel);
                if (j < img->w) {
                    if (rs->bmp_h < 0) { // vertical flip (BMP file perspective)
                        if (rs->bmp_w < 0) { // horizontal flip (BMP file perspective)
                            IM_SET_RGB565_PIXEL(img, (img->w-j-1), i, pixel);
                        } else {
                            IM_SET_RGB565_PIXEL(img, j, i, pixel);
                        }
                    } else {
                        if (rs->bmp_w < 0) {
                            IM_SET_RGB565_PIXEL(img, (img->w-j-1), (img->h-i-1), pixel);
                        } else {
                            IM_SET_RGB565_PIXEL(img, j, (img->h-i-1), pixel);
                        }
                    }
                }
            }
        }
    } else if (rs->bmp_bpp == 24) {
        for (int i = 0; i < n_lines; i++) {
            for (int j = 0, jj = rs->bmp_row_bytes / 3; j < jj; j++) {
                uint8_t b, g, r;
                read_byte(fp, &b);
                read_byte(fp, &g);
                read_byte(fp, &r);
                int pixel = COLOR_R8_G8_B8_TO_RGB888(r, g, b);
                if (j < img->w) {
                    if (rs->bmp_h < 0) { // vertical flip
                        if (rs->bmp_w < 0) { // horizontal flip
                            IM_SET_RGB888_PIXEL(img, (img->w-j-1), i, pixel);
                        } else {
                            IM_SET_RGB888_PIXEL(img, j, i, pixel);
                        }
                    } else {
                        if (rs->bmp_w < 0) {
                            IM_SET_RGB888_PIXEL(img, (img->w-j-1), (img->h-i-1), pixel);
                        } else {
                            IM_SET_RGB888_PIXEL(img, j, (img->h-i-1), pixel);
                        }
                    }
                }
            }
            for (int j = 0, jj = rs->bmp_row_bytes % 3; j < jj; j++) {
                read_byte_ignore(fp);
            }
        }
    }
}

void bmp_read(image_t *img, const char *path)
{
    FIL fp;
    bmp_read_settings_t rs;
    file_read_open(&fp, path);
    // file_buffer_on(&fp);
    bmp_read_geometry(&fp, img, path, &rs);
    if(img->is_data_alloc){
        img->pixels = xrealloc(img->pixels, img->w * img->h * img->bpp);
    }else{
        img->pixels = xalloc(img->w * img->h * img->bpp);
        img->is_data_alloc = true;
    }
    bmp_read_pixels(&fp, img, img->h, &rs);
    // file_buffer_off(&fp);
    file_close(&fp);
}

void bmp_write_subimg(image_t *img, const char *path, rectangle_t *r)
{
    rectangle_t rect;
    if(!r)
    {
        rectangle_t tmp_rect;
        rectangle_init(&tmp_rect, 0, 0, img->w, img->h);
        if (!rectangle_subimg(img, &tmp_rect, &rect)) {
            ERR_PRINT("OSError: No intersection!");
        }
    }
    else
    {
        if (!rectangle_subimg(img, r, &rect)) {
            ERR_PRINT("OSError: No intersection!");
        }
    }
    FIL fp;
    file_write_open(&fp, path);
    // file_buffer_on(&fp);
    if (IM_IS_GS(img)) {
        const int row_bytes = (((rect.w * 8) + 31) / 32) * 4;
        const int data_size = (row_bytes * rect.h);
        const int waste = (row_bytes / sizeof(uint8_t)) - rect.w;
        // File Header (14 bytes)
        write_byte(&fp, 'B');
        write_byte(&fp, 'M');
        write_long(&fp, 14 + 40 + 1024 + data_size);
        write_word(&fp, 0);
        write_word(&fp, 0);
        write_long(&fp, 14 + 40 + 1024);
        // Info Header (40 bytes)
        write_long(&fp, 40);
        write_long(&fp, rect.w);
        write_long(&fp, -rect.h); // store the image flipped (correctly)
        write_word(&fp, 1);
        write_word(&fp, 8);
        write_long(&fp, 0);
        write_long(&fp, data_size);
        write_long(&fp, 0);
        write_long(&fp, 0);
        write_long(&fp, 0);
        write_long(&fp, 0);
        // Color Table (1024 bytes)
        for (int i = 0; i < 256; i++) {
            write_long(&fp, ((i) << 16) | ((i) << 8) | i);
        }
        if ((rect.x == 0) && (rect.w == img->w) && (img->w == row_bytes)) {
            write_data(&fp, // Super Fast - Zoom, Zoom!
                       img->pixels + (rect.y * img->w),
                       rect.w * rect.h);
        } else {
            for (int i = 0; i < rect.h; i++) {
                write_data(&fp, img->pixels+((rect.y+i)*img->w)+rect.x, rect.w);
                for (int j = 0; j < waste; j++) {
                    write_byte(&fp, 0);
                }
            }
        }
    } else if(IM_IS_RGB565(img)) {
        const int row_bytes = (((rect.w * 16) + 31) / 32) * 4;
        const int data_size = (row_bytes * rect.h);
        const int waste = (row_bytes / sizeof(uint16_t)) - rect.w;
        // File Header (14 bytes)
        write_byte(&fp, 'B');                                               //表示文件类型
        write_byte(&fp, 'M');
        write_long(&fp, 14 + 40 + 12 + data_size);                          //表示文件的大小
        write_word(&fp, 0);                                                 //保留位，必须设置为0；
        write_word(&fp, 0);                                                 //保留位，必须设置为0；
        write_long(&fp, 14 + 40 + 12);                                      //4字节的偏移，表示从文件头到位图数据的偏移
        // Info Header (40 bytes)
        write_long(&fp, 40);                                                //信息头的大小，即40；
        write_long(&fp, rect.w);                                            //以像素为单位说明图像的宽度；
        write_long(&fp, -rect.h); // store the image flipped (correctly)    //以像素为单位说明图像的高度，同时如果为正，说明位图倒立
        write_word(&fp, 1);                                                 //为目标设备说明颜色平面数，总被设置为1；
        write_word(&fp, 16);                                                //说明比特数/像素数，值有1、2、4、8、16、24、32；
        write_long(&fp, 3);                                                 //说明图像的压缩类型，最常用的就是0（BI_RGB），表示不压缩；
        write_long(&fp, data_size);                                         //说明位图数据的大小，当用BI_RGB格式时，可以设置为0
        write_long(&fp, 0);                                                 //表示水平分辨率，单位是像素/米，有符号整数；
        write_long(&fp, 0);                                                 //表示垂直分辨率，单位是像素/米，有符号整数；
        write_long(&fp, 0);                                                 //说明位图使用的调色板中的颜色索引数，为0说明使用所有；
        write_long(&fp, 0);                                                 //说明对图像显示有重要影响的颜色索引数，为0说明都重要；
        // Bit Masks (12 bytes)
        write_long(&fp, 0x1F << 11);                                        //调色板
        write_long(&fp, 0x3F << 5);
        write_long(&fp, 0x1F);
        for (int i = 0; i < rect.h; i++) {
            for (int j = 0; j < rect.w; j++) {
                write_word(&fp, IM_GET_RGB565_PIXEL(img, (rect.x + j), (rect.y + i)));
            }
            for (int j = 0; j < waste; j++) {
                write_word(&fp, 0);
            }
        }
    }else if(IM_IS_RGB888(img)) {
        const int row_bytes = (((rect.w * 24) + 31) / 32) * 4;
        const int data_size = (row_bytes * rect.h);
        const int waste = row_bytes - rect.w * 24;
        // File Header (14 bytes)
        write_byte(&fp, 'B');              //0x42
        write_byte(&fp, 'M');               //0x4d
        write_long(&fp, 14 + 40 + data_size);
        write_word(&fp, 0);
        write_word(&fp, 0);
        write_long(&fp, 14 + 40);
        // Info Header (40 bytes)
        write_long(&fp, 40);
        write_long(&fp, rect.w);
        write_long(&fp, -rect.h); // store the image flipped (correctly)
        write_word(&fp, 1);
        write_word(&fp, 24);
        write_long(&fp, 0);         //说明图像的压缩类型，最常用的就是0（BI_RGB），表示不压缩；
        write_long(&fp, data_size);
        write_long(&fp, 0);
        write_long(&fp, 0);
        write_long(&fp, 0);
        write_long(&fp, 0);
        for (int i = 0; i < rect.h; i++) {
            for (int j = 0; j < rect.w; j++) {
                pixel24_t pixel_tmp = IM_GET_RGB888_PIXEL_(img, (rect.x + j), (rect.y + i));
                int tmp_r = pixel_tmp.red;
                pixel_tmp.red = pixel_tmp.blue;
                pixel_tmp.blue = tmp_r;
                write_data(&fp, &pixel_tmp, 3);
            }
            for (int j = 0; j < waste; j++) {
                write_byte(&fp, 0);
            }
        }
    }
    // file_buffer_off(&fp);

    file_close(&fp);
}
#endif //IMLIB_ENABLE_IMAGE_FILE_IO
