/*
 * SPDX-FileCopyrightText: 2024 M5Stack Technology CO LTD
 *
 * SPDX-License-Identifier: MIT
 */
#include <fcntl.h>
#include <linux/fb.h>
#include <linux/input.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/ioctl.h>
#include <sys/mman.h>
#include <unistd.h>

#include "framebuffer/fbtools.h"
#include "imlib.h"
#include "qrencode.h"

int main(int argc, char **argv) {
    FBDEV fbdev;
    memset(&fbdev, 0, sizeof(FBDEV));
    system("echo 0 > /sys/class/graphics/fbcon/cursor_blink");
    {  // Get LCD device name
        char dev_number[8] = {0};
        char dev_name[64]  = {0};
        FILE *fb_config    = fopen("/proc/fb", "r");
        if (fb_config == NULL) {
            perror("Failed to open /proc/fb");
            return 1;
        }

        while (fscanf(fb_config, "%7s %63s", dev_number, dev_name) == 2) {
            if (strstr(dev_name, "fb_ili") != NULL) {
                strcpy(fbdev.dev, "/dev/fb");
                strcat(fbdev.dev, dev_number);
                break;
            }
            memset(dev_number, 0, sizeof(dev_number));
        }
        fclose(fb_config);
    }

    if (fb_open(&fbdev) == 0) {
        printf("open frame buffer error/n");
        return -1;
    }
    int w     = fbdev.fb_var.xres;
    int h     = fbdev.fb_var.yres;
    int color = fbdev.fb_var.bits_per_pixel;

    // Creating an Imlib image using framebuffer video memory
    image_t *img = imlib_image_create(w, h, PIXFORMAT_RGB565, w * h * 2, (void *)fbdev.fb_mem, 0);
    fb_memset((void *)(fbdev.fb_mem + fbdev.fb_mem_offset), 0, fbdev.fb_fix.smem_len);
    
    const char *data = "Hello, World!";
    QRcode *qr = QRcode_encodeString(data, 0, QR_ECLEVEL_L, QR_MODE_8, 1);

    if (!qr) {
        fprintf(stderr, "Failed to generate QR code\n");
        goto project_end;
    }

    for (int y = 0; y < qr->width; y++) {
        for (int x = 0; x < qr->width; x++) {
            if(qr->data[(y * qr->width + x)] & 1)
                imlib_draw_rectangle(img,  10 * x, 10 * y, 10, 10, COLOR_R8_G8_B8_TO_RGB565(0xff, 0xff, 0xff), 1, 1);
            else
                imlib_draw_rectangle(img,  10 * x, 10 * y, 10, 10, COLOR_R8_G8_B8_TO_RGB565(0, 0, 0), 1, 1);
        }
    }
    
project_end:
    imlib_image_destroy(&img);
    fb_close(&fbdev);
    return 0;
}
