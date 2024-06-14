/* camera.h */
// Copyright (c) 2024 M5Stack Technology CO LTD
#ifndef CAMERA_H
#define CAMERA_H

#include <stdint.h>

typedef void (*vCameraFrameProcess)(uint8_t* pData, uint32_t Width,
                                    uint32_t Height, uint32_t Length);

/**
 * Open the camera
 * @pDevName Device node
 * Return value: 0 for success, -1 for failure
 */
int CameraOpen(const char* pDevName);

/**
 * Close the camera
 * Return value: 0 for success, -1 for failure
 */
int CameraClose(void);

/**
 * Set capture frame callback
 * Return value: 0 for success, -1 for failure
 */
int CameraCaptureCallbackSet(vCameraFrameProcess pCallBack);

/**
 * Start capturing
 * Return value: 0 for success, -1 for failure
 */
int CameraCaptureStart(void);

/**
 * Stop capturing
 * Return value: 0 for success, -1 for failure
 */
int CameraCaptureStop(void);

#endif