//
// Copyright (c) 2023-2025 dianjixz
//

#ifndef __AX_VENC_HAL_H_
#define __AX_VENC_HAL_H_


#include "sample_log.h"
#include "ax_venc_api.h"
#include "ax_ivps_api.h"
#include "common_codec/common_venc.h"
#include "my_hal.h"


typedef struct _stRCInfo
{
    SAMPLE_VENC_RC_E eRCType;
    AX_U32 nMinQp;
    AX_U32 nMaxQp;
    AX_U32 nMinIQp;
    AX_U32 nMaxIQp;
    AX_S32 nIntraQpDelta;
} RC_INFO_T;
typedef struct _stVideoConfig
{
    AX_PAYLOAD_TYPE_E ePayloadType;
    AX_U32 nGOP;
    AX_U32 nSrcFrameRate;
    AX_U32 nDstFrameRate;
    AX_U32 nStride;
    AX_S32 nInWidth;
    AX_S32 nInHeight;
    AX_S32 nOutWidth;
    AX_S32 nOutHeight;
    AX_S32 nOffsetCropX;
    AX_S32 nOffsetCropY;
    AX_S32 nOffsetCropW;
    AX_S32 nOffsetCropH;
    AX_IMG_FORMAT_E eImgFormat;
    RC_INFO_T stRCInfo;
    AX_S32 nBitrate;
} VIDEO_CONFIG_T;

typedef hal_buffer_t Venc_Frame;
typedef hal_buffer_t venc_buffer_t;


typedef enum
{
    AX_VENC_CHN_NONT = 0,
    AX_VENC_CHN_CREATE,
    AX_VENC_CHN_START_RECV,
    AX_VENC_CHN_PTHREAD_LOOP,
    AX_VENC_CHN_PTHREAD_LOOP_STOP,
    AX_VENC_CHN_STOP_RECV,
} AX_VENC_CHN_S;

typedef struct{
	int status;
    int Chn;
    pthread_t farm_pthread_p;
    void (*out_farm)(Venc_Frame*);

    VIDEO_CONFIG_T config;
    AX_VENC_CHN_ATTR_S stVencChnAttr;
    AX_VENC_RECV_PIC_PARAM_S stRecvParam;

    AX_VENC_JPEG_PARAM_S stJpegParam;
    int jpeg_u32Qfactor;

}ax_venc_dev_info;
// typedef struct{
//     int pipeid;
//     int m_output_type;
//     int n_width, n_height, n_size, n_stride;
//     int d_type;
//     void* p_vir;
// 	unsigned long long int p_phy;
// 	unsigned long long int n_pts;
// 	void* payload;
// }Venc_Frame;

typedef struct ax_venc_hal_t{
    int ChnSize;
    ax_venc_dev_info dev[10];
    AX_VENC_MOD_ATTR_S stModAttr;
    int (*OpenChn)(struct ax_venc_hal_t*, int, int);
    int (*SetChnOut)(struct ax_venc_hal_t*, int, int, int, int, AX_PAYLOAD_TYPE_E, void (*out_farm)(Venc_Frame*));
    int (*on_farm)(struct ax_venc_hal_t*,int , Venc_Frame*);
    int (*set_farm_on)(struct ax_venc_hal_t*,int , Venc_Frame*);
    void (*start)(struct ax_venc_hal_t*, int);
    void (*stop)(struct ax_venc_hal_t*, int);
    int (*getStatus)(struct ax_venc_hal_t*, int);
    
    int (*EncodeOneFrameToJpeg)(struct ax_venc_hal_t*, void*, int* , AX_U32, AX_U32, AX_U32, AX_U64, void*, AX_U32);

    void (*set_Chn_mode_par[10])(ax_venc_dev_info *);
    void (*set_Chn_mode_par_after[10])(ax_venc_dev_info *);
}ax_venc_hal;

#ifdef __cplusplus
extern "C"{
#endif

int ax_create_venc(ax_venc_hal* venc_dev);
void ax_destroy_venc(ax_venc_hal* venc_dev);


#ifdef __cplusplus
}
#endif


#ifdef __cplusplus
class ax_venc_hal_cpp
{
private:
    bool exit_flage;
public:
    ax_venc_hal _dev;
    ax_venc_hal_cpp(/* args */)
    {
        exit_flage = true;
        ax_create_venc(&_dev);
    }
    int SetChnOut(int Chn, int w, int h, int fps, AX_PAYLOAD_TYPE_E out_type, void (*out_farm)(Venc_Frame *))
    {
        return _dev.SetChnOut(&_dev, Chn, w, h, fps, out_type, out_farm);
    }
    int OpenChn(int Chn, int mode)
    {
        return _dev.OpenChn(&_dev, Chn, mode);
    }
    int on_farm(int Chn, Venc_Frame *buffer)
    {
        return _dev.on_farm(&_dev, Chn, buffer);
    }

    void start(int Chn)
    {
        _dev.start(&_dev, Chn);
    }
    void exit()
    {
        if(exit_flage)
        {
            ax_destroy_venc(&_dev);
            exit_flage = false;
        }
    }

    ~ax_venc_hal_cpp()
    {
        if(exit_flage)
        {
            exit();
        }
    }
};

#endif



#endif /* __AX_VENC_HAL_H_ */
