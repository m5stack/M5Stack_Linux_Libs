/*
 * SPDX-FileCopyrightText: 2024 M5Stack Technology CO LTD
 *
 * SPDX-License-Identifier: MIT
 */
#include <sample_log.h>
#include <signal.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

#include "linux_v4l2/linux_camera.h"
#include "framebuffer/fbtools.h"
#include "imlib.h"

#define CONFIG_DISPLAY_DEV "/dev/fb1"  // lcd

FBDEV fbdev;
image_t* img;
image_t* fb_img;
camera_t* cam = NULL;
int asdasd    = 0;
void DisplayCallback(uint8_t* pData, uint32_t Width, uint32_t Height, uint32_t Length, void* ctx)
{
    SLOGI("Get img Width Width Length %d %d %d %d\n", Width, Height, Length, asdasd++);
    img->data = pData;
    imlib_pixfmt_to(fb_img, img, NULL);
    {
        static int fcnt = 0, fps;
        fcnt++;
        static struct timespec ts1, ts2;
        clock_gettime(CLOCK_MONOTONIC, &ts2);
        if ((ts2.tv_sec * 1000 + ts2.tv_nsec / 1000000) - (ts1.tv_sec * 1000 + ts1.tv_nsec / 1000000) >= 1000) {
            // printf("FPS:%d\n", fps);
            fps  = fcnt;
            ts1  = ts2;
            fcnt = 0;
        }
        char strs[100];
        sprintf(strs, "FPS:%d", fps);
        imlib_draw_string(fb_img, 10, 10, strs, COLOR_R8_G8_B8_TO_RGB565(0xff, 0, 0), 2, 0, 0, 1, 0, 0, 0, 0, 0, 0);
    }
}

void SignalHandle(int SignalNumber)
{
    printf("Now clean resource\n");
    cam->camera_capture_stop(cam);
    camera_close(cam);
    cam = NULL;

    imlib_image_destroy(&img);
    imlib_image_destroy(&fb_img);
    fb_close(&fbdev);
}

int main(int Argc, char* pArgv[])
{
    int Ret = -1;
    int w = 320, h = 240;
    signal(SIGINT, SignalHandle);

    memset(&fbdev, 0, sizeof(FBDEV));
    strcpy(fbdev.dev, CONFIG_DISPLAY_DEV);

    if (fb_open(&fbdev) == false) {
        printf("open frame buffer error/n");
        return -1;
    }

    fb_img = imlib_image_create(w, h, PIXFORMAT_RGB565, w * h * 2, (void*)fbdev.fb_mem, 0);

    img = imlib_image_create(w, h, PIXFORMAT_YUV422, w * h * 2, NULL, 0);
    cam = camera_open("/dev/video0", 320, 240, 30);
    if (cam == NULL) {
        printf("Camera open failed \n");
        return -1;
    }

    cam->camera_capture_callback_set(cam, DisplayCallback);
    cam->camera_capture_start(cam);

    char KeyValue = getchar();
    printf("You press [%c] button, now stop capture\n", KeyValue);
    SignalHandle(0);
    return 0;
}