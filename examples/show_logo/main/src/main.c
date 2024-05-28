/*
* SPDX-FileCopyrightText: 2024 M5Stack Technology CO LTD
*
* SPDX-License-Identifier: MIT
*/
#include <stdio.h>
#include <string.h>
#include "framebuffer/fbtools.h"

#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"


int main(int argc, char **argv)
{
    FBDEV fbdev;
    memset(&fbdev, 0, sizeof(FBDEV));
    strcpy(fbdev.dev, "/dev/fb1");
    if (fb_open(&fbdev) == 0)
    {
        printf("open frame buffer error/n");
        return -1;
    }
    int w = fbdev.fb_var.xres;
    int h = fbdev.fb_var.yres;
    int color = fbdev.fb_var.bits_per_pixel;

    uint16_t *piex = fbdev.fb_mem;

    int iw, ih, n;
    unsigned char *idata = stbi_load("/usr/local/m5stack/logo.jpg", &iw, &ih, &n, 0);
    for (int i = 0; i < iw * ih; i++)
    {
        piex[i] = ((((idata[3 * i]) & 0xF8) << 8) | (((idata[3 * i + 1]) & 0xFC) << 3) | ((idata[3 * i + 2]) >> 3));
    }
    stbi_image_free(idata);
    
    usleep(60 * 1000);

    fb_close(&fbdev);
    return 0;
}
