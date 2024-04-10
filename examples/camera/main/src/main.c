/* main.c */
/**
 * author:  rootming
 * date:    2019.4
 * version: v1.0
*/
 
/*
    # Video capture
    ## Basic note
    1. use V4L2 interface open camera & capture video
    2. use framebuffer driver for preview
    3. text overlay video
    4. use h264 algorithm compresse frame
    ## Hardware
    1. Raspberry Pi 3
    2. USB camera
    ## Target
    1. capture frame size: 640*480
    2. display fps: >= 30fps
    3. memory limit: <20M
    ## Addtion
    1. Maybe can add log library
*/
#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <string.h>
#include <signal.h>

#include "config.h"
#include "camera.h"
//#include "encode.h"
#include "display.h"
#include "queue.h"
 
 
/* 入队列回调 */
void EnQueueCallback(uint8_t* pData, uint32_t Width, uint32_t Height, uint32_t Length)
{
    sQueueData QueueData;
    QueueData.pData = malloc(Length);
    if(!QueueData.pData) {
        perror("Malloc failed");
        return;
    }
    QueueData.Length = Length;
    memcpy(QueueData.pData, pData, Length);
    QueuePutData(&QueueData);
}
 
void SignalHandle(int SignalNumber)
{
    printf("Now clean resource\n");
    CameraCaptureStop();
    CameraClose();
    DisplayStop();
//    EncodeStop();
}
 
int main(int Argc, char* pArgv[])
{
    int Ret = -1;
 
    signal(SIGINT, SignalHandle);
 
    Ret = CameraOpen(CONFIG_CAPTURE_DEVICE);
    if(Ret) {
        printf("Camera open failed \n");
        return -1;
    }
    
    Ret = DisplayInit(CONFIG_DISPLAY_DEV);
 
    if(Ret) {
        printf("Diaplay open failed \n");
        return -1;
    }
 
    CameraCaptureCallbackSet(EnQueueCallback);
    CameraCaptureStart();
    DisplayStart();
 //   EncodeStart("test.h264");
 
    char KeyValue = getchar();
    printf("You press [%c] button, now stop capture\n", KeyValue);
    SignalHandle(0);
    return 0;
}