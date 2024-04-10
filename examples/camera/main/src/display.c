/* display.c */
#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>
#include <string.h>
#include <pthread.h>
#include <unistd.h>
#include <fcntl.h>
#include <errno.h>
#include <sys/ioctl.h>
#include <sys/mman.h>
#include <sys/stat.h>
#include <sys/time.h>
#include <linux/fb.h>
#include <pthread.h>
 
#include "config.h"
#include "queue.h"
#include "display.h"
 
typedef enum {
    DISPLAY_STATE_STOP,
    DISPLAY_STATE_START
} eDisplayState;
 
static struct {
    int DevFd;
    int ScreenSize;
    uint8_t* pMmap;
    eDisplayState State;
    pthread_t DisplayThreadId;
    sQueue QueuePrivateData;
    uint32_t ScreenWidth;
    uint32_t ScreenHeight;
} sDisplayPrivateData;
 
static const int BufferSize = CONFIG_CAPTURE_WIDTH * CONFIG_CAPTURE_HEIGHT * 4;
 
extern int Yuyv2Rgb32(uint8_t* pYuv, uint8_t* pRgb, uint32_t Width, uint32_t Height);
 
int DisplayInit(const char* pDevName)
{
    QueueInit(&sDisplayPrivateData.QueuePrivateData);
 
    struct fb_fix_screeninfo FbFix;
    struct fb_var_screeninfo FbVar;
 
    if((sDisplayPrivateData.DevFd = open(pDevName, O_RDWR)) < 0){
        perror("Open Framebuffer Device");
        return -1;
    }
 
    printf("Open Framebuffer Device:%s successful!\n", pDevName);
 
    if(ioctl(sDisplayPrivateData.DevFd, FBIOGET_FSCREENINFO, &FbFix)){
        perror("Get Fb_fix_info error!");
        return -1;
    }
    printf("Get Fb_fix_info successful!\n");
 
    if(ioctl(sDisplayPrivateData.DevFd, FBIOGET_VSCREENINFO, &FbVar)){
        printf("Get Fb_var_info error!\n");
        return -1;
    }
    printf("Get Fb_var_info successful!\n");
    
    printf("Screen origin param: physics resultion %dx%d, line length: %d\n", FbVar.xres, FbVar.yres, FbFix.line_length);
    printf("Screen origin param: virtual resultion %dx%d\n", FbVar.xres_virtual, FbVar.yres_virtual);
 
    // FbVar.xres = CONFIG_CAPTURE_WIDTH;
    // FbVar.yres = CONFIG_CAPTURE_HEIGHT;
    FbVar.xres_virtual = CONFIG_CAPTURE_WIDTH;
    FbVar.yres_virtual = CONFIG_CAPTURE_WIDTH;
 
    if(ioctl(sDisplayPrivateData.DevFd, FBIOPUT_VSCREENINFO, &FbVar)) {
        printf("Set Fb_var_info error!\n");
        // return -1;
    }
    printf("Set Fb_var_info successful!\n");
 
    if(ioctl(sDisplayPrivateData.DevFd, FBIOGET_VSCREENINFO, &FbVar)) {
        printf("Get Fb_var_info error!\n");
        return -1;
    }
    printf("Get Fb_var_info successful!\n");
    
    printf("Screen after param: physics resultion %dx%d, line length: %d\n", FbVar.xres, FbVar.yres, FbFix.line_length);
    printf("Screen after param: virtual resultion %dx%d\n", FbVar.xres_virtual, FbVar.yres_virtual);
 
    sDisplayPrivateData.ScreenWidth = FbVar.xres;
    sDisplayPrivateData.ScreenHeight = FbVar.yres;
 
    sDisplayPrivateData.ScreenSize = FbVar.xres * FbVar.yres * (FbVar.bits_per_pixel / 8);
 
    printf("Screen size: %d\n", sDisplayPrivateData.ScreenSize);
 
    sDisplayPrivateData.pMmap = mmap(NULL, sDisplayPrivateData.ScreenSize, PROT_READ | PROT_WRITE, MAP_SHARED, sDisplayPrivateData.DevFd, 0);
    if(!sDisplayPrivateData.pMmap){
        perror("Memory Mmap error");
        return -1;
    }
 
    printf("Memory Mmap successful!\n");
 
    return 0;
 
}
 
static void* DisplayThread(void* pParam)
{
    int Ret = -1;
    sDisplayPrivateData.State = DISPLAY_STATE_START;
 
    uint8_t* pRgbBuffer = malloc(BufferSize);
    if(!pRgbBuffer) {
        perror("Malloc failed");
        return NULL;
    }
 
    while(sDisplayPrivateData.State == DISPLAY_STATE_START) {
        sQueueData QueueData;
        Ret = QueuePopData(&sDisplayPrivateData.QueuePrivateData, &QueueData);
 
        if(Ret) {
            continue;
        }
 
        Yuyv2Rgb32(QueueData.pData, pRgbBuffer, CONFIG_CAPTURE_WIDTH, CONFIG_CAPTURE_HEIGHT);
        // printf("Copy length: %d\n", BufferSize);
 
        // memcpy(sDisplayPrivateData.pMmap, pRgbBuffer, BufferSize);
 
        uint32_t CopyWidthLength = sDisplayPrivateData.ScreenWidth > CONFIG_CAPTURE_WIDTH ?
                                    CONFIG_CAPTURE_WIDTH : sDisplayPrivateData.ScreenWidth;
 
        for(int i = 0; i < CONFIG_CAPTURE_HEIGHT; i++) {
            memcpy(sDisplayPrivateData.pMmap + sDisplayPrivateData.ScreenWidth * 4 * i, 
                &pRgbBuffer[CONFIG_CAPTURE_WIDTH * 4 * i], 
                    CopyWidthLength * 4);
        }
 
        QueueCallback(&QueueData);
    }
 
    free(pRgbBuffer);
    DisplayStop();
 
    return NULL;
}
 
int DisplayStart(void)
{
    if(sDisplayPrivateData.State == DISPLAY_STATE_START) {
        return -1;
    }
    pthread_create(&sDisplayPrivateData.DisplayThreadId, NULL, DisplayThread, (void *)NULL);
    return 0;
 
}
 
int DisplayStop(void)
{
    if(sDisplayPrivateData.State == DISPLAY_STATE_STOP) {
        return 0;
    }
    sDisplayPrivateData.State = DISPLAY_STATE_STOP;
    pthread_join(sDisplayPrivateData.DisplayThreadId, NULL);
    munmap(sDisplayPrivateData.pMmap, sDisplayPrivateData.ScreenSize);
    close(sDisplayPrivateData.DevFd);
    printf("Display thread stop\n");
    return 0;
}