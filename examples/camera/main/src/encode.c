/* encode.c */
#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <string.h>
#include <pthread.h>
#include <x264.h>
#include <pthread.h>
#include "config.h"
#include "encode.h"
#include "queue.h"
 
 
#define CONFIG_CSP X264_CSP_I422
 
typedef enum {
    ENCODE_STATE_STOP,
    ENCODE_STATE_START
} eEncodeState;
 
struct {
    x264_nal_t* pNals;
    x264_t* pHandle;
    x264_picture_t* pPicIn;
    x264_param_t* pParam;
    eEncodeState State;
    pthread_t EncodeThreadId;
    FILE* pFile;
    sQueue QueuePrivateData;
} sEncodePrivateData;
 
 
static int H264FrameEncodeBegin(uint32_t Width, uint32_t Height)
{
    QueueInit(&sEncodePrivateData.QueuePrivateData);
    sEncodePrivateData.pPicIn = (x264_picture_t*)malloc(sizeof(x264_picture_t));
    sEncodePrivateData.pParam = (x264_param_t*)malloc(sizeof(x264_param_t));
 
    x264_param_default(sEncodePrivateData.pParam);
    x264_param_default_preset(sEncodePrivateData.pParam, "fast" , "zerolatency" );
 
    /* 设置参数 */
    sEncodePrivateData.pParam->i_csp = CONFIG_CSP;
    sEncodePrivateData.pParam->i_width = Width;
    sEncodePrivateData.pParam->i_height = Height; 
    sEncodePrivateData.pParam->i_fps_num = CONFIG_CAPTURE_FPS;
    sEncodePrivateData.pParam->i_fps_den = 1;
 
    sEncodePrivateData.pParam->i_threads  = X264_SYNC_LOOKAHEAD_AUTO;
    sEncodePrivateData.pParam->i_keyint_max = 10;
 
    sEncodePrivateData.pParam->rc.i_bitrate = CONFIG_ENCODE_BITRATE;
    sEncodePrivateData.pParam->rc.i_rc_method = X264_RC_ABR;
 
    x264_param_apply_profile(sEncodePrivateData.pParam, "high422");
 
    sEncodePrivateData.pHandle = x264_encoder_open(sEncodePrivateData.pParam);
 
    x264_picture_alloc(sEncodePrivateData.pPicIn, CONFIG_CSP, 
        sEncodePrivateData.pParam->i_width, sEncodePrivateData.pParam->i_height);
 
    return 0;
}
 
static int H264FrameEncodeEnd(void)
{
    printf("Encode end, now clear resource\n");
    if (sEncodePrivateData.pHandle) {
        x264_encoder_close(sEncodePrivateData.pHandle);
    }
 
    if (sEncodePrivateData.pPicIn) {
        x264_picture_clean(sEncodePrivateData.pPicIn);
        free(sEncodePrivateData.pPicIn);
        sEncodePrivateData.pPicIn = NULL;
    }
 
    if (sEncodePrivateData.pParam) {
        free(sEncodePrivateData.pParam);
        sEncodePrivateData.pParam = NULL;
    }
    if(sEncodePrivateData.pFile) {
        fclose(sEncodePrivateData.pFile);
        sEncodePrivateData.pFile = NULL;
    }
 
    return 0;
}
 
static int H264FrameEncode(const uint8_t* pData, uint32_t Width, uint32_t Height, uint32_t Length)
{
    x264_picture_t* pPicOut = (x264_picture_t*)malloc(sizeof(x264_picture_t));
    if(!pPicOut) {
        perror("Malloc failed");
        return -1;
    }
    x264_picture_init(pPicOut);
 
    int IndexY, IndexU, IndexV;
    int YuyvLength = Width * Height * 2 - 4;
    int NalCnt = 0;
    int Result = 0;
    static long int pts = 0;
    // uint8_t* pOut = pDataOut;
    uint8_t* pY = sEncodePrivateData.pPicIn->img.plane[0];
    uint8_t* pU = sEncodePrivateData.pPicIn->img.plane[1];
    uint8_t* pV = sEncodePrivateData.pPicIn->img.plane[2];
 
    // if(YuyvLength != Length) {
    //  printf("Param invalid, Length not match\n");
    //  return -1;
    // }
 
    IndexY = 0;
    IndexU = 0;
    IndexV = 0;
 
    for(int i = 0; i < YuyvLength; i += 4) {
            *(pY + (IndexY++)) = *(pData + i);
            *(pU + (IndexU++)) = *(pData + i + 1);
            *(pY + (IndexY++)) = *(pData + i + 2);
            *(pV + (IndexV++)) = *(pData + i + 3);
    }
 
    sEncodePrivateData.pPicIn->i_type = X264_TYPE_AUTO;
 
    sEncodePrivateData.pPicIn->i_pts = pts++;
 
    if (x264_encoder_encode(sEncodePrivateData.pHandle, &(sEncodePrivateData.pNals), &NalCnt, sEncodePrivateData.pPicIn, pPicOut) < 0) {
        free(pPicOut);
        return -1;
    }
    for (int i = 0; i < NalCnt; i++) {
        // memcpy(pOut, sEncodePrivateData.pNals[i].p_payload, sEncodePrivateData.pNals[i].i_payload);
        // pOut += sEncodePrivateData.pNals[i].i_payload;
        fwrite(sEncodePrivateData.pNals[i].p_payload, 1, sEncodePrivateData.pNals[i].i_payload, sEncodePrivateData.pFile);
        Result += sEncodePrivateData.pNals[i].i_payload;
    }
 
    free(pPicOut);
 
    return Result;
}
 
static void* H264EncodeThread(void* pParam)
{
    int Ret = -1;
    while(sEncodePrivateData.State == ENCODE_STATE_START) {
        sQueueData QueueData;
        Ret = QueuePopData(&sEncodePrivateData.QueuePrivateData, &QueueData);
        if(Ret) {
            continue;
        }
        H264FrameEncode(QueueData.pData, 
                CONFIG_CAPTURE_WIDTH, CONFIG_CAPTURE_HEIGHT, 
                QueueData.Length);
        QueueCallback(&QueueData);
    }
    H264FrameEncodeEnd();
    return NULL;
}
 
int EncodeStart(const char* pFilename)
{
    H264FrameEncodeBegin(CONFIG_CAPTURE_WIDTH, CONFIG_CAPTURE_HEIGHT);
 
    if(sEncodePrivateData.State) {
        printf("Encode thread already start\n");
        return -1;
    }
    sEncodePrivateData.State = ENCODE_STATE_START;
 
    sEncodePrivateData.pFile = fopen(pFilename, "w+");
    if(!sEncodePrivateData.pFile) {
        perror("Create h264 file failed");
        return -1;
    }
 
    pthread_create(&sEncodePrivateData.EncodeThreadId, NULL, H264EncodeThread, (void *)NULL);
 
    return 0;
}
 
int EncodeStop(void)
{
    sEncodePrivateData.State = ENCODE_STATE_STOP;
    pthread_join(sEncodePrivateData.EncodeThreadId, NULL);
    printf("Encode thread stop\n");
    return 0;
}