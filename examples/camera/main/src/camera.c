/* camera.c */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include <fcntl.h>
#include <unistd.h>
#include <string.h>
#include <sys/ioctl.h>
#include <sys/mman.h>
#include <assert.h>
#include <linux/videodev2.h>
#include <pthread.h>
 
#include "config.h"
#include "camera.h"
 
static int Sign3 = 1;
 
#define IOCTL_VIDEO(fd, req, value) ioctl(fd, req, value)
 
typedef struct{  
    void* pStart;
    int Length;
} sBufferType;
 
typedef enum {
    CAMERA_STATE_CLOSE,
    CAMERA_SATTE_OPEN,
    CAMERA_SATTE_CAP,
    CAMERA_STATE_ERR
} eCAMERA_STATE;
 
static struct {
    char        DevName[CONFIG_DEVNAME_LEN];
    int         DevFd;
    int         BufferCnt;
    uint32_t    Width;
    uint32_t    Height;
    sBufferType* pCameraBuffer;
    vCameraFrameProcess pCallback;
    eCAMERA_STATE State;                            /* 0 未打开, 1打开摄像头, 2开始捕捉, 3错误 */
    pthread_t   CaptureThreadId;
} sCameraPrivateData;
 
 
// static int xioctl(int Fd, int IOCTL_X, void *pArg)
// {
//  int Ret = 0;
//  int Tries = CONFIG_IOCTL_RETRY;
//  do {
//      Ret = IOCTL_VIDEO(Fd, IOCTL_X, pArg);
//  } while(Ret && Tries-- && ((errno == EINTR) || (errno == EAGAIN) || (errno == ETIMEDOUT)));
 
//  if(Ret && (Tries <= 0)) fprintf(stderr, "ioctl (%i) retried %i times - giving up: %s)\n", IOCTL_X, CONFIG_IOCTL_RETRY, strerror(errno));
 
//  return (Ret);
// }
 
 
/**
 * 存储设备名称
*/
static int DevNameSet(const char* pDevName)
{
    int CopyLen = 0;
    CopyLen = strlen(pDevName);
    if(CopyLen > CONFIG_DEVNAME_LEN - 1) {
        printf("Error: device name length over limit: %d", CopyLen);
        return -1;
    } 
    memset(sCameraPrivateData.DevName, 0, CONFIG_DEVNAME_LEN);
    memcpy(sCameraPrivateData.DevName, pDevName, CopyLen);
    return 0;
}
 
/**
 * 重置摄像头信息
*/
static void DevInfoReset(void)
{
    memset(sCameraPrivateData.DevName, 0, CONFIG_DEVNAME_LEN);
    sCameraPrivateData.State = CAMERA_STATE_CLOSE;
    sCameraPrivateData.DevFd = -1;
    sCameraPrivateData.pCallback = NULL;
    sCameraPrivateData.Width = 0;
    sCameraPrivateData.Height = 0;
}
 
 
static inline int Yuv2Rgb(int Y, int U, int V)
{
    uint32_t Pixel32 = 0;
    int R, G, B;
    static long int Ruv, Guv, Buv;
 
    if(Sign3) {
        Sign3 = 0;
        Ruv = 1159 * (V - 128);
        Guv = -380 * (U - 128) + 813 * (V - 128);
        Buv = 2018 * (U - 128);
    }
 
    int Delta = 1164 * (Y - 16);
 
    R = (Delta + Ruv) / 1000;
    G = (Delta - Guv) / 1000;
    B = (Delta + Buv) / 1000;
 
    R = R > 255 ? 255 : R < 0 ? 0 : R;
    G = G > 255 ? 255 : G < 0 ? 0 : G;
    B = B > 255 ? 255 : B < 0 ? 0 : B;
 
    Pixel32 = (B & 0xFF) | (G & 0xFF) << 8 | (R & 0xFF) << 16;
 
    return Pixel32;
}
 
int Yuyv2Rgb32(uint8_t* pYuv, uint8_t* pRgb, uint32_t Width, uint32_t Height)
{
    int y0, U, y1, V;
    uint32_t Pixel32;
    uint8_t* pPixel = (uint8_t *)&Pixel32;
    // 分辨率描述像素点个数，而yuv2个像素点占有4个字符，所以这里计算总的字符个数，需要乘2
    uint32_t Size = Width * Height * 2; 
 
    for(uint32_t In = 0, Out = 0; In < Size; In += 4, Out += 8) {
        y0 = pYuv[In + 0];
        U  = pYuv[In + 1];
        y1 = pYuv[In + 2];
        V  = pYuv[In + 3];
 
        Sign3 = 1;
        Pixel32 = Yuv2Rgb(y0, U, V);
        pRgb[Out + 0] = pPixel[0];   
        pRgb[Out + 1] = pPixel[1];
        pRgb[Out + 2] = pPixel[2];
        pRgb[Out + 3] = 0;  //32位rgb多了一个保留位
 
        Pixel32 = Yuv2Rgb(y1, U, V);
        pRgb[Out + 4] = pPixel[0];
        pRgb[Out + 5] = pPixel[1];
        pRgb[Out + 6] = pPixel[2];
        pRgb[Out + 7] = 0;
 
    }
    return 0;
}
 
int Yuyv2Rgb24(uint8_t* pYuv, uint8_t* pRgb, uint32_t Width, uint32_t Height)
{
    int y0, U, y1, V;
    uint32_t Pixel32;
    uint8_t* pPixel = (uint8_t *)&Pixel32;
    // 分辨率描述像素点个数，而yuv2个像素点占有4个字符，所以这里计算总的字符个数，需要乘2
    uint32_t Size = Width * Height * 2; 
 
    for(uint32_t In = 0, Out = 0; In < Size; In += 4, Out += 6) {
        y0 = pYuv[In + 0];
        U  = pYuv[In + 1];
        y1 = pYuv[In + 2];
        V  = pYuv[In + 3];
 
        Sign3 = 1;
        Pixel32 = Yuv2Rgb(y0, U, V);
        pRgb[Out + 0] = pPixel[0];   
        pRgb[Out + 1] = pPixel[1];
        pRgb[Out + 2] = pPixel[2];
 
        Pixel32 = Yuv2Rgb(y1, U, V);
        pRgb[Out + 3] = pPixel[0];
        pRgb[Out + 4] = pPixel[1];
        pRgb[Out + 5] = pPixel[2];
 
    }
    return 0;
}
 
 
int CameraOpen(const char* pDevName)
{
    int Ret = -1;
 
    /* 检查摄像头是否已经打开或者在错误状态 */
    printf("Open camera %s...\n", pDevName);
    if(sCameraPrivateData.State != CAMERA_STATE_CLOSE) {
        printf("Error: camera was open or meet error, now state is: %d", sCameraPrivateData.State);
        goto ErrorHandle;
    }
 
    /* 打开摄像头设备 */
    if((sCameraPrivateData.DevFd = open(pDevName, O_RDWR, 0)) == -1) {
    // if((sCameraPrivateData.DevFd = open(pDevName, O_RDWR | O_NONBLOCK, 0)) == -1) {
        printf("Open camera %s faild.\n", pDevName);
        goto ErrorHandle;
    }
 
    printf("Open device successful.\n");
 
    /* 设置采集来源 */
    struct v4l2_input Input;
    Input.index = 0;
 
    if (ioctl(sCameraPrivateData.DevFd, VIDIOC_S_INPUT, &Input)) {
        perror("VIDIOC_S_INPUT");
    }
 
    /* 获取摄像头信息 */
    struct v4l2_capability Cap;
    
    Ret = ioctl(sCameraPrivateData.DevFd, VIDIOC_QUERYCAP, &Cap);
 
    if(Ret){
        perror("Get device info error");
        goto ErrorHandle;
    }
 
    printf("Get device info successful.\n");
 
    printf("Driver name: %s\n", Cap.driver);
    printf("Card name: %s\n", Cap.card);
    printf("Bus info: %s\n", Cap.bus_info);
 
    DevNameSet(pDevName);
 
    struct v4l2_format TvFmt;
    struct v4l2_fmtdesc FmtDesc;
    struct v4l2_streamparm StreamParam;
 
    memset(&FmtDesc, 0, sizeof(FmtDesc));
    FmtDesc.index = 0;  
    FmtDesc.type = V4L2_BUF_TYPE_VIDEO_CAPTURE;
 
    /* 枚举支持的桢格式 */
    printf("Support format:\n");
    while(ioctl(sCameraPrivateData.DevFd, VIDIOC_ENUM_FMT, &FmtDesc) == 0){  
        FmtDesc.index++;
        printf("Pixel format = %c%c%c%c\n", 
        (FmtDesc.pixelformat & 0xFF),
        ((FmtDesc.pixelformat >> 8) & 0xFF),
        ((FmtDesc.pixelformat >> 16) & 0xFF),
        (FmtDesc.pixelformat >> 24));
        printf("Description = %s \n", FmtDesc.description); 
    }
 
    /* 判断设备是否是可以捕获视频的设备 */
    if(!(Cap.capabilities & V4L2_BUF_TYPE_VIDEO_CAPTURE)) {  
        printf("The Current device is not a video capture device\n");  
        goto ErrorHandle;
    }
 
    /* 判断是否支持视频流 */
    if(!(Cap.capabilities & V4L2_CAP_STREAMING)) {  
        printf("The Current device does not support streaming i/o\n");  
        goto ErrorHandle;
    }
 
    /* 获取视频流信息 */
    memset(&StreamParam, 0, sizeof(struct v4l2_streamparm));
    StreamParam.type = V4L2_BUF_TYPE_VIDEO_CAPTURE; 
    Ret = ioctl(sCameraPrivateData.DevFd, VIDIOC_G_PARM, &StreamParam);
    if(Ret) {
        perror("Get stream info failed");
        goto ErrorHandle;
    }
 
    printf("Before setting stream params:\n");
    printf("Capability: %u\n", StreamParam.parm.capture.capability);
    printf("Capturemode: %u\n", StreamParam.parm.capture.capturemode);
    printf("Extendedmode: %u\n", StreamParam.parm.capture.extendedmode);
    printf("Timeperframe denominator: %u\n", StreamParam.parm.capture.timeperframe.denominator);
    printf("Timeperframe numerator: %u\n", StreamParam.parm.capture.timeperframe.numerator);
 
    /* 帧率分母 分子设置 */
    StreamParam.parm.capture.timeperframe.denominator = CONFIG_CAPTURE_FPS;
    StreamParam.parm.capture.timeperframe.numerator = 1;
 
    if(ioctl(sCameraPrivateData.DevFd, VIDIOC_S_PARM, &StreamParam) == -1) {
        perror("Unable to set fps");
        goto ErrorHandle;
    }
 
    /* 获取视频流信息 */
    Ret = ioctl(sCameraPrivateData.DevFd, VIDIOC_G_PARM, &StreamParam);
    if(Ret) {
        perror("Get stream info failed");
        goto ErrorHandle;
    }
 
    printf("After setting stream params:\n");
    printf("Capability: %u\n", StreamParam.parm.capture.capability);
    printf("Capturemode: %u\n", StreamParam.parm.capture.capturemode);
    printf("Extendedmode: %u\n", StreamParam.parm.capture.extendedmode);
    printf("Timeperframe denominator: %u\n", StreamParam.parm.capture.timeperframe.denominator);
    printf("Timeperframe numerator: %u\n", StreamParam.parm.capture.timeperframe.numerator);
 
    // struct v4l2_control Ctrl;
    // Ctrl.id = V4L2_CID_EXPOSURE_AUTO;
    // Ret = ioctl(sCameraPrivateData.DevFd, VIDIOC_G_CTRL, &Ctrl);
 
    // if(Ret) {
    //  perror("Get exposure info failed");
    //  goto ErrorHandle;
    // }
 
    // printf("Before setting control params:\n");
    // printf("Controll value: %#X", Ctrl.value);
 
    /* 设置自动曝光 */
    struct v4l2_control Ctrl;
    Ctrl.id = V4L2_CID_EXPOSURE_ABSOLUTE;
    Ret = ioctl(sCameraPrivateData.DevFd, VIDIOC_G_CTRL, &Ctrl);
 
    if(Ret) {
        perror("Set exposure failed");
        // goto ErrorHandle;
    }
 
    /* 设置捕获格式 */
    TvFmt.type = V4L2_BUF_TYPE_VIDEO_CAPTURE;
    TvFmt.fmt.pix.width = CONFIG_CAPTURE_WIDTH;
    TvFmt.fmt.pix.height = CONFIG_CAPTURE_HEIGHT;
    TvFmt.fmt.pix.pixelformat = V4L2_PIX_FMT_YUYV;
    TvFmt.fmt.pix.field = V4L2_FIELD_INTERLACED;
 
    if (ioctl(sCameraPrivateData.DevFd, VIDIOC_S_FMT, &TvFmt) < 0) {  
        perror("Unable set VIDIOC_S_FMT");
        goto ErrorHandle;
    }
 
    if (ioctl(sCameraPrivateData.DevFd, VIDIOC_G_FMT, &TvFmt) < 0) {  
        perror("Get VIDIOC_S_FMT error");
        goto ErrorHandle;
    }
 
    printf("After setting frame params:\n");
    printf("Width: %d, height: %d\n", TvFmt.fmt.pix.width, TvFmt.fmt.pix.height);
        printf("Pixel format = %c%c%c%c\n", 
        (TvFmt.fmt.pix.pixelformat & 0xFF),
        ((TvFmt.fmt.pix.pixelformat >> 8) & 0xFF),
        ((TvFmt.fmt.pix.pixelformat >> 16) & 0xFF),
        (TvFmt.fmt.pix.pixelformat >> 24));
 
    sCameraPrivateData.Width = TvFmt.fmt.pix.width;
    sCameraPrivateData.Height = TvFmt.fmt.pix.height;
 
    /* 请求V4L2驱动申请内存 */
    struct v4l2_requestbuffers ReqBuffer;
    memset(&ReqBuffer, 0, sizeof(ReqBuffer));
    ReqBuffer.count = CONFIG_CAPTURE_BUF_CNT;
    ReqBuffer.type = V4L2_BUF_TYPE_VIDEO_CAPTURE;
    ReqBuffer.memory = V4L2_MEMORY_MMAP;  
 
    if(-1 == ioctl(sCameraPrivateData.DevFd, VIDIOC_REQBUFS, &ReqBuffer)){  
        perror("Fail to ioctl 'VIDIOC_REQBUFS'");
        goto ErrorHandle;
    }
 
    sCameraPrivateData.BufferCnt = ReqBuffer.count;  
    printf("Buffer count: %d\n", sCameraPrivateData.BufferCnt);
 
    sCameraPrivateData.pCameraBuffer = (sBufferType *)calloc(sCameraPrivateData.BufferCnt, sizeof(sBufferType));
    if(sCameraPrivateData.pCameraBuffer == NULL) {
        printf("Malloc buffer failed\n");
        goto ErrorHandle;
    }
 
    /* 映射内核缓存到用户空间 */
    struct v4l2_buffer Buf;  
    for(int i = 0; i < sCameraPrivateData.BufferCnt; i++) {
        memset(&Buf, 0, sizeof(Buf));  
        Buf.type = V4L2_BUF_TYPE_VIDEO_CAPTURE;
        Buf.memory = V4L2_MEMORY_MMAP;
        Buf.index = i;
 
        if(-1 == ioctl(sCameraPrivateData.DevFd, VIDIOC_QUERYBUF, &Buf)) {
            perror("Fail to ioctl : VIDIOC_QUERYBUF");
            goto ErrorHandle;
        }
 
        sCameraPrivateData.pCameraBuffer[i].Length = Buf.length;
        sCameraPrivateData.pCameraBuffer[i].pStart = 
            mmap(NULL, Buf.length, PROT_READ | PROT_WRITE, MAP_SHARED, sCameraPrivateData.DevFd, Buf.m.offset);
 
        if(MAP_FAILED == sCameraPrivateData.pCameraBuffer[i].pStart) {
            perror("Fail to mmap");
            goto ErrorHandle;
        }
 
        if(ioctl(sCameraPrivateData.DevFd, VIDIOC_QBUF, &Buf)) {
            perror("Unable to queue buffer");
            goto ErrorHandle;;
        }
    }
 
    /* 开始捕获 */
    enum v4l2_buf_type BufType;
    BufType = V4L2_BUF_TYPE_VIDEO_CAPTURE;
    if(ioctl(sCameraPrivateData.DevFd, VIDIOC_STREAMON, &BufType) < 0) {
        perror("VIDIOC_STREAMON error");
        goto ErrorHandle;
    }
    sCameraPrivateData.State = CAMERA_SATTE_OPEN;
 
    return 0;
 
ErrorHandle:
    printf("Camera open meet error, now handle it\n");
    close(sCameraPrivateData.DevFd);
    sCameraPrivateData.State = CAMERA_STATE_CLOSE;
    free(sCameraPrivateData.pCameraBuffer);
    sCameraPrivateData.pCameraBuffer = NULL;
    return -1;
}
 
int CameraClose(void)
{
    if(sCameraPrivateData.State == CAMERA_SATTE_OPEN || sCameraPrivateData.State == CAMERA_SATTE_CAP) {
        printf("Close camera %s\n", sCameraPrivateData.DevName);
    } else {
        printf("Skip close camera progress\n");
        return 0;
    }
    
    for(int i = 0; i < CONFIG_CAPTURE_BUF_CNT; i++) {
        
        munmap(sCameraPrivateData.pCameraBuffer[i].pStart, 
                sCameraPrivateData.pCameraBuffer[i].Length);
        sCameraPrivateData.pCameraBuffer[i].Length = 0;
    }
 
    free(sCameraPrivateData.pCameraBuffer);
    sCameraPrivateData.pCameraBuffer = NULL;
 
    int Off = 1;
 
    if(ioctl(sCameraPrivateData.DevFd , VIDIOC_STREAMOFF, &Off))  {
        perror("Stop camera fail");
        return -1 ;
    }
 
    close(sCameraPrivateData.DevFd);
 
    DevInfoReset();
 
    printf("Camera closed\n");
 
    return 0;
}
 
int CameraCaptureCallbackSet(vCameraFrameProcess pCallback)
{
    if(sCameraPrivateData.State == CAMERA_SATTE_CAP) {
        printf("Set capture callback failed");
        return -1;
    }
    sCameraPrivateData.pCallback = pCallback;
    return 0;
}
 
static void* CameraCaptureThread(void* pParam)
{
    int Ret = -1;
    
    struct v4l2_buffer EnQueueBuf;
    struct v4l2_buffer DeQueueBuf;
 
    printf("Start capture\n");
 
    while(sCameraPrivateData.State == CAMERA_SATTE_CAP) {
 
        memset(&DeQueueBuf, 0, sizeof(struct v4l2_buffer));
        DeQueueBuf.type = V4L2_BUF_TYPE_VIDEO_CAPTURE;
        DeQueueBuf.memory = V4L2_MEMORY_MMAP;
 
        memset(&EnQueueBuf, 0, sizeof(struct v4l2_buffer));
        EnQueueBuf.type = V4L2_BUF_TYPE_VIDEO_CAPTURE;
        EnQueueBuf.memory = V4L2_MEMORY_MMAP;
 
        // 取出队列中的数据
        Ret = ioctl(sCameraPrivateData.DevFd, VIDIOC_DQBUF, &DeQueueBuf);
        if(Ret) {
            perror("Dequeue fail");
            break;
        }
        sCameraPrivateData.pCallback(sCameraPrivateData.pCameraBuffer[DeQueueBuf.index].pStart, 
                                        sCameraPrivateData.Width,
                                        sCameraPrivateData.Height,
                                        sCameraPrivateData.pCameraBuffer[DeQueueBuf.index].Length);
 
        // 重新入队列
        EnQueueBuf.index = DeQueueBuf.index;
 
        if(ioctl(sCameraPrivateData.DevFd , VIDIOC_QBUF , &EnQueueBuf)) {
            perror("Enqueue fail");
            break;
        }
    }
 
    printf("Stop capture\n");
 
    return NULL;
}
 
int CameraCaptureStart(void)
{
    printf("Start capture thread\n");
    if(!sCameraPrivateData.pCallback) {
        printf("Capture callback not set, start faild\n");
        return -1;
    }
    if(sCameraPrivateData.State != CAMERA_SATTE_OPEN) {
        printf("Camera not open, start faild\n");
        return -1;
    }
    sCameraPrivateData.State = CAMERA_SATTE_CAP;
 
    pthread_create(&sCameraPrivateData.CaptureThreadId, NULL, CameraCaptureThread, (void *)NULL);
 
    return 0;
}
 
int CameraCaptureStop(void)
{
    printf("Stop capture thread\n");
    sCameraPrivateData.State = CAMERA_SATTE_OPEN;
    pthread_join(sCameraPrivateData.CaptureThreadId, NULL);
    printf("Capture thread stop\n");
    return 0;
}