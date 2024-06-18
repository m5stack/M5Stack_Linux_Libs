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

#include "camera.h"
#include "framebuffer/fbtools.h"
#include "imlib.h"

#define CONFIG_DISPLAY_DEV "/dev/fb1"  // lcd

FBDEV fbdev;
image_t* img;
image_t* fb_img;
Camera_t* cam = NULL;

void DisplayCallback(uint8_t* pData, uint32_t Width, uint32_t Height, uint32_t Length) {
    // SLOGI("Get img Width Width Length %d %d %d\n", Width, Height, Length);
    img->data = pData;
    imlib_pixfmt_to(fb_img, img, NULL);
}

void SignalHandle(int SignalNumber) {
    printf("Now clean resource\n");
    cam->CameraCaptureStop(cam);
    CameraClose(cam);
    cam = NULL;

    imlib_image_destroy(&img);
    imlib_image_destroy(&fb_img);
    fb_close(&fbdev);
}

int main(int Argc, char* pArgv[]) {
    int Ret = -1;
    int w = CONFIG_CAPTURE_WIDTH, h = CONFIG_CAPTURE_HEIGHT;
    signal(SIGINT, SignalHandle);

    memset(&fbdev, 0, sizeof(FBDEV));
    strcpy(fbdev.dev, CONFIG_DISPLAY_DEV);

    if (fb_open(&fbdev) == false) {
        printf("open frame buffer error/n");
        return -1;
    }

    fb_img = imlib_image_create(w, h, PIXFORMAT_RGB565, w * h * 2, (void*)fbdev.fb_mem, 0);

    img = imlib_image_create(w, h, PIXFORMAT_YUV422, w * h * 2, NULL, 0);
    cam = CameraOpen(CONFIG_CAPTURE_DEVICE);
    if (cam == NULL) {
        printf("Camera open failed \n");
        return -1;
    }

    cam->CameraCaptureCallbackSet(cam, DisplayCallback);
    cam->CameraCaptureStart(cam);

    char KeyValue = getchar();
    printf("You press [%c] button, now stop capture\n", KeyValue);
    SignalHandle(0);
    return 0;
}