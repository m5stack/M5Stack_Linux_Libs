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

int main(int argc, char **argv) {
    FBDEV fbdev;
    memset(&fbdev, 0, sizeof(FBDEV));

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

    char strs[100];
    sprintf(strs, "hello_world!");
    // Writing characters in the framebuffer
    imlib_draw_string(img, 75, 40, strs, COLOR_R8_G8_B8_TO_RGB565(0xff, 0xff, 0xff), 2, 0, 0, 1, 0, 0, 0, 0, 0, 0);

    int fb_fd, input_fd;
    struct fb_var_screeninfo vinfo;
    struct input_event event;
    char *fbp;
    input_fd = open("/dev/input/by-path/platform-4c004000.i2c-event", O_RDONLY);
    if (input_fd == -1) {
        perror("Error: cannot open input device");
        munmap(fbp, vinfo.yres_virtual * vinfo.xres_virtual * vinfo.bits_per_pixel / 8);
        close(fb_fd);
        exit(4);
    }

    while (1) {
        read(input_fd, &event, sizeof(struct input_event));
        if (event.type == EV_ABS && event.code == ABS_X) {
            printf("X : %d\n", event.value);
            memset(strs, 0, 100);
            sprintf(strs, "X : %d\n", event.value);
            imlib_draw_rectangle(img, 75, 110, 320, 40, COLOR_R8_G8_B8_TO_RGB565(0, 0, 0), 0, 1);
            imlib_draw_string(img, 75, 110, strs, COLOR_R8_G8_B8_TO_RGB565(0xff, 0xff, 0xff), 2, 0, 0, 1, 0, 0, 0, 0, 0,
                              0);
        }
        if (event.type == EV_ABS && event.code == ABS_Y) {
            printf("Y : %d\n", event.value);
            memset(strs, 0, 100);
            sprintf(strs, "Y : %d\n", event.value);

            imlib_draw_rectangle(img, 75, 180, 320, 40, COLOR_R8_G8_B8_TO_RGB565(0, 0, 0), 0, 1);
            imlib_draw_string(img, 75, 180, strs, COLOR_R8_G8_B8_TO_RGB565(0xff, 0xff, 0xff), 2, 0, 0, 1, 0, 0, 0, 0, 0,
                              0);
        }
    }

    imlib_image_destroy(&img);
    fb_close(&fbdev);
    return 0;
}
