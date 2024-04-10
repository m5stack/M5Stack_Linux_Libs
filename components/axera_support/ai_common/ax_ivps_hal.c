

// /**********************************************************************************
//  *
//  * Copyright (c) 2019-2020 Beijing AXera Technology Co., Ltd. All Rights Reserved.
//  *
//  * This source file is the property of Beijing AXera Technology Co., Ltd. and
//  * may not be copied or distributed in any isomorphic form without the prior
//  * written consent of Beijing AXera Technology Co., Ltd.
//  *
//  **********************************************************************************/
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <signal.h>
#include <errno.h>
#include <unistd.h>
#include <pthread.h>

#include "ax_sys_api.h"
#include "ax_ivps_api.h"
#include "sample_log.h"
#include "ax_ivps_hal.h"

#define SAMPLE_NAME "SampleVenc"
#define VENC_LOG(str, arg...)                                                  \
    do                                                                         \
    {                                                                          \
        printf("%s: %s:%d " str "\n", SAMPLE_NAME, __func__, __LINE__, ##arg); \
    } while (0)

#define SAMPLE_LOG(str, arg...)                                                \
    do                                                                         \
    {                                                                          \
        printf("%s: %s:%d " str "\n", SAMPLE_NAME, __func__, __LINE__, ##arg); \
    } while (0)

#define SAMPLE_ERR_LOG(str, arg...)                                                       \
    do                                                                                    \
    {                                                                                     \
        printf("%s: %s:%d Error! " str "\n", "sample_venc.c", __func__, __LINE__, ##arg); \
    } while (0)


#ifndef ALIGN_UP
#define ALIGN_UP(x, align) ((((x) + ((align)-1)) / (align)) * (align))
#endif


static int _private_flage[20] = {0};
static int _private_flage_len = 0;



static void set_Chn_mode_par_0(ax_ivps_dev_info *self)
{
    // // 输入参数
    // self->stPipelineAttr.tFilter[self->Chn][0].bEnable = AX_TRUE;
    // self->stPipelineAttr.tFilter[self->Chn][0].tFRC.nSrcFrameRate = 30;
    // self->stPipelineAttr.tFilter[self->Chn][0].tFRC.nDstFrameRate = 30;
    // self->stPipelineAttr.tFilter[self->Chn][0].nDstPicOffsetX0 = 0;
    // self->stPipelineAttr.tFilter[self->Chn][0].nDstPicOffsetY0 = 0;
    // self->stPipelineAttr.tFilter[self->Chn][0].nDstPicWidth = self->n_ivps_width;
    // self->stPipelineAttr.tFilter[self->Chn][0].nDstPicHeight = self->n_ivps_height;
    // self->stPipelineAttr.tFilter[self->Chn][0].nDstPicStride = ALIGN_UP(self->ChnstPipelineAttr.tFilter[self->Chn][0].nDstPicWidth, 64);
    // self->stPipelineAttr.tFilter[self->Chn][0].nDstFrameWidth = self->n_ivps_width;
    // self->stPipelineAttr.tFilter[self->Chn][0].nDstFrameHeight = self->n_ivps_height;
    // self->stPipelineAttr.tFilter[self->Chn][0].eDstPicFormat = AX_YUV420_SEMIPLANAR;
    // self->stPipelineAttr.tFilter[self->Chn][0].eEngine = AX_IVPS_ENGINE_TDP;
    // // if (self->b_letterbox)
    // // {
    // //     // letterbox filling image
    // //     AX_IVPS_ASPECT_RATIO_S tAspectRatio;
    // //     tAspectRatio.eMode = AX_IVPS_ASPECT_RATIO_AUTO;
    // //     tAspectRatio.eAligns[0] = AX_IVPS_ASPECT_RATIO_HORIZONTAL_CENTER;
    // //     tAspectRatio.eAligns[1] = AX_IVPS_ASPECT_RATIO_VERTICAL_CENTER;
    // //     tAspectRatio.nBgColor = 0x0000FF;
    // //     self->stPipelineAttr.tFilter[self->Chn][0].tTdpCfg.tAspectRatio = tAspectRatio;
    // // }
    // // 翻转
    // self->stPipelineAttr.tFilter[self->Chn][0].tTdpCfg.bFlip = self->b_ivps_flip > 0 ? AX_TRUE : AX_FALSE;
    // // 镜像
    // self->stPipelineAttr.tFilter[self->Chn][0].tTdpCfg.bMirror = self->b_ivps_mirror > 0 ? AX_TRUE : AX_FALSE;
    // // 0-0 1-90 2-180 3-270 旋转
    // self->stPipelineAttr.tFilter[self->Chn][0].tTdpCfg.eRotation = (AX_IVPS_ROTATION_E)self->n_ivps_rotate;
    // // 重新定义画面
    // if(self->stPipelineAttr.tFilter[self->Chn][0].tTdpCfg.eRotation == AX_IVPS_ROTATION_90 
    // || self->stPipelineAttr.tFilter[self->Chn][0].tTdpCfg.eRotation == AX_IVPS_ROTATION_270)
    // {
    //     int tmp_change = self->stPipelineAttr.tFilter[self->Chn][0].nDstPicWidth;
    //     self->stPipelineAttr.tFilter[self->Chn][0].nDstPicWidth = self->stPipelineAttr.tFilter[self->Chn][0].nDstPicHeight;
    //     self->stPipelineAttr.tFilter[self->Chn][0].nDstPicHeight = tmp_change;
    //     self->stPipelineAttr.tFilter[self->Chn][0].nDstPicStride = ALIGN_UP(self->stPipelineAttr.tFilter[self->Chn][0].nDstPicWidth, 64);
    //     self->stPipelineAttr.tFilter[self->Chn][0].nDstFrameWidth = self->stPipelineAttr.tFilter[self->Chn][0].nDstPicWidth;
    //     self->stPipelineAttr.tFilter[self->Chn][0].nDstFrameHeight = self->stPipelineAttr.tFilter[self->Chn][0].nDstPicHeight;
    // }
    // // 设置输出图像格式
    // self->stPipelineAttr.tFilter[self->Chn][0].eDstPicFormat = AX_FORMAT_RGB888;



    // // [0]表示不输出，[1-4]表示队列的个数，大于[0]则可以在调用回调输出图像
    // self->stPipelineAttr.nOutFifoDepth[self->Chn] = self->m_ivps_attr.n_fifo_count;

    // if (self->stPipelineAttr.nOutFifoDepth[self->Chn] < 0)
    //     self->stPipelineAttr.nOutFifoDepth[self->Chn] = 0;
    // if (self->stPipelineAttr.nOutFifoDepth[self->Chn] > 4)
    //     self->stPipelineAttr.nOutFifoDepth[self->Chn] = 4;






}

// static void set_Pipe_mode_par(ax_ivps_hal *self)
// {

// }


static void set_Chn_mode_par_1(ax_ivps_hal *self)
{
    self->stGrpAttr.nInFifoDepth = 1;
    self->stGrpAttr.ePipeline = AX_IVPS_PIPELINE_DEFAULT;

    self->stPipelineAttr.tFbInfo.PoolId = AX_INVALID_POOLID;
    self->stPipelineAttr.nOutChnNum = 3;// 通道的过滤器控制参数。通道0 是组过滤器。从编号1开始是通道过滤器
    
    int chnid = 0;
    AX_IVPS_FILTER_S *config = &self->stPipelineAttr.tFilter[chnid][0];
    self->stPipelineAttr.tFilter[chnid][0].bEnable = AX_TRUE;
    self->stPipelineAttr.tFilter[chnid][0].tFRC.nSrcFrameRate = 30;// 帧率控制
    self->stPipelineAttr.tFilter[chnid][0].tFRC.nDstFrameRate = 30;
    self->stPipelineAttr.tFilter[chnid][0].nDstPicOffsetX0 = 0;// 设置图片剪切
    self->stPipelineAttr.tFilter[chnid][0].nDstPicOffsetY0 = 0;
    self->stPipelineAttr.tFilter[chnid][0].nDstPicWidth = 1280;
    self->stPipelineAttr.tFilter[chnid][0].nDstPicHeight = 720;
    self->stPipelineAttr.tFilter[chnid][0].nDstPicStride = ALIGN_UP(self->stPipelineAttr.tFilter[0][0].nDstPicWidth, 64);
    self->stPipelineAttr.tFilter[chnid][0].nDstFrameWidth = 1280;// 设置输出帧大小
    self->stPipelineAttr.tFilter[chnid][0].nDstFrameHeight = 720;
    self->stPipelineAttr.tFilter[chnid][0].eDstPicFormat = AX_YUV420_SEMIPLANAR;
    self->stPipelineAttr.tFilter[chnid][0].tTdpCfg.bFlip = AX_FALSE;// 翻转
    self->stPipelineAttr.tFilter[chnid][0].tTdpCfg.bMirror = AX_FALSE;// 镜像
    self->stPipelineAttr.tFilter[chnid][0].tTdpCfg.eRotation = AX_IVPS_ROTATION_0;// 旋转
    self->stPipelineAttr.tFilter[chnid][0].eEngine = AX_IVPS_ENGINE_TDP;// 设置使用引擎
    // self->stPipelineAttr.nOutFifoDepth[chnid] = 0; // 用户是否需要图像
    // 填充，一般用于ai检测
    //     // letterbox filling image
    //     AX_IVPS_ASPECT_RATIO_S tAspectRatio;
    //     tAspectRatio.eMode = AX_IVPS_ASPECT_RATIO_AUTO;
    //     tAspectRatio.eAligns[0] = AX_IVPS_ASPECT_RATIO_HORIZONTAL_CENTER;
    //     tAspectRatio.eAligns[1] = AX_IVPS_ASPECT_RATIO_VERTICAL_CENTER;
    //     tAspectRatio.nBgColor = 0x0000FF;
    //     stPipelineAttr.tFilter[nChn][0].tTdpCfg.tAspectRatio = tAspectRatio;
    chnid = 1;
    memcpy(&self->stPipelineAttr.tFilter[chnid][0], &self->stPipelineAttr.tFilter[0][0], sizeof(AX_IVPS_FILTER_S));
    self->stPipelineAttr.tFilter[chnid][0].eEngine = AX_IVPS_ENGINE_VO;
    chnid = 2;
    memcpy(&self->stPipelineAttr.tFilter[chnid][0], &self->stPipelineAttr.tFilter[0][0], sizeof(AX_IVPS_FILTER_S));
    self->stPipelineAttr.tFilter[chnid][0].eEngine = AX_IVPS_ENGINE_VO;
    
}
// static void set_Chn_mode_par_after_1(ax_ivps_dev_info *self)
// {

// }

static int hal_OpenGrp(ax_ivps_hal *self, int Chn, int mode)
{
    AX_S32 s32Ret;
    self->set_Chn_mode_par[mode](self);
    // 创建组
    s32Ret = AX_IVPS_CreateGrp(self->GRP_id, &self->stGrpAttr);
    if (0 != s32Ret)
    {
        ALOGE("AX_IVPS_CreateGrp failed,nGrp %d,s32Ret:0x%x\n", self->GRP_id, s32Ret);
        return -2;
    }
    self->status = 1;

    // 设置组过滤器
    s32Ret = AX_IVPS_SetPipelineAttr(self->GRP_id, &self->stPipelineAttr);
    if (0 != s32Ret)
    {
        ALOGE("AX_IVPS_SetPipelineAttr failed,nGrp %d,s32Ret:0x%x\n", self->GRP_id, s32Ret);
        return s32Ret;
    }
    self->status = 2;

    for (int i = 0; i < 5; i++)
    {
        self->dev[i].GRP_id = self->GRP_id;
        self->dev[i].Chn = i;
    }
}

static int hal_SetChnOut(ax_ivps_hal *self, int Chn, void (*out_farm)(void *))
{
    self->dev[Chn].out_farm = out_farm;
}

static void *hal_ivps_get_farm_pthread(void *p)
{
    ax_ivps_dev_info *self = (ax_ivps_dev_info *)p;

    while (self->status)
    {
        AX_VIDEO_FRAME_S tVideoFrame;
        AX_S32 ret = AX_IVPS_GetChnFrame(self->GRP_id, self->Chn, &tVideoFrame, 200);
        if (0 != ret)
        {
            if (AX_ERR_IVPS_BUF_EMPTY == ret)
            {
                usleep(1000);
                continue;
            }
            usleep(1000);
            continue;
        }
        tVideoFrame.u64VirAddr[0] = (AX_U32)AX_POOL_GetBlockVirAddr(tVideoFrame.u32BlkId[0]);
        tVideoFrame.u64PhyAddr[0] = AX_POOL_Handle2PhysAddr(tVideoFrame.u32BlkId[0]);
        
        self->out_farm(&tVideoFrame);

        AX_IVPS_ReleaseChnFrame(self->GRP_id, self->Chn, &tVideoFrame);
    }
    return NULL;
}


static int hal_on_osd(ax_ivps_hal *self, int Chn, AX_IVPS_RGN_DISP_GROUP_S *osd)
{
    int tmp_tran[] = {0x00, 0x01, 0x10, 0x11, 0x20, 0x21, 0x30, 0x31, 0x40, 0x41, 0x50, 0x51};
    if(self->n_osd_rgn_chn_hand[Chn])
    {
        return AX_IVPS_RGN_Update(self->n_osd_rgn_chn[Chn], osd);
    }
    else
    {
        IVPS_RGN_HANDLE hChnRgn = AX_IVPS_RGN_Create();
        if (AX_IVPS_INVALID_REGION_HANDLE != hChnRgn)
        {
            int nRet = AX_IVPS_RGN_AttachToFilter(hChnRgn, self->GRP_id, tmp_tran[Chn]);
            if (0 != nRet)
            {
                ALOGE("AX_IVPS_RGN_AttachToFilter(Grp: %d, Filter: 0x%x) failed, ret=0x%x", self->GRP_id, tmp_tran[Chn], nRet);
                return nRet;
            }
            self->n_osd_rgn_chn[Chn] = hChnRgn;
            self->n_osd_rgn_chn_hand[Chn] = 1;
        }
        return 0;
    }
}


static int hal_on_farm(ax_ivps_hal *self, int Chn, Venc_Frame *buffer)
{

    return 0;
}
/**
 * 当通道值为0-5时对应的是通道号
 * 当通道值为-1 时，开启组通道
*/
static void hal_start(ax_ivps_hal *self, int Chn)
{
    AX_S32 s32Ret;
    if(Chn != -1)
    {
        ALOGI("AX_IVPS_EnableChn:%d\n", Chn);
        s32Ret = AX_IVPS_EnableChn(self->GRP_id, Chn);
        if (0 != s32Ret)
        {
            ALOGE("AX_IVPS_EnableChn failed,nGrp %d,nChn %d,s32Ret:0x%x\n", self->GRP_id, Chn, s32Ret);
            return s32Ret;
        }
        
        self->dev[Chn].status = 1;
        if(self->dev[Chn].out_farm)
        {
            pthread_create(&self->dev[Chn].farm_pthread_p, NULL, hal_ivps_get_farm_pthread, &self->dev[Chn]);
        }
    }
    else
    {
        ALOGI("AX_IVPS_StartGrp\n");
        s32Ret = AX_IVPS_StartGrp(self->GRP_id);
        if (0 != s32Ret)
        {
            ALOGE("AX_IVPS_StartGrp failed,nGrp %d,s32Ret:0x%x\n", self->GRP_id, s32Ret);
            return s32Ret;
        }
        self->status = 3;
    }
}

static void hal_stop(ax_ivps_hal *self, int Chn)
{
    AX_S32 s32Ret;
    int tmp_tran[] = {0x00, 0x01, 0x10, 0x11, 0x20, 0x21, 0x30, 0x31, 0x40, 0x41, 0x50, 0x51};
    if(Chn != -1)
    {
        self->dev[Chn].status = 0;
        if(self->dev[Chn].out_farm)
        {
            pthread_join(self->dev[Chn].farm_pthread_p, NULL);
        }
    
        if(self->n_osd_rgn_chn_hand[2 * Chn + 2])
        {
            AX_IVPS_RGN_AttachToFilter(self->n_osd_rgn_chn[2 * Chn + 2], self->GRP_id, tmp_tran[2 * Chn + 2]);
            AX_IVPS_RGN_Destroy(self->n_osd_rgn_chn[2 * Chn + 2]);
        }
        if(self->n_osd_rgn_chn_hand[2 * Chn + 3])
        {
            AX_IVPS_RGN_AttachToFilter(self->n_osd_rgn_chn[2 * Chn + 3], self->GRP_id, tmp_tran[2 * Chn + 3]);
            AX_IVPS_RGN_Destroy(self->n_osd_rgn_chn[2 * Chn + 3]);
        }


        AX_IVPS_DisableChn(self->GRP_id, Chn);
    }
    else
    {
        if(self->n_osd_rgn_chn_hand[0])
        {
            AX_IVPS_RGN_AttachToFilter(self->n_osd_rgn_chn[0], self->GRP_id, tmp_tran[0]);
            AX_IVPS_RGN_Destroy(self->n_osd_rgn_chn[0]);
        }
        if(self->n_osd_rgn_chn_hand[1])
        {
            AX_IVPS_RGN_AttachToFilter(self->n_osd_rgn_chn[1], self->GRP_id, tmp_tran[1]);
            AX_IVPS_RGN_Destroy(self->n_osd_rgn_chn[1]);
        }

        if(self->status == 3)
        {
            AX_IVPS_DestoryGrp(self->GRP_id);
            self->status = 0;
        }
    }
}



static int hal_getStatus(ax_ivps_hal *self, int Chn)
{
    return self->dev[Chn].status;
}


int ax_create_ivps(ax_ivps_hal *ivps_dev)
{
    AX_S32 s32Ret;
    memset(ivps_dev, 0, sizeof(ax_ivps_hal));
    if(_private_flage_len == 0)
    {
        s32Ret = AX_IVPS_Init();
        if(s32Ret)
        {
            return -1;
        }
    }
    ivps_dev->GRP_id = _private_flage_len;
    _private_flage[ivps_dev->GRP_id] = 1;
    
    // ivps_dev->init = hal_init;
    // ivps_dev->exit = hal_exit;

    ivps_dev->OpenChn = hal_OpenGrp;
    // ivps_dev->SetChnOut = hal_SetChnOut;
    // ivps_dev->on_farm = hal_on_farm;
    ivps_dev->set_farm_on = hal_SetChnOut;
    ivps_dev->start = hal_start;
    ivps_dev->stop = hal_stop;
    ivps_dev->getStatus = hal_getStatus;
    ivps_dev->on_osd = hal_on_osd;

    ivps_dev->set_Chn_mode_par[0] = set_Chn_mode_par_0;
    ivps_dev->set_Chn_mode_par[1] = set_Chn_mode_par_1;

    // ivps_dev->set_Chn_mode_par_after[1] = set_Chn_mode_par_after_1;

    _private_flage_len ++;
    return 0;
}
void ax_destroy_ivps(ax_ivps_hal *ivps_dev)
{
    for (int i = 0; i < 5; i++)
    {
        if(ivps_dev->dev[i].status)
        {
            ivps_dev->stop(ivps_dev, i);
        }
    }
    ivps_dev->stop(ivps_dev, -1);
    if(_private_flage[ivps_dev->GRP_id] == 1 && _private_flage_len == 1)
    {
        AX_IVPS_Deinit();
    }
    _private_flage[ivps_dev->GRP_id] = 0;
    _private_flage_len --;
    ALOGI("ivps_exit over");
}
