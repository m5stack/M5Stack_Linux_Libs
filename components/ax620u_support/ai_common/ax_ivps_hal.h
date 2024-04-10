//
// Copyright (c) 2023-2025 dianjixz
//

#ifndef __AX_IVPS_HAL_H_
#define __AX_IVPS_HAL_H_


#include "sample_log.h"
#include "ax_ivps_api.h"
#include "common_codec/common_venc.h"
#include "my_hal.h"



typedef hal_buffer_t Venc_Frame;
typedef hal_buffer_t ivps_buffer_t;


typedef enum
{
    AX_IVPS_CHN_NONT = 0,
    AX_IVPS_CHN_CREATE,
    AX_IVPS_CHN_START_RECV,
    AX_IVPS_CHN_PTHREAD_LOOP,
    AX_IVPS_CHN_PTHREAD_LOOP_STOP,
    AX_IVPS_CHN_STOP_RECV,
} AX_IVPS_CHN_S;

typedef struct{
	int status;
    int Chn;
    int GRP_id;
    pthread_t farm_pthread_p;
    void (*out_farm)(Venc_Frame*);
    AX_IVPS_PIPELINE_ATTR_S stPipelineAttr;
    int n_osd_rgn;
    // int n_osd_rgn_chn[12]; // rgn 的句柄

}ax_ivps_dev_info;


typedef struct ax_ivps_hal_t{
    int GRP_id;
    AX_IVPS_GRP_ATTR_S stGrpAttr;
    AX_IVPS_PIPELINE_ATTR_S stPipelineAttr;

    int n_osd_rgn_chn[12];
    int n_osd_rgn_chn_hand[12];
    ax_ivps_dev_info dev[5];

    int status;
    // AX_IVPS_CreateGrp
    // AX_IVPS_SetPipelineAttr
    // ax_ivps_hal *self, int Chn, int mode
    int (*OpenChn)(void*, int, int);
    // // config
    // int (*SetChnOut)(void*, int, int, int, int, AX_PAYLOAD_TYPE_E, void (*out_farm)(void*));
    int (*on_farm)(void*,int , void*);


    int (*on_osd)(void*,int , void*);

    // ax_ivps_hal *self, int Chn, void (*out_farm)(void *)
    int (*set_farm_on)(void*,int , void*);
    // AX_IVPS_EbavleChn
    // AX_IVPS_StartGrp
    // ax_ivps_hal *self, int Chn
    void (*start)(void*, int);
    // AX_IVPS_DisableChn
    // AX_IVPS_DestoryGrp
    // ax_ivps_hal *self, int Chn
    void (*stop)(void*, int);

    int (*getStatus)(void*, int);
    
    void (*set_Chn_mode_par[10])(void *);
    void (*set_Chn_mode_par_after[10])(void *);
}ax_ivps_hal;

#ifdef __cplusplus
extern "C"{
#endif
// AX_IVPS_Init
int ax_create_ivps(ax_ivps_hal* ivps_dev);
// AX_IVPS_DEinit
void ax_destroy_ivps(ax_ivps_hal* ivps_dev);


#ifdef __cplusplus
}
#endif


#ifdef __cplusplus
class ax_ivps_hal_cpp
{
private:
    bool exit_flage;
public:
    ax_ivps_hal _dev;
    ax_ivps_hal_cpp()
    {
        ax_create_ivps(&_dev);
        exit_flage = true;
    }
    int OpenChn(int Chn, int mode)
    {
        return _dev.OpenChn(&_dev, Chn, mode);
    }
    void start(int Chn)
    {
        _dev.start(&_dev, Chn);
    }
    int get_grp_id()
    {
        return _dev.GRP_id;
    }


    void exit()
    {
        if(exit_flage)
        {
            ax_destroy_ivps(&_dev);
            exit_flage = false;
        }
    }
    ~ax_ivps_hal_cpp()
    {
        if(exit_flage)
        {
            exit();
        }
    }
};


#endif



#endif /* __AX_IVPS_HAL_H_ */
