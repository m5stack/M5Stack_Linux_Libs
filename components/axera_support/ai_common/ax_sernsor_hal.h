#ifndef __SENSOR_HAL_H__
#define __SENSOR_HAL_H__

#include <pthread.h>


#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <signal.h>
#include <errno.h>
#include <unistd.h>
#include <pthread.h>
#include <time.h>
#ifdef SAMPLE_IRCUT
#include <stdbool.h>
#endif
#include "ax_isp_api.h"
#include "common_sys.h"
#include "common_vin.h"
#include "common_cam.h"
#include "sample_log.h"
/* ir-cut */
#ifdef SAMPLE_IRCUT
#include <stdbool.h>
#ifndef ABS
#define ABS(a) (((a) < 0) ? -(a) : (a))
#endif
#endif

#include "my_hal.h"

/* tuning server */
#ifdef TUNING_CTRL
#include "ax_nt_stream_api.h"
#include "ax_nt_ctrl_api.h"
#endif




#define  LOAD_BIN_FILE_NAME                 "/opt/etc/thermal_lce_param.bin"

#define  OS04A10_SDR_620A_NO_PFR_BIN_FILE_NAME      "/opt/etc/os04a10_sdr_620a_no_pfr.bin"
#define  OS04A10_HDR_2X_620A_NO_PFR_BIN_FILE_NAME   "/opt/etc/os04a10_hdr_2x_620a_no_pfr.bin"
#define  OS04A10_SDR_620A_QUARTER_EIS_BIN_FILE_NAME      "/opt/etc/os04a10_sdr_quarter_eis.bin"
#define  OS04A10_HDR_2X_620A_QUARTER_EIS_BIN_FILE_NAME      "/opt/etc/os04a10_hdr_2x_quarter_eis.bin"

#define  OS04A10_SDR_620U_NO_PFR_BIN_FILE_NAME      "/opt/etc/os04a10_sdr_620u_no_pfr.bin"
#define  OS04A10_HDR_2X_620U_NO_PFR_BIN_FILE_NAME   "/opt/etc/os04a10_hdr_2x_620u_no_pfr.bin"

#define  SC530AI_SDR_620A_QUARTER_EIS_BIN_FILE_NAME      "/opt/etc/sc530ai_sdr_quarter_eis.bin"
#define  SC530AI_HDR_2X_620A_QUARTER_EIS_BIN_FILE_NAME      "/opt/etc/sc530ai_hdr_2x_quarter_eis.bin"

#define  IMX327_SDR_620A_QUARTER_EIS_BIN_FILE_NAME      "/opt/etc/imx327_sdr_quarter_eis.bin"
#define  IMX327_HDR_2X_620A_QUARTER_EIS_BIN_FILE_NAME      "/opt/etc/imx327_hdr_2x_quarter_eis.bin"

#define  IMX464_SDR_620A_QUARTER_EIS_BIN_FILE_NAME      "/opt/etc/imx464_sdr_quarter_eis.bin"
#define  IMX464_HDR_2X_620A_QUARTER_EIS_BIN_FILE_NAME      "/opt/etc/imx464_hdr_2x_quarter_eis.bin"




typedef enum {
    SYS_CASE_NONE  = -1,
    SYS_CASE_SINGLE_OS04A10  = 0,
    SYS_CASE_SINGLE_IMX334   = 1,
    SYS_CASE_SINGLE_GC4653   = 2,
    SYS_CASE_DUAL_OS04A10    = 3,
    SYS_CASE_SINGLE_OS08A20  = 4,
    SYS_CASE_SINGLE_OS04A10_ONLINE  = 5,
    SYS_CASE_SINGLE_DVP  = 6,
    SYS_CASE_SINGLE_BT601  = 7,
    SYS_CASE_SINGLE_BT656  = 8,
    SYS_CASE_SINGLE_BT1120  = 9,
    SYS_CASE_MIPI_YUV = 10,
    SYS_CASE_THERMAL = 11,
    SYS_CASE_THERMAL_AND_OS04A10 = 12,
    SYS_CASE_SINGLE_OS04A10_SPLITE_OFFLINE = 13,
    SYS_CASE_SINGLE_OS08A20_SPLITE_ONLINE = 14,
    SYS_CASE_SINGLE_YUV = 15,
    SYS_CASE_IMX327_IMX327_AND_YUV = 16,
    SYS_CASE_FOUR_SC230AI = 17,
    SYS_CASE_SINGLE_OV12D2Q = 18,
    SYS_CASE_OS04A10_AND_MIPITX422 = 19,
    SYS_CASE_DUAL_OS04A10_ONLINE = 20,
    SYS_CASE_SINGLE_SC530AI_ONLINE  = 21,
    SYS_CASE_SINGLE_IMX327_ONLINE   = 22,
    SYS_CASE_DUAL_IMX327_ONLINE_U = 23,
    SYS_CASE_TRIPLE_IMX327 = 24,
    SYS_CASE_FOUR_IMX327 = 25,
    SYS_CASE_SINGLE_GC4653_ONLINE_U   = 26,
    SYS_CASE_SINGLE_IMX464   = 27,
    SYS_CASE_SINGLE_IMX415   = 28,
    SYS_CASE_OS04A10_AND_YUV_ONLINE = 29,
    SYS_CASE_IMX327_IMX327_AND_YUV_ONLINE = 30,
    SYS_CASE_OS04A10_YUV422_RX = 31,
    SYS_CASE_SYNC_DUAL_OS04A10_PTS = 32,
    SYS_CASE_SYNC_DUAL_SC230AI_PTS = 33,
    SYS_CASE_SINGLE_OS04A10_DCG_HDR = 34,
    SYS_CASE_BUTT
} COMMON_SYS_CASE_E;


typedef enum
{
    AX_SENSOR_CHN_NONT = 0,
    AX_SENSOR_CHN_CREATE,
    AX_SENSOR_CHN_START_RECV,
    AX_SENSOR_CHN_PTHREAD_LOOP,
    AX_SENSOR_CHN_PTHREAD_LOOP_STOP,
    AX_SENSOR_CHN_STOP_RECV,
} AX_SENSOR_CHN_S;


typedef enum
{
    AX_SENSOR_NONT = 0,
    AX_SENSOR_AI_ISP,
    AX_SENSOR_SYS,
    AX_SENSOR_CAM_INIT,
    AX_SENSOR_CAM_OPEN,
    AX_SENSOR_TUNING_CTRL,
    AX_SENSOR_ISP_RUN
} AX_SENSOR_S;


typedef hal_buffer_t Sensor_Frame;


typedef struct{
	int status;
    int nPipeId;
    int Chn;
    pthread_t farm_pthread_p;
    void (*out_farm)(void*);
    AX_IMG_INFO_T ax_img;
    int sernsor_frame_rate;

    int fcnt ;
    int fps ;
    struct timespec ts1, ts2;


}ax_sensor_dev_info;

#define AX_MAX_SENSOR_CHN_ 3
#define AX_MAX_SENSOR_CHN_CONFIG_ 10
typedef struct ax_sensor_hal_t{
    int ChnSize;
    
    CAMERA_T gCams[MAX_CAMERAS];
    COMMON_SYS_CASE_E eSysCase;
    COMMON_SYS_ARGS_T tCommonArgs;
    AX_SNS_HDR_MODE_E eHdrMode;
    AX_SNS_HDR_DATA_OUT_MODE_E eSnsDataOutMode;
    AX_ISP_PIPE_SOURCE_E ePipeDataSrc;
    AX_NPU_SDK_EX_ATTR_T sNpuAttr;
    AX_BOOL infoTx;


    ax_sensor_dev_info dev[AX_MAX_SENSOR_CHN_];

    int status;
    int (*OpenSensor)(struct ax_sensor_hal_t*, int, int);
    int (*CloseSensor)(struct ax_sensor_hal_t*);

    int (*SetSensorOut)(struct ax_sensor_hal_t*, int, void (*out_farm)(void*));
    void (*start)(struct ax_sensor_hal_t*, int);
    void (*stop)(struct ax_sensor_hal_t*, int);
    int (*GetFrameRate)(struct ax_sensor_hal_t*, int);
    int (*getStatus)(struct ax_sensor_hal_t*, int);
    
    void (*set_Sensor_mode_par[AX_MAX_SENSOR_CHN_CONFIG_])(struct ax_sensor_hal_t *);
    void (*set_Sensor_mode_par_after[AX_MAX_SENSOR_CHN_CONFIG_])(struct ax_sensor_hal_t *);
}ax_sensor_hal;

#ifdef __cplusplus
extern "C"{
#endif

int ax_create_sensor(ax_sensor_hal* sensor_dev);
void ax_destroy_sensor(ax_sensor_hal* sensor_dev);


#ifdef __cplusplus
}
#endif


#ifdef __cplusplus
class ax_sernsor_hal_cpp
{
private:
    bool exit_flage;
public:
    ax_sensor_hal _dev;
    ax_sernsor_hal_cpp(/* args */)
    {
        ax_create_sensor(&_dev);
        exit_flage = true;
    }
    int OpenSensor(int sensor_id, int mode)
    {
        return _dev.OpenSensor(&_dev, sensor_id, mode);
    }
    int SetSensorOut(int Chn, void (*out_farm)(void *))
    {
        return _dev.SetSensorOut(&_dev, Chn, out_farm);
    }
    void start(int Chn)
    {
        _dev.start(&_dev, Chn);
    }

    void exit()
    {
        if(exit_flage)
        {
            ax_destroy_sensor(&_dev);
            exit_flage = false;
        }
    }
    ~ax_sernsor_hal_cpp()
    {
        if(exit_flage)
        {
            exit();
        }
    }
};

#endif





















#endif