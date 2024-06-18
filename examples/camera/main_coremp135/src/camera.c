/*
 * SPDX-FileCopyrightText: 2024 M5Stack Technology CO LTD
 *
 * SPDX-License-Identifier: MIT
 */
#include "camera.h"

#include <assert.h>
#include <errno.h>
#include <fcntl.h>
#include <linux/videodev2.h>
#include <sample_log.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/ioctl.h>
#include <sys/mman.h>
#include <unistd.h>

/**
 * Name of storage device
 */
static int DevNameSet(struct Camera_t* Camera, const char* pDevName) {
    int CopyLen = 0;
    CopyLen     = strlen(pDevName);
    if (CopyLen > CONFIG_DEVNAME_LEN - 1) {
        SLOGE("Error: device name length over limit: %d", CopyLen);
        return -1;
    }
    memset(Camera->DevName, 0, CONFIG_DEVNAME_LEN);
    memcpy(Camera->DevName, pDevName, CopyLen);
    return 0;
}

static int CameraCaptureCallbackSet(struct Camera_t* Camera, vCameraFrameGet pCallback) {
    if (Camera->State == CAMERA_SATTE_CAP) {
        SLOGW("Set capture callback failed");
        return -1;
    }
    Camera->pCallback = pCallback;
    return 0;
}

static void* CameraCaptureThread(void* pParam) {
    int Ret          = -1;
    Camera_t* Camera = (Camera_t*)pParam;
    struct v4l2_buffer EnQueueBuf;
    struct v4l2_buffer DeQueueBuf;

    SLOGI("Start capture");

    while (Camera->State == CAMERA_SATTE_CAP) {
        memset(&DeQueueBuf, 0, sizeof(struct v4l2_buffer));
        DeQueueBuf.type   = V4L2_BUF_TYPE_VIDEO_CAPTURE;
        DeQueueBuf.memory = V4L2_MEMORY_MMAP;

        memset(&EnQueueBuf, 0, sizeof(struct v4l2_buffer));
        EnQueueBuf.type   = V4L2_BUF_TYPE_VIDEO_CAPTURE;
        EnQueueBuf.memory = V4L2_MEMORY_MMAP;

        // Retrieve data from the queue
        Ret = ioctl(Camera->DevFd, VIDIOC_DQBUF, &DeQueueBuf);
        if (Ret) {
            perror("Dequeue fail");
            break;
        }
        Camera->pCallback(Camera->pCameraBuffer[DeQueueBuf.index].pStart, Camera->Width, Camera->Height,
                          Camera->pCameraBuffer[DeQueueBuf.index].Length);

        // Re-queue the data
        EnQueueBuf.index = DeQueueBuf.index;

        if (ioctl(Camera->DevFd, VIDIOC_QBUF, &EnQueueBuf)) {
            perror("Enqueue fail");
            break;
        }
    }

    SLOGI("Stop capture");

    return NULL;
}

static int CameraCaptureStart(struct Camera_t* Camera) {
    SLOGI("Start capture thread");
    if (!Camera->pCallback) {
        SLOGW("Capture callback not set, start faild");
        return -1;
    }
    if (Camera->State != CAMERA_SATTE_OPEN) {
        SLOGW("Camera not open, start faild");
        return -1;
    }
    Camera->State = CAMERA_SATTE_CAP;

    pthread_create(&Camera->CaptureThreadId, NULL, CameraCaptureThread, Camera);

    return 0;
}

static int CameraCaptureStop(struct Camera_t* Camera) {
    SLOGI("Stop capture thread");
    Camera->State = CAMERA_SATTE_OPEN;
    pthread_join(Camera->CaptureThreadId, NULL);
    SLOGI("Capture thread stop");
    return 0;
}

int CameraOpenFrom(Camera_t* Camera) {
    int Ret = -1;
    if (Camera == NULL) return -1;
    /* Check whether the camera is already open or in an error state */
    SLOGI("Open camera %s...", Camera->DevName);
    if (Camera->State != CAMERA_STATE_CLOSE) {
        SLOGE("Error: camera was open or meet error, now state is: %d", Camera->State);
        goto ErrorHandle;
    }

    /* Open the camera device */
    if ((Camera->DevFd = open(Camera->DevName, O_RDWR, 0)) == -1) {
        SLOGE("Open camera %s faild.", Camera->DevName);
        goto ErrorHandle;
    }

    SLOGI("Open device successful.");

    /* Set the capture source */
    struct v4l2_input Input;
    Input.index = 0;

    if (ioctl(Camera->DevFd, VIDIOC_S_INPUT, &Input)) {
        perror("VIDIOC_S_INPUT");
    }

    /* Retrieve camera information */
    struct v4l2_capability Cap;

    Ret = ioctl(Camera->DevFd, VIDIOC_QUERYCAP, &Cap);

    if (Ret) {
        perror("Get device info error");
        goto ErrorHandle;
    }

    SLOGI("Get device info successful.");

    SLOGI("Driver name: %s", Cap.driver);
    SLOGI("Card name: %s", Cap.card);
    SLOGI("Bus info: %s", Cap.bus_info);

    struct v4l2_format TvFmt;
    struct v4l2_fmtdesc FmtDesc;
    struct v4l2_streamparm StreamParam;

    memset(&FmtDesc, 0, sizeof(FmtDesc));
    FmtDesc.index = 0;
    FmtDesc.type  = V4L2_BUF_TYPE_VIDEO_CAPTURE;

    /* Enumerate supported frame formats */
    SLOGI("Support format:");
    while (ioctl(Camera->DevFd, VIDIOC_ENUM_FMT, &FmtDesc) == 0) {
        FmtDesc.index++;
        SLOGI("Pixel format = %c%c%c%c", (FmtDesc.pixelformat & 0xFF), ((FmtDesc.pixelformat >> 8) & 0xFF),
              ((FmtDesc.pixelformat >> 16) & 0xFF), (FmtDesc.pixelformat >> 24));
        SLOGI("Description = %s ", FmtDesc.description);
    }

    /* Determine if the device is a device that can capture video */
    if (!(Cap.capabilities & V4L2_BUF_TYPE_VIDEO_CAPTURE)) {
        SLOGE("The Current device is not a video capture device");
        goto ErrorHandle;
    }

    /* Determine if video streaming is supported */
    if (!(Cap.capabilities & V4L2_CAP_STREAMING)) {
        SLOGE("The Current device does not support streaming i/o");
        goto ErrorHandle;
    }

    /* Get video stream information */
    memset(&StreamParam, 0, sizeof(struct v4l2_streamparm));
    StreamParam.type = V4L2_BUF_TYPE_VIDEO_CAPTURE;
    Ret              = ioctl(Camera->DevFd, VIDIOC_G_PARM, &StreamParam);
    if (Ret) {
        perror("Get stream info failed");
        goto ErrorHandle;
    }

    SLOGI("Before setting stream params:");
    SLOGI("Capability: %u", StreamParam.parm.capture.capability);
    SLOGI("Capturemode: %u", StreamParam.parm.capture.capturemode);
    SLOGI("Extendedmode: %u", StreamParam.parm.capture.extendedmode);
    SLOGI("Timeperframe denominator: %u", StreamParam.parm.capture.timeperframe.denominator);
    SLOGI("Timeperframe numerator: %u", StreamParam.parm.capture.timeperframe.numerator);

    /* Set frame rate denominator and numerator */
    StreamParam.parm.capture.timeperframe.denominator = Camera->capture_fps;
    StreamParam.parm.capture.timeperframe.numerator   = 1;

    if (ioctl(Camera->DevFd, VIDIOC_S_PARM, &StreamParam) == -1) {
        perror("Unable to set fps");
        goto ErrorHandle;
    }

    /* Get video stream information */
    Ret = ioctl(Camera->DevFd, VIDIOC_G_PARM, &StreamParam);
    if (Ret) {
        perror("Get stream info failed");
        goto ErrorHandle;
    }

    SLOGI("After setting stream params:");
    SLOGI("Capability: %u", StreamParam.parm.capture.capability);
    SLOGI("Capturemode: %u", StreamParam.parm.capture.capturemode);
    SLOGI("Extendedmode: %u", StreamParam.parm.capture.extendedmode);
    SLOGI("Timeperframe denominator: %u", StreamParam.parm.capture.timeperframe.denominator);
    SLOGI("Timeperframe numerator: %u", StreamParam.parm.capture.timeperframe.numerator);

    /* Set automatic exposure */
    struct v4l2_control Ctrl;
    Ctrl.id = V4L2_CID_EXPOSURE_ABSOLUTE;
    Ret     = ioctl(Camera->DevFd, VIDIOC_G_CTRL, &Ctrl);

    if (Ret) {
        perror("Set exposure failed");
        // goto ErrorHandle;
    }

    /* Set capture format */
    TvFmt.type                = V4L2_BUF_TYPE_VIDEO_CAPTURE;
    TvFmt.fmt.pix.width       = Camera->Width;
    TvFmt.fmt.pix.height      = Camera->Height;
    TvFmt.fmt.pix.pixelformat = V4L2_PIX_FMT_YUYV;
    TvFmt.fmt.pix.field       = V4L2_FIELD_INTERLACED;

    if (ioctl(Camera->DevFd, VIDIOC_S_FMT, &TvFmt) < 0) {
        perror("Unable set VIDIOC_S_FMT");
        goto ErrorHandle;
    }

    if (ioctl(Camera->DevFd, VIDIOC_G_FMT, &TvFmt) < 0) {
        perror("Get VIDIOC_S_FMT error");
        goto ErrorHandle;
    }

    SLOGI("After setting frame params:");
    SLOGI("Width: %d, height: %d", TvFmt.fmt.pix.width, TvFmt.fmt.pix.height);
    SLOGI("Pixel format = %c%c%c%c", (TvFmt.fmt.pix.pixelformat & 0xFF), ((TvFmt.fmt.pix.pixelformat >> 8) & 0xFF),
          ((TvFmt.fmt.pix.pixelformat >> 16) & 0xFF), (TvFmt.fmt.pix.pixelformat >> 24));

    Camera->Width  = TvFmt.fmt.pix.width;
    Camera->Height = TvFmt.fmt.pix.height;

    /* Request V4L2 driver to allocate memory */
    struct v4l2_requestbuffers ReqBuffer;
    memset(&ReqBuffer, 0, sizeof(ReqBuffer));
    ReqBuffer.count  = Camera->BufferCnt;
    ReqBuffer.type   = V4L2_BUF_TYPE_VIDEO_CAPTURE;
    ReqBuffer.memory = V4L2_MEMORY_MMAP;

    if (-1 == ioctl(Camera->DevFd, VIDIOC_REQBUFS, &ReqBuffer)) {
        perror("Fail to ioctl 'VIDIOC_REQBUFS'");
        goto ErrorHandle;
    }

    Camera->BufferCnt = ReqBuffer.count;
    SLOGI("Buffer count: %d", Camera->BufferCnt);

    Camera->pCameraBuffer = (CAMERA_Buffer_t*)calloc(Camera->BufferCnt, sizeof(CAMERA_Buffer_t));
    if (Camera->pCameraBuffer == NULL) {
        SLOGE("Malloc buffer failed");
        goto ErrorHandle;
    }

    /* Map kernel buffer to user space */
    struct v4l2_buffer Buf;
    for (int i = 0; i < Camera->BufferCnt; i++) {
        memset(&Buf, 0, sizeof(Buf));
        Buf.type   = V4L2_BUF_TYPE_VIDEO_CAPTURE;
        Buf.memory = V4L2_MEMORY_MMAP;
        Buf.index  = i;

        if (-1 == ioctl(Camera->DevFd, VIDIOC_QUERYBUF, &Buf)) {
            perror("Fail to ioctl : VIDIOC_QUERYBUF");
            goto ErrorHandle;
        }

        Camera->pCameraBuffer[i].Length = Buf.length;
        Camera->pCameraBuffer[i].pStart =
            mmap(NULL, Buf.length, PROT_READ | PROT_WRITE, MAP_SHARED, Camera->DevFd, Buf.m.offset);

        if (MAP_FAILED == Camera->pCameraBuffer[i].pStart) {
            perror("Fail to mmap");
            goto ErrorHandle;
        }

        if (ioctl(Camera->DevFd, VIDIOC_QBUF, &Buf)) {
            perror("Unable to queue buffer");
            goto ErrorHandle;
        }
    }

    /* Start capturing */
    enum v4l2_buf_type BufType;
    BufType = V4L2_BUF_TYPE_VIDEO_CAPTURE;
    if (ioctl(Camera->DevFd, VIDIOC_STREAMON, &BufType) < 0) {
        perror("VIDIOC_STREAMON error");
        goto ErrorHandle;
    }
    Camera->State                    = CAMERA_SATTE_OPEN;
    Camera->CameraCaptureCallbackSet = CameraCaptureCallbackSet;
    Camera->CameraCaptureStart       = CameraCaptureStart;
    Camera->CameraCaptureStop        = CameraCaptureStop;
    Camera->is_alloc                 = 0;
    return 0;

ErrorHandle:
    SLOGE("Camera open meet error, now handle it");
    close(Camera->DevFd);
    free(Camera->pCameraBuffer);
    return -1;
}

Camera_t* CameraOpen(const char* pDevName) {
    int Ret          = -1;
    Camera_t* Camera = (Camera_t*)malloc(sizeof(Camera_t));
    if (Camera == NULL) return NULL;
    /* Check whether the camera is already open or in an error state */
    SLOGI("Open camera %s...", pDevName);
    if (Camera->State != CAMERA_STATE_CLOSE) {
        SLOGE("Error: camera was open or meet error, now state is: %d", Camera->State);
        goto ErrorHandle;
    }

    /* Open the camera device */
    if ((Camera->DevFd = open(pDevName, O_RDWR, 0)) == -1) {
        SLOGE("Open camera %s faild.", pDevName);
        goto ErrorHandle;
    }

    SLOGI("Open device successful.");

    /* Set the capture source */
    struct v4l2_input Input;
    Input.index = 0;

    if (ioctl(Camera->DevFd, VIDIOC_S_INPUT, &Input)) {
        perror("VIDIOC_S_INPUT");
    }

    /* Retrieve camera information */
    struct v4l2_capability Cap;

    Ret = ioctl(Camera->DevFd, VIDIOC_QUERYCAP, &Cap);

    if (Ret) {
        perror("Get device info error");
        goto ErrorHandle;
    }

    SLOGI("Get device info successful.");

    SLOGI("Driver name: %s", Cap.driver);
    SLOGI("Card name: %s", Cap.card);
    SLOGI("Bus info: %s", Cap.bus_info);

    DevNameSet(Camera, pDevName);

    struct v4l2_format TvFmt;
    struct v4l2_fmtdesc FmtDesc;
    struct v4l2_streamparm StreamParam;

    memset(&FmtDesc, 0, sizeof(FmtDesc));
    FmtDesc.index = 0;
    FmtDesc.type  = V4L2_BUF_TYPE_VIDEO_CAPTURE;

    /* Enumerate supported frame formats */
    SLOGI("Support format:");
    while (ioctl(Camera->DevFd, VIDIOC_ENUM_FMT, &FmtDesc) == 0) {
        FmtDesc.index++;
        SLOGI("Pixel format = %c%c%c%c", (FmtDesc.pixelformat & 0xFF), ((FmtDesc.pixelformat >> 8) & 0xFF),
              ((FmtDesc.pixelformat >> 16) & 0xFF), (FmtDesc.pixelformat >> 24));
        SLOGI("Description = %s ", FmtDesc.description);
    }

    /* Determine if the device is a device that can capture video */
    if (!(Cap.capabilities & V4L2_BUF_TYPE_VIDEO_CAPTURE)) {
        SLOGE("The Current device is not a video capture device");
        goto ErrorHandle;
    }

    /* Determine if video streaming is supported */
    if (!(Cap.capabilities & V4L2_CAP_STREAMING)) {
        SLOGE("The Current device does not support streaming i/o");
        goto ErrorHandle;
    }

    /* Get video stream information */
    memset(&StreamParam, 0, sizeof(struct v4l2_streamparm));
    StreamParam.type = V4L2_BUF_TYPE_VIDEO_CAPTURE;
    Ret              = ioctl(Camera->DevFd, VIDIOC_G_PARM, &StreamParam);
    if (Ret) {
        perror("Get stream info failed");
        goto ErrorHandle;
    }

    SLOGI("Before setting stream params:");
    SLOGI("Capability: %u", StreamParam.parm.capture.capability);
    SLOGI("Capturemode: %u", StreamParam.parm.capture.capturemode);
    SLOGI("Extendedmode: %u", StreamParam.parm.capture.extendedmode);
    SLOGI("Timeperframe denominator: %u", StreamParam.parm.capture.timeperframe.denominator);
    SLOGI("Timeperframe numerator: %u", StreamParam.parm.capture.timeperframe.numerator);

    /* Set frame rate denominator and numerator */
    StreamParam.parm.capture.timeperframe.denominator = CONFIG_CAPTURE_FPS;
    StreamParam.parm.capture.timeperframe.numerator   = 1;

    if (ioctl(Camera->DevFd, VIDIOC_S_PARM, &StreamParam) == -1) {
        perror("Unable to set fps");
        goto ErrorHandle;
    }

    /* Get video stream information */
    Ret = ioctl(Camera->DevFd, VIDIOC_G_PARM, &StreamParam);
    if (Ret) {
        perror("Get stream info failed");
        goto ErrorHandle;
    }

    SLOGI("After setting stream params:");
    SLOGI("Capability: %u", StreamParam.parm.capture.capability);
    SLOGI("Capturemode: %u", StreamParam.parm.capture.capturemode);
    SLOGI("Extendedmode: %u", StreamParam.parm.capture.extendedmode);
    SLOGI("Timeperframe denominator: %u", StreamParam.parm.capture.timeperframe.denominator);
    SLOGI("Timeperframe numerator: %u", StreamParam.parm.capture.timeperframe.numerator);

    /* Set automatic exposure */
    struct v4l2_control Ctrl;
    Ctrl.id = V4L2_CID_EXPOSURE_ABSOLUTE;
    Ret     = ioctl(Camera->DevFd, VIDIOC_G_CTRL, &Ctrl);

    if (Ret) {
        perror("Set exposure failed");
        // goto ErrorHandle;
    }

    /* Set capture format */
    TvFmt.type                = V4L2_BUF_TYPE_VIDEO_CAPTURE;
    TvFmt.fmt.pix.width       = CONFIG_CAPTURE_WIDTH;
    TvFmt.fmt.pix.height      = CONFIG_CAPTURE_HEIGHT;
    TvFmt.fmt.pix.pixelformat = V4L2_PIX_FMT_YUYV;
    TvFmt.fmt.pix.field       = V4L2_FIELD_INTERLACED;

    if (ioctl(Camera->DevFd, VIDIOC_S_FMT, &TvFmt) < 0) {
        perror("Unable set VIDIOC_S_FMT");
        goto ErrorHandle;
    }

    if (ioctl(Camera->DevFd, VIDIOC_G_FMT, &TvFmt) < 0) {
        perror("Get VIDIOC_S_FMT error");
        goto ErrorHandle;
    }

    SLOGI("After setting frame params:");
    SLOGI("Width: %d, height: %d", TvFmt.fmt.pix.width, TvFmt.fmt.pix.height);
    SLOGI("Pixel format = %c%c%c%c", (TvFmt.fmt.pix.pixelformat & 0xFF), ((TvFmt.fmt.pix.pixelformat >> 8) & 0xFF),
          ((TvFmt.fmt.pix.pixelformat >> 16) & 0xFF), (TvFmt.fmt.pix.pixelformat >> 24));

    Camera->Width  = TvFmt.fmt.pix.width;
    Camera->Height = TvFmt.fmt.pix.height;

    /* Request V4L2 driver to allocate memory */
    struct v4l2_requestbuffers ReqBuffer;
    memset(&ReqBuffer, 0, sizeof(ReqBuffer));
    ReqBuffer.count  = CONFIG_CAPTURE_BUF_CNT;
    ReqBuffer.type   = V4L2_BUF_TYPE_VIDEO_CAPTURE;
    ReqBuffer.memory = V4L2_MEMORY_MMAP;

    if (-1 == ioctl(Camera->DevFd, VIDIOC_REQBUFS, &ReqBuffer)) {
        perror("Fail to ioctl 'VIDIOC_REQBUFS'");
        goto ErrorHandle;
    }

    Camera->BufferCnt = ReqBuffer.count;
    SLOGI("Buffer count: %d", Camera->BufferCnt);

    Camera->pCameraBuffer = (CAMERA_Buffer_t*)calloc(Camera->BufferCnt, sizeof(CAMERA_Buffer_t));
    if (Camera->pCameraBuffer == NULL) {
        SLOGE("Malloc buffer failed");
        goto ErrorHandle;
    }

    /* Map kernel buffer to user space */
    struct v4l2_buffer Buf;
    for (int i = 0; i < Camera->BufferCnt; i++) {
        memset(&Buf, 0, sizeof(Buf));
        Buf.type   = V4L2_BUF_TYPE_VIDEO_CAPTURE;
        Buf.memory = V4L2_MEMORY_MMAP;
        Buf.index  = i;

        if (-1 == ioctl(Camera->DevFd, VIDIOC_QUERYBUF, &Buf)) {
            perror("Fail to ioctl : VIDIOC_QUERYBUF");
            goto ErrorHandle;
        }

        Camera->pCameraBuffer[i].Length = Buf.length;
        Camera->pCameraBuffer[i].pStart =
            mmap(NULL, Buf.length, PROT_READ | PROT_WRITE, MAP_SHARED, Camera->DevFd, Buf.m.offset);

        if (MAP_FAILED == Camera->pCameraBuffer[i].pStart) {
            perror("Fail to mmap");
            goto ErrorHandle;
        }

        if (ioctl(Camera->DevFd, VIDIOC_QBUF, &Buf)) {
            perror("Unable to queue buffer");
            goto ErrorHandle;
            ;
        }
    }

    /* Start capturing */
    enum v4l2_buf_type BufType;
    BufType = V4L2_BUF_TYPE_VIDEO_CAPTURE;
    if (ioctl(Camera->DevFd, VIDIOC_STREAMON, &BufType) < 0) {
        perror("VIDIOC_STREAMON error");
        goto ErrorHandle;
    }
    Camera->State                    = CAMERA_SATTE_OPEN;
    Camera->CameraCaptureCallbackSet = CameraCaptureCallbackSet;
    Camera->CameraCaptureStart       = CameraCaptureStart;
    Camera->CameraCaptureStop        = CameraCaptureStop;
    Camera->is_alloc                 = 1;
    return Camera;

ErrorHandle:
    SLOGE("Camera open meet error, now handle it");
    close(Camera->DevFd);
    free(Camera->pCameraBuffer);
    free(Camera);
    return NULL;
}

int CameraClose(Camera_t* Camera) {
    if (Camera == NULL) return -1;
    if (Camera->State == CAMERA_SATTE_OPEN || Camera->State == CAMERA_SATTE_CAP) {
        SLOGI("Close camera %s", Camera->DevName);
    } else {
        SLOGW("Skip close camera progress");
        return 0;
    }

    for (int i = 0; i < CONFIG_CAPTURE_BUF_CNT; i++) {
        munmap(Camera->pCameraBuffer[i].pStart, Camera->pCameraBuffer[i].Length);
        Camera->pCameraBuffer[i].Length = 0;
    }

    free(Camera->pCameraBuffer);
    Camera->pCameraBuffer = NULL;

    int Off = 1;

    if (ioctl(Camera->DevFd, VIDIOC_STREAMOFF, &Off)) {
        perror("Stop camera fail");
        return -1;
    }

    close(Camera->DevFd);
    if (Camera->is_alloc) free(Camera);

    SLOGI("Camera closed");

    return 0;
}