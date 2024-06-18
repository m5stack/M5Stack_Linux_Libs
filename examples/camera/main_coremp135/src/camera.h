/*
* SPDX-FileCopyrightText: 2024 M5Stack Technology CO LTD
*
* SPDX-License-Identifier: MIT
*/
#ifndef CAMERA_H
#define CAMERA_H

#include <stdint.h>
#include <pthread.h>
#define CONFIG_CAPTURE_WIDTH   320   // Display width
#define CONFIG_CAPTURE_HEIGHT  240   // Display height
#define CONFIG_CAPTURE_FPS     30    //
#define CONFIG_CAPTURE_BUF_CNT 10    //
#define CONFIG_ENCODE_BITRATE  2500  //

#define CONFIG_DEVNAME_LEN    32
#define CONFIG_IOCTL_RETRY    5
#define CONFIG_CAPTURE_DEVICE "/dev/video0"


typedef void (*vCameraFrameGet)(uint8_t* pData, uint32_t Width, uint32_t Height, uint32_t Length);
typedef enum { CAMERA_STATE_CLOSE, CAMERA_SATTE_OPEN, CAMERA_SATTE_CAP, CAMERA_STATE_ERR } CAMERA_STATE_t;

typedef struct {
    void* pStart;
    int Length;
} CAMERA_Buffer_t;

typedef struct Camera_t{
    char DevName[32];
    int DevFd;
    int BufferCnt;
    int capture_fps;
    int is_alloc;
    uint32_t Width;
    uint32_t Height;
    CAMERA_Buffer_t* pCameraBuffer;
    vCameraFrameGet pCallback;
    CAMERA_STATE_t State; /* 0 Not open, 1 Turn on the camera, 2 Start capturing,
                            3 Error */
    pthread_t CaptureThreadId;
    /**
     * Set capture frame callback
     * Return value: 0 for success, -1 for failure
     */
    int (*CameraCaptureCallbackSet)(struct Camera_t*, vCameraFrameGet);
    /**
     * Start capturing
     * Return value: 0 for success, -1 for failure
     */
    int (*CameraCaptureStart)(struct Camera_t*);
    /**
     * Stop capturing
     * Return value: 0 for success, -1 for failure
     */
    int (*CameraCaptureStop)(struct Camera_t*);
}Camera_t;

/**
 * Open the camera
 * @pDevName Device node
 * Return value: NULL for failure
 */
Camera_t* CameraOpen(const char* pDevName);

/**
 * Open the camera from config
 * @pDevName Device node
 * Return value: 0 for success, -1 for failure
 */
int CameraOpenFrom(Camera_t* Camera);

/**
 * Close the camera
 * Return value: 0 for success, -1 for failure
 */
int CameraClose(Camera_t* Camera);

#endif