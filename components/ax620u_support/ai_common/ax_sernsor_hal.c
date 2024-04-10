/**********************************************************************************
 *
 * Copyright (c) 2019-2020 Beijing AXera Technology Co., Ltd. All Rights Reserved.
 *
 * This source file is the property of Beijing AXera Technology Co., Ltd. and
 * may not be copied or distributed in any isomorphic form without the prior
 * written consent of Beijing AXera Technology Co., Ltd.
 *
 **********************************************************************************/
#include "ax_sernsor_hal.h"
#include "ax_isp_common.h"

#ifdef SAMPLE_620U_NAND
static COMMON_SYS_POOL_CFG_T gtSysCommPoolSingleOs04a10Sdru[] = {
    {2688, 1520, 2688, AX_FORMAT_BAYER_RAW_10BPP, 6}, /*vin raw10 use */
    {2688, 1520, 2688, AX_FORMAT_BAYER_RAW_16BPP, 6}, /*vin raw16 use */
    {2688, 1520, 2688, AX_YUV420_SEMIPLANAR, 2},      /*vin nv21/nv21 use */
};

static COMMON_SYS_POOL_CFG_T gtSysCommPoolSingleOs04a10OnlineHdru[] = {
    {2688, 1520, 2688, AX_FORMAT_BAYER_RAW_10BPP, 6}, /*vin raw10 use */
    {2688, 1520, 2688, AX_FORMAT_BAYER_RAW_16BPP, 6}, /*vin raw16 use */
    {2688, 1520, 2688, AX_YUV420_SEMIPLANAR, 3},      /*vin nv21/nv21 use */
};
#endif

#if 1
static COMMON_SYS_POOL_CFG_T gtSysCommPoolSingleOs04a10Sdr[] = {
    {2688, 1520, 2688, AX_FORMAT_BAYER_RAW_10BPP, 15}, /*vin raw10 use */
    {2688, 1520, 2688, AX_FORMAT_BAYER_RAW_16BPP, 4},  /*vin raw16 use */
    {2688, 1520, 2688, AX_YUV420_SEMIPLANAR, 5},       /*vin nv21/nv21 use */
    {1920, 1080, 1920, AX_YUV420_SEMIPLANAR, 4},
    {1280, 720, 1280, AX_YUV420_SEMIPLANAR, 4},
};
#else

static COMMON_SYS_POOL_CFG_T gtSysCommPoolSingleOs04a10Sdr[] = {
    {2688, 1520, 2688, AX_FORMAT_BAYER_RAW_10BPP, 6}, /*vin raw10 use */
    {2688, 1520, 2688, AX_FORMAT_BAYER_RAW_16BPP, 4}, /*vin raw16 use */
    {2688, 1520, 2688, AX_YUV420_SEMIPLANAR, 3},      /*vin nv21/nv21 use */
    {1920, 1080, 1920, AX_YUV420_SEMIPLANAR, 2},
    {720, 576, 720, AX_YUV420_SEMIPLANAR, 2},
};

#endif
static COMMON_SYS_POOL_CFG_T gtSysCommPoolSingleOs04a10OnlineHdr[] = {
    {2688, 1520, 2688, AX_FORMAT_BAYER_RAW_10BPP, 6}, /*vin raw10 use */
    {2688, 1520, 2688, AX_FORMAT_BAYER_RAW_16BPP, 4}, /*vin raw16 use */
    {2688, 1520, 2688, AX_YUV420_SEMIPLANAR, 3},      /*vin nv21/nv21 use */
    {1920, 1080, 1920, AX_YUV420_SEMIPLANAR, 2},
    {720, 576, 720, AX_YUV420_SEMIPLANAR, 2},
};

// static COMMON_SYS_POOL_CFG_T gtSysCommPoolSingleOs04a10OnlineSdr[] = {
//     {2688, 1520, 2688, AX_FORMAT_BAYER_RAW_10BPP, 3}, /*vin raw10 use */
//     {2688, 1520, 2688, AX_FORMAT_BAYER_RAW_16BPP, 4}, /*vin raw16 use */
//     {2688, 1520, 2688, AX_YUV420_SEMIPLANAR, 3},      /*vin nv21/nv21 use */
//     {1920, 1080, 1920, AX_YUV420_SEMIPLANAR, 2},
//     {1280, 720, 1280, AX_YUV420_SEMIPLANAR, 2},
// };

static COMMON_SYS_POOL_CFG_T gtSysCommPoolSingleOs04a10OnlineSdr[] = {
    {2688, 1520, 2688, AX_FORMAT_BAYER_RAW_10BPP, 3}, /*vin raw10 use */
    {2688, 1520, 2688, AX_FORMAT_BAYER_RAW_16BPP, 4}, /*vin raw16 use */
    {2688, 1520, 2688, AX_YUV420_SEMIPLANAR, 3},      /*vin nv21/nv21 use */
    {1280, 720, 1280, AX_YUV420_SEMIPLANAR, 2},
    {960, 540, 960, AX_YUV420_SEMIPLANAR, 2},
};


static COMMON_SYS_POOL_CFG_T gtSysCommPoolSingleOs04a10Hdr[] = {
    {2688, 1520, 2688, AX_FORMAT_BAYER_RAW_10BPP, 17}, /*vin raw10 use */
    {2688, 1520, 2688, AX_FORMAT_BAYER_RAW_16BPP, 5},  /*vin raw16 use */
    {2688, 1520, 2688, AX_YUV420_SEMIPLANAR, 6},       /*vin nv21/nv21 use */
    {1920, 1080, 1920, AX_YUV420_SEMIPLANAR, 6},
    {720, 576, 720, AX_YUV420_SEMIPLANAR, 6},
};

COMMON_SYS_POOL_CFG_T gtSysCommPoolSingleImx334Sdr[] = {

    {3840, 2160, 3840, AX_FORMAT_BAYER_RAW_12BPP, 15}, /*vin raw10 use */
    {3840, 2160, 3840, AX_FORMAT_BAYER_RAW_16BPP, 5},  /*vin raw16 use */
    {3840, 2160, 3840, AX_YUV420_SEMIPLANAR, 6},       /*vin nv21/nv21 use */
    {1920, 1080, 1920, AX_YUV420_SEMIPLANAR, 3},
    {960, 540, 960, AX_YUV420_SEMIPLANAR, 3},

};

COMMON_SYS_POOL_CFG_T gtSysCommPoolSingleImx334Hdr[] = {

    {3840, 2160, 3840, AX_FORMAT_BAYER_RAW_10BPP, 17}, /*vin raw10 use */
    {3840, 2160, 3840, AX_FORMAT_BAYER_RAW_16BPP, 5},  /*vin raw16 use */
    {3840, 2160, 3840, AX_YUV420_SEMIPLANAR, 6},       /*vin nv21/nv21 use */
    {1920, 1080, 1920, AX_YUV420_SEMIPLANAR, 6},
    {960, 540, 960, AX_YUV420_SEMIPLANAR, 6},

};

static COMMON_SYS_POOL_CFG_T gtSysCommPoolSingleGc4653Onlineu[] = {

    {2560, 1440, 2560, AX_FORMAT_BAYER_RAW_10BPP, 3}, /*vin raw10 use */
    {2560, 1440, 2560, AX_FORMAT_BAYER_RAW_16BPP, 4}, /*vin raw16 use */
    {2560, 1440, 2560, AX_YUV420_SEMIPLANAR, 2},      /*vin nv21/nv21 use */

};

static COMMON_SYS_POOL_CFG_T gtSysCommPoolSingleGc4653[] = {

    {2560, 1440, 2560, AX_FORMAT_BAYER_RAW_10BPP, 3}, /*vin raw10 use */
    {2560, 1440, 2560, AX_FORMAT_BAYER_RAW_16BPP, 4}, /*vin raw16 use */
    {2560, 1440, 2560, AX_YUV420_SEMIPLANAR, 2},      /*vin nv21/nv21 use */
    {1280, 720, 1280, AX_YUV420_SEMIPLANAR, 2},
    {640, 360, 640, AX_YUV420_SEMIPLANAR, 2},

};

static COMMON_SYS_POOL_CFG_T gtSysCommPoolDoubleOs04a10[] = {
    {2688, 1520, 2688, AX_FORMAT_BAYER_RAW_10BPP, 15 * 2}, /*vin raw10 use */
    {2688, 1520, 2688, AX_FORMAT_BAYER_RAW_16BPP, 5 * 2},  /*vin raw16 use */
    {2688, 1520, 2688, AX_YUV420_SEMIPLANAR, 6 * 2},       /*vin nv21/nv21 use */
    {1344, 760, 1344, AX_YUV420_SEMIPLANAR, 3 * 2},
    {1344, 760, 1344, AX_YUV420_SEMIPLANAR, 3 * 2},
};

COMMON_SYS_POOL_CFG_T gtSysCommPoolSingleOs08a20Sdr[] = {

    {3840, 2160, 3840, AX_FORMAT_BAYER_RAW_12BPP, 15}, /*vin raw10 use */
    {3840, 2160, 3840, AX_FORMAT_BAYER_RAW_16BPP, 5},  /*vin raw16 use */
    {3840, 2160, 3840, AX_YUV420_SEMIPLANAR, 6},       /*vin nv21/nv21 use */
    {1920, 1080, 1920, AX_YUV420_SEMIPLANAR, 3},
    {960, 540, 960, AX_YUV420_SEMIPLANAR, 3},

};

COMMON_SYS_POOL_CFG_T gtSysCommPoolSingleOs08a20Hdr[] = {

    {3840, 2160, 3840, AX_FORMAT_BAYER_RAW_10BPP, 17}, /*vin raw10 use */
    {3840, 2160, 3840, AX_FORMAT_BAYER_RAW_16BPP, 5},  /*vin raw16 use */
    {3840, 2160, 3840, AX_YUV420_SEMIPLANAR, 6},       /*vin nv21/nv21 use */
    {1920, 1080, 1920, AX_YUV420_SEMIPLANAR, 6},
    {960, 540, 960, AX_YUV420_SEMIPLANAR, 6},

};

COMMON_SYS_POOL_CFG_T gtSysCommPoolSingleOs08a20OnlineSdr[] = {

    {3840, 2160, 3840, AX_FORMAT_BAYER_RAW_12BPP, 3}, /*vin raw10 use */
    {3840, 2160, 3840, AX_FORMAT_BAYER_RAW_16BPP, 4}, /*vin raw16 use */
    {3840, 2160, 3840, AX_YUV420_SEMIPLANAR, 3},      /*vin nv21/nv21 use */
    {1920, 1080, 1920, AX_YUV420_SEMIPLANAR, 2},
    {960, 540, 960, AX_YUV420_SEMIPLANAR, 2},

};

COMMON_SYS_POOL_CFG_T gtSysCommPoolSingleOs08a20OnlineHdr[] = {

    {3840, 2160, 3840, AX_FORMAT_BAYER_RAW_10BPP, 6}, /*vin raw10 use */
    {3840, 2160, 3840, AX_FORMAT_BAYER_RAW_16BPP, 4}, /*vin raw16 use */
    {3840, 2160, 3840, AX_YUV420_SEMIPLANAR, 3},      /*vin nv21/nv21 use */
    {1920, 1080, 1920, AX_YUV420_SEMIPLANAR, 2},
    {960, 540, 960, AX_YUV420_SEMIPLANAR, 2},

};

COMMON_SYS_POOL_CFG_T gtSysCommPoolSingleOs08a20Hdr_SplitMode[] = {

    {3840, 2160, 3840, AX_FORMAT_BAYER_RAW_10BPP, 17}, /*vin raw10 use */
    {3840, 2160, 3840, AX_FORMAT_BAYER_RAW_16BPP, 7},  /*vin raw16 use */
    {3840, 2160, 3840, AX_YUV420_SEMIPLANAR, 8},       /*vin nv21/nv21 use */
    {1920, 1080, 1920, AX_YUV420_SEMIPLANAR, 6},
    {960, 540, 960, AX_YUV420_SEMIPLANAR, 6},

};

static COMMON_SYS_POOL_CFG_T gtSysCommPoolSingleDVP[] = {
    {1600, 300, 1600, AX_FORMAT_BAYER_RAW_8BPP, 40},  /*vin raw8 use */
    {1600, 300, 1600, AX_FORMAT_BAYER_RAW_16BPP, 5},  /*vin raw16 use */
    {1600, 300, 1600, AX_YUV422_INTERLEAVED_UYVY, 6}, /*vin nv21/nv21 use */
};

static COMMON_SYS_POOL_CFG_T gtSysCommPoolBT601[] = {
    {2688, 1520, 2688, AX_FORMAT_BAYER_RAW_10BPP, 40}, /*vin raw10 use */
    {2688, 1520, 2688, AX_FORMAT_BAYER_RAW_16BPP, 5},  /*vin raw16 use */
    {2688, 1520, 2688, AX_YUV422_INTERLEAVED_YUYV, 6}, /*vin nv21/nv21 use */
    {1920, 1080, 1920, AX_YUV422_INTERLEAVED_YUYV, 3},
    {1280, 720, 1280, AX_YUV422_INTERLEAVED_YUYV, 3},
};
static COMMON_SYS_POOL_CFG_T gtSysCommPoolBT656[] = {
    {2688, 1520, 2688, AX_FORMAT_BAYER_RAW_10BPP, 40}, /*vin raw10 use */
    {2688, 1520, 2688, AX_FORMAT_BAYER_RAW_16BPP, 5},  /*vin raw16 use */
    {2688, 1520, 2688, AX_YUV422_INTERLEAVED_YUYV, 6}, /*vin nv21/nv21 use */
    {1920, 1080, 1920, AX_YUV422_INTERLEAVED_YUYV, 3},
    {1280, 720, 1280, AX_YUV422_INTERLEAVED_YUYV, 3},
};
static COMMON_SYS_POOL_CFG_T gtSysCommPoolBT1120[] = {
    {2688, 1520, 2688, AX_FORMAT_BAYER_RAW_10BPP, 40}, /*vin raw10 use */
    {2688, 1520, 2688, AX_FORMAT_BAYER_RAW_16BPP, 5},  /*vin raw16 use */
    {2688, 1520, 2688, AX_YUV422_INTERLEAVED_YUYV, 6}, /*vin nv21/nv21 use */
    {1920, 1080, 1920, AX_YUV422_INTERLEAVED_YUYV, 3},
    {1280, 720, 1280, AX_YUV422_INTERLEAVED_YUYV, 3},
};

static COMMON_SYS_POOL_CFG_T gtSysCommPoolMIPI_YUV[] = {
    {3840, 2160, 3840, AX_FORMAT_BAYER_RAW_16BPP, 40}, /*vin raw16 use */
};

static COMMON_SYS_POOL_CFG_T gtSysCommPoolSingleThermal[] = {
    {400 * 2, 300, 400 * 2, AX_FORMAT_BAYER_RAW_16BPP, 6},       /*vin raw16 use */
    {400, 300, 400, AX_YUV422_INTERLEAVED_YUYV, 12},             /*vin nv21/nv21 use */
    {400 * 2 * 4, 300, 400 * 2, AX_YUV422_INTERLEAVED_YUYV, 48}, /*vin nv21/nv21 use */
};

static COMMON_SYS_POOL_CFG_T gtSysCommPoolThermalAndOs04a10[] = {
    /* thermal common pool */
    {400 * 2, 300, 400 * 2, AX_FORMAT_BAYER_RAW_16BPP, 6},       /*vin raw16 use */
    {400, 300, 400, AX_YUV422_INTERLEAVED_YUYV, 12},             /*vin nv21/nv21 use */
    {400 * 2 * 4, 300, 400 * 2, AX_YUV422_INTERLEAVED_YUYV, 48}, /*vin nv21/nv21 use */

    /* os04a10 common pool */
    {2688, 1520, 2688, AX_FORMAT_BAYER_RAW_10BPP, 15}, /*vin raw10 use */
    {2688, 1520, 2688, AX_FORMAT_BAYER_RAW_16BPP, 4},  /*vin raw16 use */
    {2688, 1520, 2688, AX_YUV420_SEMIPLANAR, 5},       /*vin nv21/nv21 use */
    {1920, 1080, 1920, AX_YUV420_SEMIPLANAR, 4},
    {1280, 720, 1280, AX_YUV420_SEMIPLANAR, 4},

};

static COMMON_SYS_POOL_CFG_T gtSysCommPoolMIPI_AHD_YUV[] = {
    {1920, 1080, 1920, AX_FORMAT_BAYER_RAW_16BPP, 40}, /*vin raw16 use */
};

static COMMON_SYS_POOL_CFG_T gtSysCommPoolMIPI_OS04A10_RX_YUV[] = {
    {2688, 1520, 2688, AX_FORMAT_BAYER_RAW_16BPP, 20}, /*vin YUV422 use */
    {2688, 1520, 2688, AX_YUV420_SEMIPLANAR, 6},       /*vin nv21/nv21 use */
    {1920, 1080, 1920, AX_YUV420_SEMIPLANAR, 3},
    {1280, 720, 1280, AX_YUV420_SEMIPLANAR, 3},
};

static COMMON_SYS_POOL_CFG_T gtSysCommPoolIMX327_IMX327_AHDYUV[] = {
    /* imx327 common pool */
    {1920, 1080, 1920, AX_FORMAT_BAYER_RAW_16BPP, 40}, /*vin raw16 use */
};

static COMMON_SYS_POOL_CFG_T gtSysCommPoolFourSC230AI[] = {
    {1920, 1080, 1920, AX_FORMAT_BAYER_RAW_10BPP, 10 * 4}, /*vin raw10 use */
    {1920, 1080, 1920, AX_FORMAT_BAYER_RAW_16BPP, 7 * 4},  /*vin raw16 use */
    {1920, 1080, 1920, AX_YUV420_SEMIPLANAR, 7 * 4},       /*vin nv21/nv21 use */
    {1280, 720, 1280, AX_YUV420_SEMIPLANAR, 6 * 4},
    {1280, 720, 1280, AX_YUV420_SEMIPLANAR, 6 * 4},
};

static COMMON_SYS_POOL_CFG_T gtSysCommPoolDualSC230AI[] = {
    {1920, 1080, 1920, AX_FORMAT_BAYER_RAW_10BPP, 8}, /*vin raw10 use*/
    {1920, 1080, 1920, AX_FORMAT_BAYER_RAW_16BPP, 6}, /*vin raw16 use */
    {1920, 1080, 1920, AX_YUV420_SEMIPLANAR, 12},     /*vin nv21/nv21 use */
    {1280, 720, 1280, AX_YUV420_SEMIPLANAR, 12},
    {640, 320, 640, AX_YUV420_SEMIPLANAR, 12},
};

COMMON_SYS_POOL_CFG_T gtSysCommPoolSingleOv12d2qSdr[] = {

    {3840, 2160, 3840, AX_FORMAT_BAYER_RAW_10BPP, 15}, /*vin raw10 use */
    {3840, 2160, 3840, AX_FORMAT_BAYER_RAW_16BPP, 5},  /*vin raw16 use */
    {3840, 2160, 3840, AX_YUV420_SEMIPLANAR, 6},       /*vin nv21/nv21 use */
    {1920, 1080, 1920, AX_YUV420_SEMIPLANAR, 3},
    {960, 540, 960, AX_YUV420_SEMIPLANAR, 3},

};

COMMON_SYS_POOL_CFG_T gtSysCommPoolSingleOv12d2qHdr[] = {

    {3840, 2160, 3840, AX_FORMAT_BAYER_RAW_10BPP, 17}, /*vin raw10 use */
    {3840, 2160, 3840, AX_FORMAT_BAYER_RAW_16BPP, 5},  /*vin raw16 use */
    {3840, 2160, 3840, AX_YUV420_SEMIPLANAR, 6},       /*vin nv21/nv21 use */
    {1920, 1080, 1920, AX_YUV420_SEMIPLANAR, 6},
    {960, 540, 960, AX_YUV420_SEMIPLANAR, 6},

};

static COMMON_SYS_POOL_CFG_T gtSysCommPoolSingleSc530aiOnlineSdr[] = {
    {2880, 1620, 2880, AX_FORMAT_BAYER_RAW_10BPP, 3}, /*vin raw10 use */
    {2880, 1620, 2880, AX_FORMAT_BAYER_RAW_16BPP, 4}, /*vin raw16 use */
    {2880, 1620, 2880, AX_YUV420_SEMIPLANAR, 3},      /*vin nv21/nv21 use */
    {1920, 1080, 1920, AX_YUV420_SEMIPLANAR, 2},
    {1280, 720, 1280, AX_YUV420_SEMIPLANAR, 2},
};

static COMMON_SYS_POOL_CFG_T gtSysCommPoolSingleSc530aiOnlineHdr[] = {
    {2880, 1620, 2880, AX_FORMAT_BAYER_RAW_10BPP, 6}, /*vin raw10 use */
    {2880, 1620, 2880, AX_FORMAT_BAYER_RAW_16BPP, 4}, /*vin raw16 use */
    {2880, 1620, 2880, AX_YUV420_SEMIPLANAR, 3},      /*vin nv21/nv21 use */
    {1920, 1080, 1920, AX_YUV420_SEMIPLANAR, 2},
    {1280, 720, 1280, AX_YUV420_SEMIPLANAR, 2},
};

static COMMON_SYS_POOL_CFG_T gtSysCommPoolSingleImx327OnlineSdr[] = {
    {1920, 1080, 1920, AX_FORMAT_BAYER_RAW_12BPP, 3}, /*vin raw12 use */
    {1920, 1080, 1920, AX_FORMAT_BAYER_RAW_16BPP, 4}, /*vin raw16 use */
    {1920, 1080, 1920, AX_YUV420_SEMIPLANAR, 3},      /*vin nv21/nv21 use */
    {960, 540, 960, AX_YUV420_SEMIPLANAR, 2},
    {480, 270, 480, AX_YUV420_SEMIPLANAR, 2},
};

static COMMON_SYS_POOL_CFG_T gtSysCommPoolSingleImx327OnlineHdr[] = {
    {1920, 1080, 1920, AX_FORMAT_BAYER_RAW_10BPP, 6}, /*vin raw10 use */
    {1920, 1080, 1920, AX_FORMAT_BAYER_RAW_16BPP, 4}, /*vin raw16 use */
    {1920, 1080, 1920, AX_YUV420_SEMIPLANAR, 3},      /*vin nv21/nv21 use */
    {960, 540, 960, AX_YUV420_SEMIPLANAR, 2},
    {480, 270, 480, AX_YUV420_SEMIPLANAR, 2},
};
#ifdef SAMPLE_620U_NAND
static COMMON_SYS_POOL_CFG_T gtSysCommPoolSingleImx327OnlineSdru[] = {
    {1920, 1080, 1920, AX_FORMAT_BAYER_RAW_12BPP, 6}, /*vin raw12 use */
    {1920, 1080, 1920, AX_FORMAT_BAYER_RAW_16BPP, 6}, /*vin raw16 use */
    {1920, 1080, 1920, AX_YUV420_SEMIPLANAR, 2},      /*vin nv21/nv21 use */
};

static COMMON_SYS_POOL_CFG_T gtSysCommPoolSingleImx327OnlineHdru[] = {
    {1920, 1080, 1920, AX_FORMAT_BAYER_RAW_10BPP, 6}, /*vin raw10 use */
    {1920, 1080, 1920, AX_FORMAT_BAYER_RAW_16BPP, 6}, /*vin raw16 use */
    {1920, 1080, 1920, AX_YUV420_SEMIPLANAR, 3},      /*vin nv21/nv21 use */
};
#endif
static COMMON_SYS_POOL_CFG_T gtSysCommPoolDoubleImx327OnlineSdru[] = {
    {1920, 1080, 1920, AX_FORMAT_BAYER_RAW_12BPP, 6 * 2}, /*vin raw10 use */
    {1920, 1080, 1920, AX_FORMAT_BAYER_RAW_16BPP, 6 * 2}, /*vin raw16 use */
    {1920, 1080, 1920, AX_YUV420_SEMIPLANAR, 2 * 2},      /*vin nv21/nv21 use */
};

static COMMON_SYS_POOL_CFG_T gtSysCommPoolDoubleImx327OnlineHdru[] = {
    {1920, 1080, 1920, AX_FORMAT_BAYER_RAW_10BPP, 6 * 2}, /*vin raw10 use */
    {1920, 1080, 1920, AX_FORMAT_BAYER_RAW_16BPP, 6 * 2}, /*vin raw16 use */
    {1920, 1080, 1920, AX_YUV420_SEMIPLANAR, 3 * 2},      /*vin nv21/nv21 use */
};

static COMMON_SYS_POOL_CFG_T gtSysCommPoolIMX327TripleSdr[] = {
    /* imx327 common pool */
    {1920, 1080, 1920, AX_FORMAT_BAYER_RAW_16BPP, 40}, /*vin raw16 use */
};

static COMMON_SYS_POOL_CFG_T gtSysCommPoolFourIMX327[] = {
    {1920, 1080, 1920, AX_FORMAT_BAYER_RAW_12BPP, 10 * 4}, /*vin raw12 use */
    {1920, 1080, 1920, AX_FORMAT_BAYER_RAW_16BPP, 7 * 4},  /*vin raw16 use */
    {1920, 1080, 1920, AX_YUV420_SEMIPLANAR, 7 * 4},       /*vin nv21/nv21 use */
    {960, 540, 960, AX_YUV420_SEMIPLANAR, 6 * 4},
    {480, 270, 480, AX_YUV420_SEMIPLANAR, 6 * 4},
};

static COMMON_SYS_POOL_CFG_T gtSysCommPoolSingleImx464Sdr[] = {
    {2688, 1520, 2688, AX_FORMAT_BAYER_RAW_12BPP, 15}, /*vin raw12 use */
    {2688, 1520, 2688, AX_FORMAT_BAYER_RAW_16BPP, 4},  /*vin raw16 use */
    {2688, 1520, 2688, AX_YUV420_SEMIPLANAR, 5},       /*vin nv21/nv21 use */
    {1920, 1080, 1920, AX_YUV420_SEMIPLANAR, 4},
    {1280, 720, 1280, AX_YUV420_SEMIPLANAR, 4},
};

static COMMON_SYS_POOL_CFG_T gtSysCommPoolSingleImx464Hdr[] = {
    {2688, 1520, 2688, AX_FORMAT_BAYER_RAW_12BPP, 17}, /*vin raw12 use */
    {2688, 1520, 2688, AX_FORMAT_BAYER_RAW_16BPP, 5},  /*vin raw16 use */
    {2688, 1520, 2688, AX_YUV420_SEMIPLANAR, 6},       /*vin nv21/nv21 use */
    {1920, 1080, 1920, AX_YUV420_SEMIPLANAR, 6},
    {720, 576, 720, AX_YUV420_SEMIPLANAR, 6},
};
static COMMON_SYS_POOL_CFG_T gtSysCommPoolSingleImx464OnlineHdr[] = {
    {2688, 1520, 2688, AX_FORMAT_BAYER_RAW_12BPP, 6}, /*vin raw12 use */
    {2688, 1520, 2688, AX_FORMAT_BAYER_RAW_16BPP, 4}, /*vin raw16 use */
    {2688, 1520, 2688, AX_YUV420_SEMIPLANAR, 3},      /*vin nv21/nv21 use */
    {1920, 1080, 1920, AX_YUV420_SEMIPLANAR, 2},
    {720, 576, 720, AX_YUV420_SEMIPLANAR, 2},
};

static COMMON_SYS_POOL_CFG_T gtSysCommPoolSingleImx464OnlineSdr[] = {
    {2688, 1520, 2688, AX_FORMAT_BAYER_RAW_12BPP, 3}, /*vin raw10 use */
    {2688, 1520, 2688, AX_FORMAT_BAYER_RAW_16BPP, 4}, /*vin raw16 use */
    {2688, 1520, 2688, AX_YUV420_SEMIPLANAR, 3},      /*vin nv21/nv21 use */
    {1920, 1080, 1920, AX_YUV420_SEMIPLANAR, 2},
    {1280, 720, 1280, AX_YUV420_SEMIPLANAR, 2},
};

static COMMON_SYS_POOL_CFG_T gtSysCommPoolSingleImx415Sdru[] = {

    {3864, 2164, 3864, AX_FORMAT_BAYER_RAW_10BPP, 6}, /*vin raw10 use */
    {3840, 2160, 3840, AX_FORMAT_BAYER_RAW_16BPP, 6}, /*vin raw16 use */
    {3840, 2160, 3840, AX_YUV420_SEMIPLANAR, 2},      /*vin nv21/nv21 use */
};

static COMMON_SYS_POOL_CFG_T gtSysCommPoolSingleImx415Hdru[] = {

    {3864, 2164, 3864, AX_FORMAT_BAYER_RAW_10BPP, 6}, /*vin raw10 use */
    {3840, 2160, 3840, AX_FORMAT_BAYER_RAW_16BPP, 6}, /*vin raw16 use */
    {3840, 2160, 3840, AX_YUV420_SEMIPLANAR, 3},      /*vin nv21/nv21 use */

};

static COMMON_SYS_POOL_CFG_T gtSysCommPoolSingleImx415OnlineSdru[] = {

    {3864, 2164, 3864, AX_FORMAT_BAYER_RAW_10BPP, 3}, /*vin raw10 use */
    {3840, 2160, 3840, AX_FORMAT_BAYER_RAW_16BPP, 4}, /*vin raw16 use */
    {3840, 2160, 3840, AX_YUV420_SEMIPLANAR, 3},      /*vin nv21/nv21 use */

};

static COMMON_SYS_POOL_CFG_T gtSysCommPoolSingleImx415OnlineHdru[] = {

    {3864, 2164, 3864, AX_FORMAT_BAYER_RAW_10BPP, 6}, /*vin raw10 use */
    {3840, 2160, 3840, AX_FORMAT_BAYER_RAW_16BPP, 4}, /*vin raw16 use */
    {3840, 2160, 3840, AX_YUV420_SEMIPLANAR, 3},      /*vin nv21/nv21 use */

};

static COMMON_SYS_POOL_CFG_T gtSysCommPoolOs04a10AndAhd[] = {
    /* ahd common pool */
    {1920, 1080, 1920, AX_YUV422_INTERLEAVED_UYVY, 6}, /*vin yuv422 use */
    {1920, 1080, 1920, AX_YUV420_SEMIPLANAR, 4},       /*vin nv21/nv21 use */
    {1920, 1080, 1920, AX_YUV420_SEMIPLANAR, 4},
    {720, 480, 720, AX_YUV420_SEMIPLANAR, 4},

    /* os04a10 common pool */
    {2688, 1520, 2688, AX_FORMAT_BAYER_RAW_10BPP, 7}, /*vin raw10 use */
    {2688, 1520, 2688, AX_FORMAT_BAYER_RAW_16BPP, 5}, /*vin raw16 use */
    {2688, 1520, 2688, AX_YUV420_SEMIPLANAR, 4},      /*vin nv21/nv21 use */
    {1920, 1080, 1920, AX_YUV420_SEMIPLANAR, 4},
    {720, 576, 720, AX_YUV420_SEMIPLANAR, 4},
};

static COMMON_SYS_POOL_CFG_T gtSysCommPoolDoubleOnlineOs04a10[] = {
    {2688, 1520, 2688, AX_FORMAT_BAYER_RAW_10BPP, 8 * 2}, /*vin raw10 use */
    {2688, 1520, 2688, AX_FORMAT_BAYER_RAW_16BPP, 5 * 2}, /*vin raw16 use */
    {2688, 1520, 2688, AX_YUV420_SEMIPLANAR, 6 * 2},      /*vin nv21/nv21 use */
    {1344, 760, 1344, AX_YUV420_SEMIPLANAR, 3 * 2},
    {1344, 760, 1344, AX_YUV420_SEMIPLANAR, 3 * 2},
};


static void sernsor_calculateFrameRate(ax_sensor_dev_info *self)
{
    self->fcnt ++;
    clock_gettime(CLOCK_MONOTONIC, &self->ts2);
    if ((self->ts2.tv_sec * 1000 + self->ts2.tv_nsec / 1000000) - (self->ts1.tv_sec * 1000 + self->ts1.tv_nsec / 1000000) >= 1000)
    {
        self->fps = self->fcnt;
        self->ts1 = self->ts2;
        self->fcnt = 0;
        self->sernsor_frame_rate = self->fps;
    }
}



static void ax_sernsor_cam_init(struct ax_sensor_hal_t *self)
{
    /* Currently, only a single camera is supported beyond fsc mode*/
    self->gCams[0].eSnsDataOutMode = self->eSnsDataOutMode;

    self->gCams[0].ePipeDataSrc = self->ePipeDataSrc;

    self->gCams[0].run_mode = AX_ISP_PIPELINE_NORMAL;
    self->gCams[1].run_mode = AX_ISP_PIPELINE_NORMAL;

    if (self->eSysCase == SYS_CASE_SINGLE_OS04A10)
    {
        self->tCommonArgs.nCamCnt = 1;
        self->gCams[0].eSnsType = OMNIVISION_OS04A10;
        COMMON_ISP_GetSnsConfig(OMNIVISION_OS04A10, &self->gCams[0].stSnsAttr, &self->gCams[0].stSnsClkAttr, &self->gCams[0].stDevAttr,
                                &self->gCams[0].stPipeAttr, &self->gCams[0].stChnAttr);
#ifdef SAMPLE_IRCUT
        COMMON_ISP_GetIrCutAttr(OMNIVISION_OS04A10, &self->gCams[0].stSnsIrAttr);
#endif
        if (self->eHdrMode == AX_SNS_LINEAR_MODE)
        {
            self->tCommonArgs.nPoolCfgCnt = sizeof(gtSysCommPoolSingleOs04a10Sdr) / sizeof(gtSysCommPoolSingleOs04a10Sdr[0]);
            self->tCommonArgs.pPoolCfg = gtSysCommPoolSingleOs04a10Sdr;
        }
        else if (self->eHdrMode == AX_SNS_HDR_2X_MODE)
        {
            self->tCommonArgs.nPoolCfgCnt = sizeof(gtSysCommPoolSingleOs04a10Hdr) / sizeof(gtSysCommPoolSingleOs04a10Hdr[0]);
            self->tCommonArgs.pPoolCfg = gtSysCommPoolSingleOs04a10Hdr;
        }
        self->gCams[0].stPipeAttr.ePipeDataSrc = AX_PIPE_SOURCE_DEV_ONLINE;
        self->gCams[0].stSnsAttr.nFrameRate = 30;
        self->gCams[0].stChnAttr.tChnAttr[1].bEnable = AX_TRUE;
        self->gCams[0].stChnAttr.tChnAttr[0].bEnable = AX_TRUE;
        self->gCams[0].stChnAttr.tChnAttr[2].bEnable = AX_TRUE;
    }
    else if (self->eSysCase == SYS_CASE_SINGLE_OS04A10_ONLINE)
    {
        self->tCommonArgs.nCamCnt = 1;
        self->gCams[0].eSnsType = OMNIVISION_OS04A10;
        COMMON_ISP_GetSnsConfig(OMNIVISION_OS04A10, &self->gCams[0].stSnsAttr, &self->gCams[0].stSnsClkAttr, &self->gCams[0].stDevAttr,
                                &self->gCams[0].stPipeAttr, &self->gCams[0].stChnAttr);
#ifdef SAMPLE_IRCUT
        COMMON_ISP_GetIrCutAttr(OMNIVISION_OS04A10, &self->gCams[0].stSnsIrAttr);
#endif
#ifdef SAMPLE_620U_NAND
        if (self->eHdrMode == AX_SNS_LINEAR_MODE)
        {
            self->tCommonArgs.nPoolCfgCnt = sizeof(gtSysCommPoolSingleOs04a10Sdru) / sizeof(gtSysCommPoolSingleOs04a10Sdru[0]);
            self->tCommonArgs.pPoolCfg = gtSysCommPoolSingleOs04a10Sdru;
        }
        else if (self->eHdrMode == AX_SNS_HDR_2X_MODE)
        {
            self->tCommonArgs.nPoolCfgCnt = sizeof(gtSysCommPoolSingleOs04a10OnlineHdru) / sizeof(gtSysCommPoolSingleOs04a10OnlineHdru[0]);
            self->tCommonArgs.pPoolCfg = gtSysCommPoolSingleOs04a10OnlineHdru;
        }
#else
        if (self->eHdrMode == AX_SNS_LINEAR_MODE)
        {
            self->tCommonArgs.nPoolCfgCnt = sizeof(gtSysCommPoolSingleOs04a10OnlineSdr) / sizeof(gtSysCommPoolSingleOs04a10OnlineSdr[0]);
            self->tCommonArgs.pPoolCfg = gtSysCommPoolSingleOs04a10OnlineSdr;
            snprintf(self->gCams[0].szTuningFileName, MAX_FILE_NAME_CHAR_SIZE, "%s", OS04A10_SDR_620A_QUARTER_EIS_BIN_FILE_NAME);
        }
        else if (self->eHdrMode == AX_SNS_HDR_2X_MODE)
        {
            self->tCommonArgs.nPoolCfgCnt = sizeof(gtSysCommPoolSingleOs04a10OnlineHdr) / sizeof(gtSysCommPoolSingleOs04a10OnlineHdr[0]);
            self->tCommonArgs.pPoolCfg = gtSysCommPoolSingleOs04a10OnlineHdr;
            snprintf(self->gCams[0].szTuningFileName, MAX_FILE_NAME_CHAR_SIZE, "%s", OS04A10_HDR_2X_620A_QUARTER_EIS_BIN_FILE_NAME);
        }
#endif
        self->gCams[0].stPipeAttr.ePipeDataSrc = AX_PIPE_SOURCE_DEV_ONLINE;
        self->gCams[0].stChnAttr.tChnAttr[0].nDepth = 1;
        self->gCams[0].stChnAttr.tChnAttr[1].nDepth = 1;
        self->gCams[0].stChnAttr.tChnAttr[2].nDepth = 1;
    }
    else if (self->eSysCase == SYS_CASE_SINGLE_IMX334)
    {
        self->tCommonArgs.nCamCnt = 1;
        self->gCams[0].eSnsType = SONY_IMX334;
        COMMON_ISP_GetSnsConfig(SONY_IMX334, &self->gCams[0].stSnsAttr, &self->gCams[0].stSnsClkAttr, &self->gCams[0].stDevAttr,
                                &self->gCams[0].stPipeAttr,
                                &self->gCams[0].stChnAttr);
        if (self->eHdrMode == AX_SNS_LINEAR_MODE)
        {
            self->tCommonArgs.nPoolCfgCnt = sizeof(gtSysCommPoolSingleImx334Sdr) / sizeof(gtSysCommPoolSingleImx334Sdr[0]);
            self->tCommonArgs.pPoolCfg = gtSysCommPoolSingleImx334Sdr;
            self->gCams[0].stSnsAttr.eRawType = AX_RT_RAW12;
            self->gCams[0].stDevAttr.ePixelFmt = AX_FORMAT_BAYER_RAW_12BPP;
            self->gCams[0].stPipeAttr.ePixelFmt = AX_FORMAT_BAYER_RAW_12BPP;
        }
        else
        {
            self->tCommonArgs.nPoolCfgCnt = sizeof(gtSysCommPoolSingleImx334Hdr) / sizeof(gtSysCommPoolSingleImx334Hdr[0]);
            self->tCommonArgs.pPoolCfg = gtSysCommPoolSingleImx334Hdr;
        }
    }
    else if (self->eSysCase == SYS_CASE_SINGLE_GC4653)
    {
        self->tCommonArgs.nCamCnt = 1;
        self->gCams[0].eSnsType = GALAXYCORE_GC4653;
        self->tCommonArgs.nPoolCfgCnt = sizeof(gtSysCommPoolSingleGc4653) / sizeof(gtSysCommPoolSingleGc4653[0]);
        self->tCommonArgs.pPoolCfg = gtSysCommPoolSingleGc4653;
        COMMON_ISP_GetSnsConfig(GALAXYCORE_GC4653, &self->gCams[0].stSnsAttr, &self->gCams[0].stSnsClkAttr, &self->gCams[0].stDevAttr,
                                &self->gCams[0].stPipeAttr,
                                &self->gCams[0].stChnAttr);
    }
    else if (self->eSysCase == SYS_CASE_DUAL_OS04A10)
    {
        self->tCommonArgs.nCamCnt = 2;
        self->gCams[0].eSnsType = OMNIVISION_OS04A10;
        self->gCams[1].eSnsType = OMNIVISION_OS04A10;
        COMMON_ISP_GetSnsConfig(OMNIVISION_OS04A10, &self->gCams[0].stSnsAttr, &self->gCams[0].stSnsClkAttr, &self->gCams[0].stDevAttr,
                                &self->gCams[0].stPipeAttr,
                                &self->gCams[0].stChnAttr);
        COMMON_ISP_GetSnsConfig(OMNIVISION_OS04A10, &self->gCams[1].stSnsAttr, &self->gCams[1].stSnsClkAttr, &self->gCams[1].stDevAttr,
                                &self->gCams[1].stPipeAttr,
                                &self->gCams[1].stChnAttr);
        self->tCommonArgs.nPoolCfgCnt = sizeof(gtSysCommPoolDoubleOs04a10) / sizeof(gtSysCommPoolDoubleOs04a10[0]);
        self->tCommonArgs.pPoolCfg = gtSysCommPoolDoubleOs04a10;

        self->gCams[0].stSnsClkAttr.nSnsClkIdx = 0; /* mclk0 only by AX DEMO board, User defined */
        self->gCams[1].stSnsClkAttr.nSnsClkIdx = 2; /* mclk2 only by AX DEMO board, User defined */
    }
    else if (self->eSysCase == SYS_CASE_DUAL_OS04A10_ONLINE)
    {
        self->tCommonArgs.nCamCnt = 2;
        self->gCams[0].eSnsType = OMNIVISION_OS04A10;
        self->gCams[1].eSnsType = OMNIVISION_OS04A10;
        COMMON_ISP_GetSnsConfig(OMNIVISION_OS04A10, &self->gCams[0].stSnsAttr, &self->gCams[0].stSnsClkAttr, &self->gCams[0].stDevAttr,
                                &self->gCams[0].stPipeAttr,
                                &self->gCams[0].stChnAttr);
        COMMON_ISP_GetSnsConfig(OMNIVISION_OS04A10, &self->gCams[1].stSnsAttr, &self->gCams[1].stSnsClkAttr, &self->gCams[1].stDevAttr,
                                &self->gCams[1].stPipeAttr,
                                &self->gCams[1].stChnAttr);
        self->tCommonArgs.nPoolCfgCnt = sizeof(gtSysCommPoolDoubleOs04a10) / sizeof(gtSysCommPoolDoubleOs04a10[0]);
        self->tCommonArgs.pPoolCfg = gtSysCommPoolDoubleOs04a10;

        self->gCams[0].stPipeAttr.ePipeDataSrc = AX_PIPE_SOURCE_DEV_ONLINE;
        self->gCams[1].stPipeAttr.ePipeDataSrc = AX_PIPE_SOURCE_DEV_ONLINE;
        self->gCams[0].stPipeAttr.eDevSource = AX_DEV_SOURCE_SNS_0;
        self->gCams[1].stPipeAttr.eDevSource = AX_DEV_SOURCE_SNS_1;
        self->gCams[0].stPipeAttr.ePreOutput = AX_PRE_OUTPUT_FULL_MAIN;
        self->gCams[1].stPipeAttr.ePreOutput = AX_PRE_OUTPUT_LITE_MAIN;
        self->gCams[0].stSnsClkAttr.nSnsClkIdx = 0; /* mclk0 only by AX DEMO board, User defined */
        self->gCams[1].stSnsClkAttr.nSnsClkIdx = 1; /* mclk2 only by AX DEMO board, User defined */
    }
    else if (self->eSysCase == SYS_CASE_SINGLE_OS08A20)
    {
        self->tCommonArgs.nCamCnt = 1;
        self->gCams[0].eSnsType = OMNIVISION_OS08A20;
        COMMON_ISP_GetSnsConfig(OMNIVISION_OS08A20, &self->gCams[0].stSnsAttr, &self->gCams[0].stSnsClkAttr, &self->gCams[0].stDevAttr,
                                &self->gCams[0].stPipeAttr,
                                &self->gCams[0].stChnAttr);
        if (self->gCams[0].ePipeDataSrc == AX_PIPE_SOURCE_DEV_OFFLINE)
        {
            if (self->eHdrMode == AX_SNS_LINEAR_MODE)
            {
                self->tCommonArgs.nPoolCfgCnt = sizeof(gtSysCommPoolSingleOs08a20Sdr) / sizeof(gtSysCommPoolSingleOs08a20Sdr[0]);
                self->tCommonArgs.pPoolCfg = gtSysCommPoolSingleOs08a20Sdr;
                self->gCams[0].stDevAttr.ePixelFmt = AX_FORMAT_BAYER_RAW_12BPP;
                self->gCams[0].stPipeAttr.ePixelFmt = AX_FORMAT_BAYER_RAW_12BPP;
            }
            else
            {
                self->tCommonArgs.nPoolCfgCnt = sizeof(gtSysCommPoolSingleOs08a20Hdr) / sizeof(gtSysCommPoolSingleOs08a20Hdr[0]);
                self->tCommonArgs.pPoolCfg = gtSysCommPoolSingleOs08a20Hdr;
                self->gCams[0].stSnsAttr.eRawType = AX_RT_RAW10;
            }
        }
        else
        {
            if (self->eHdrMode == AX_SNS_LINEAR_MODE)
            {
                self->tCommonArgs.nPoolCfgCnt = sizeof(gtSysCommPoolSingleOs08a20OnlineSdr) / sizeof(gtSysCommPoolSingleOs08a20OnlineSdr[0]);
                self->tCommonArgs.pPoolCfg = gtSysCommPoolSingleOs08a20OnlineSdr;
                self->gCams[0].stDevAttr.ePixelFmt = AX_FORMAT_BAYER_RAW_12BPP;
                self->gCams[0].stPipeAttr.ePixelFmt = AX_FORMAT_BAYER_RAW_12BPP;
            }
            else
            {
                self->tCommonArgs.nPoolCfgCnt = sizeof(gtSysCommPoolSingleOs08a20OnlineHdr) / sizeof(gtSysCommPoolSingleOs08a20OnlineHdr[0]);
                self->tCommonArgs.pPoolCfg = gtSysCommPoolSingleOs08a20OnlineHdr;
                self->gCams[0].stSnsAttr.eRawType = AX_RT_RAW10;
            }
            self->gCams[0].stChnAttr.tChnAttr[0].nDepth = 1;
            self->gCams[0].stChnAttr.tChnAttr[1].bEnable = 0;
            self->gCams[0].stChnAttr.tChnAttr[2].bEnable = 0;
            self->gCams[0].stPipeAttr.ePipeDataSrc = AX_PIPE_SOURCE_DEV_ONLINE;
        }
    }
    else if (self->eSysCase == SYS_CASE_SINGLE_DVP)
    {
        self->tCommonArgs.nCamCnt = 1;
        self->gCams[0].eSnsType = SENSOR_DVP;
        COMMON_ISP_GetSnsConfig(SENSOR_DVP, &self->gCams[0].stSnsAttr, &self->gCams[0].stSnsClkAttr, &self->gCams[0].stDevAttr,
                                &self->gCams[0].stPipeAttr, &self->gCams[0].stChnAttr);

        self->tCommonArgs.nPoolCfgCnt = sizeof(gtSysCommPoolSingleDVP) / sizeof(gtSysCommPoolSingleDVP[0]);
        self->tCommonArgs.pPoolCfg = gtSysCommPoolSingleDVP;
    }
    else if (self->eSysCase == SYS_CASE_SINGLE_BT601)
    {
        self->tCommonArgs.nCamCnt = 1;
        self->gCams[0].eSnsType = SENSOR_BT601;
        COMMON_ISP_GetSnsConfig(SENSOR_BT601, &self->gCams[0].stSnsAttr, &self->gCams[0].stSnsClkAttr, &self->gCams[0].stDevAttr,
                                &self->gCams[0].stPipeAttr,
                                &self->gCams[0].stChnAttr);

        self->tCommonArgs.nPoolCfgCnt = sizeof(gtSysCommPoolBT601) / sizeof(gtSysCommPoolBT601[0]);
        self->tCommonArgs.pPoolCfg = gtSysCommPoolBT601;
    }
    else if (self->eSysCase == SYS_CASE_SINGLE_BT656)
    {
        self->tCommonArgs.nCamCnt = 1;
        self->gCams[0].eSnsType = SENSOR_BT656;
        COMMON_ISP_GetSnsConfig(SENSOR_BT656, &self->gCams[0].stSnsAttr, &self->gCams[0].stSnsClkAttr, &self->gCams[0].stDevAttr,
                                &self->gCams[0].stPipeAttr,
                                &self->gCams[0].stChnAttr);

        self->tCommonArgs.nPoolCfgCnt = sizeof(gtSysCommPoolBT656) / sizeof(gtSysCommPoolBT656[0]);
        self->tCommonArgs.pPoolCfg = gtSysCommPoolBT656;
    }
    else if (self->eSysCase == SYS_CASE_SINGLE_BT1120)
    {
        self->tCommonArgs.nCamCnt = 1;
        self->gCams[0].eSnsType = SENSOR_BT1120;
        COMMON_ISP_GetSnsConfig(SENSOR_BT1120, &self->gCams[0].stSnsAttr, &self->gCams[0].stSnsClkAttr, &self->gCams[0].stDevAttr,
                                &self->gCams[0].stPipeAttr,
                                &self->gCams[0].stChnAttr);
        self->gCams[0].stPipeAttr.ePipeDataSrc = AX_PIPE_SOURCE_DEV_ONLINE;

        self->tCommonArgs.nPoolCfgCnt = sizeof(gtSysCommPoolBT1120) / sizeof(gtSysCommPoolBT1120[0]);
        self->tCommonArgs.pPoolCfg = gtSysCommPoolBT1120;
    }
    else if (self->eSysCase == SYS_CASE_MIPI_YUV)
    {
        self->tCommonArgs.nCamCnt = 1;
        self->gCams[0].eSnsType = MIPI_YUV;
        COMMON_ISP_GetSnsConfig(MIPI_YUV, &self->gCams[0].stSnsAttr, &self->gCams[0].stSnsClkAttr, &self->gCams[0].stDevAttr,
                                &self->gCams[0].stPipeAttr,
                                &self->gCams[0].stChnAttr);
        self->tCommonArgs.nPoolCfgCnt = sizeof(gtSysCommPoolMIPI_YUV) / sizeof(gtSysCommPoolMIPI_YUV[0]);
        self->tCommonArgs.pPoolCfg = gtSysCommPoolMIPI_YUV;
    }
    else if (self->eSysCase == SYS_CASE_THERMAL)
    {
        self->tCommonArgs.nCamCnt = 1;
        self->gCams[0].eSnsType = MIPI_YUV_THERMAL;

        COMMON_ISP_GetSnsConfig(self->gCams[0].eSnsType, &self->gCams[0].stSnsAttr, &self->gCams[0].stSnsClkAttr, &self->gCams[0].stDevAttr,
                                &self->gCams[0].stPipeAttr,
                                &self->gCams[0].stChnAttr);
        self->tCommonArgs.nPoolCfgCnt = sizeof(gtSysCommPoolSingleThermal) / sizeof(gtSysCommPoolSingleThermal[0]);
        self->tCommonArgs.pPoolCfg = gtSysCommPoolSingleThermal;
        snprintf(self->gCams[0].szTuningFileName, MAX_FILE_NAME_CHAR_SIZE, "%s", LOAD_BIN_FILE_NAME);
    }
    else if (self->eSysCase == SYS_CASE_THERMAL_AND_OS04A10)
    {
        self->tCommonArgs.nCamCnt = 2;

        self->gCams[0].eSnsType = MIPI_YUV_THERMAL;
        COMMON_ISP_GetSnsConfig(self->gCams[0].eSnsType, &self->gCams[0].stSnsAttr, &self->gCams[0].stSnsClkAttr, &self->gCams[0].stDevAttr,
                                &self->gCams[0].stPipeAttr,
                                &self->gCams[0].stChnAttr);
        snprintf(self->gCams[0].szTuningFileName, MAX_FILE_NAME_CHAR_SIZE, "%s", LOAD_BIN_FILE_NAME);

        self->gCams[1].eSnsType = OMNIVISION_OS04A10;
        COMMON_ISP_GetSnsConfig(self->gCams[1].eSnsType, &self->gCams[1].stSnsAttr, &self->gCams[1].stSnsClkAttr, &self->gCams[1].stDevAttr,
                                &self->gCams[1].stPipeAttr,
                                &self->gCams[1].stChnAttr);
        self->tCommonArgs.nPoolCfgCnt = sizeof(gtSysCommPoolThermalAndOs04a10) / sizeof(gtSysCommPoolThermalAndOs04a10[0]);
        self->tCommonArgs.pPoolCfg = gtSysCommPoolThermalAndOs04a10;

        self->gCams[1].stSnsClkAttr.nSnsClkIdx = 2; /* mclk2 only by AX DEMO board, User defined */
    }
    else if (SYS_CASE_SINGLE_OS04A10_SPLITE_OFFLINE == self->eSysCase)
    {

        self->tCommonArgs.nCamCnt = 2;
        self->gCams[0].eSnsType = OMNIVISION_OS04A10_LF;
        self->gCams[1].eSnsType = OMNIVISION_OS04A10_SF;
        COMMON_ISP_GetSnsConfig(self->gCams[0].eSnsType, &self->gCams[0].stSnsAttr, &self->gCams[0].stSnsClkAttr, &self->gCams[0].stDevAttr,
                                &self->gCams[0].stPipeAttr,
                                &self->gCams[0].stChnAttr);
        COMMON_ISP_GetSnsConfig(self->gCams[1].eSnsType, &self->gCams[1].stSnsAttr, &self->gCams[1].stSnsClkAttr, &self->gCams[1].stDevAttr,
                                &self->gCams[1].stPipeAttr,
                                &self->gCams[1].stChnAttr);
        self->tCommonArgs.nPoolCfgCnt = sizeof(gtSysCommPoolDoubleOs04a10) / sizeof(gtSysCommPoolDoubleOs04a10[0]);
        self->tCommonArgs.pPoolCfg = gtSysCommPoolDoubleOs04a10;

        self->gCams[0].stSnsClkAttr.nSnsClkIdx = 0; /* mclk0 only by AX DEMO board, User defined */
        self->gCams[1].stSnsClkAttr.nSnsClkIdx = 0; /* mclk2 only by AX DEMO board, User defined */
        self->eHdrMode = AX_SNS_HDR_2X_MODE;
    }
    else if (SYS_CASE_SINGLE_OS08A20_SPLITE_ONLINE == self->eSysCase)
    {

        self->tCommonArgs.nCamCnt = 2;
        self->gCams[0].eSnsType = OMNIVISION_OS08A20_LF;
        self->gCams[1].eSnsType = OMNIVISION_OS08A20_SF;
        COMMON_ISP_GetSnsConfig(self->gCams[0].eSnsType, &self->gCams[0].stSnsAttr, &self->gCams[0].stSnsClkAttr, &self->gCams[0].stDevAttr,
                                &self->gCams[0].stPipeAttr,
                                &self->gCams[0].stChnAttr);
        COMMON_ISP_GetSnsConfig(self->gCams[1].eSnsType, &self->gCams[1].stSnsAttr, &self->gCams[1].stSnsClkAttr, &self->gCams[1].stDevAttr,
                                &self->gCams[1].stPipeAttr,
                                &self->gCams[1].stChnAttr);
        self->tCommonArgs.nPoolCfgCnt = sizeof(gtSysCommPoolSingleOs08a20Hdr_SplitMode) / sizeof(gtSysCommPoolSingleOs08a20Hdr_SplitMode[0]);
        self->tCommonArgs.pPoolCfg = gtSysCommPoolSingleOs08a20Hdr_SplitMode;

        self->gCams[0].stSnsClkAttr.nSnsClkIdx = 0; /* mclk0 only by AX DEMO board, User defined */
        self->gCams[1].stSnsClkAttr.nSnsClkIdx = 0; /* mclk2 only by AX DEMO board, User defined */
        self->eHdrMode = AX_SNS_HDR_2X_MODE;
        self->gCams[0].stPipeAttr.ePipeDataSrc = AX_PIPE_SOURCE_DEV_ONLINE;
        self->gCams[0].stPipeAttr.ePreOutput = AX_PRE_OUTPUT_FULL_MAIN;
        self->gCams[0].stChnAttr.tChnAttr[0].nDepth = 1;
        self->gCams[0].stChnAttr.tChnAttr[1].nDepth = 1;
        self->gCams[0].stChnAttr.tChnAttr[2].nDepth = 1;

        self->gCams[1].stPipeAttr.ePipeDataSrc = AX_PIPE_SOURCE_DEV_ONLINE;
        self->gCams[1].stPipeAttr.ePreOutput = AX_PRE_OUTPUT_LITE_MAIN;
        self->gCams[1].stChnAttr.tChnAttr[0].nDepth = 1;
        self->gCams[1].stChnAttr.tChnAttr[1].nDepth = 1;
        self->gCams[1].stChnAttr.tChnAttr[2].nDepth = 1;
    }
    else if (self->eSysCase == SYS_CASE_SINGLE_YUV)
    {
        self->tCommonArgs.nCamCnt = 1;
        self->gCams[0].eSnsType = MIPI_AHD_YUV;
        COMMON_ISP_GetSnsConfig(MIPI_AHD_YUV, &self->gCams[0].stSnsAttr, &self->gCams[0].stSnsClkAttr, &self->gCams[0].stDevAttr, &self->gCams[0].stPipeAttr,
                                &self->gCams[0].stChnAttr);
        self->tCommonArgs.nPoolCfgCnt = sizeof(gtSysCommPoolMIPI_AHD_YUV) / sizeof(gtSysCommPoolMIPI_AHD_YUV[0]);
        self->tCommonArgs.pPoolCfg = gtSysCommPoolMIPI_AHD_YUV;
        self->gCams[0].stSnsClkAttr.nSnsClkIdx = 2;
    }
    else if (self->eSysCase == SYS_CASE_OS04A10_YUV422_RX)
    {
        self->tCommonArgs.nCamCnt = 1;
        self->gCams[0].eSnsType = OMNIVISION_OS04A10_YUV422_RX;
        COMMON_ISP_GetSnsConfig(OMNIVISION_OS04A10_YUV422_RX, &self->gCams[0].stSnsAttr, &self->gCams[0].stSnsClkAttr, &self->gCams[0].stDevAttr, &self->gCams[0].stPipeAttr,
                                &self->gCams[0].stChnAttr);
        self->tCommonArgs.nPoolCfgCnt = sizeof(gtSysCommPoolMIPI_OS04A10_RX_YUV) / sizeof(gtSysCommPoolMIPI_OS04A10_RX_YUV[0]);
        self->tCommonArgs.pPoolCfg = gtSysCommPoolMIPI_OS04A10_RX_YUV;
        self->eHdrMode = AX_SNS_LINEAR_MODE; // yuv sensor not support 2dol
        self->gCams[0].ePixelFmt = AX_YUV422_INTERLEAVED_UYVY;
        self->gCams[0].info_tx_en = self->infoTx;
        self->gCams[0].eRawType = AX_RT_RAW8;
    }
    else if (self->eSysCase == SYS_CASE_IMX327_IMX327_AND_YUV)
    {
        self->tCommonArgs.nCamCnt = 3;
        self->gCams[0].eSnsType = SONY_IMX327;
        COMMON_ISP_GetSnsConfig(SONY_IMX327, &self->gCams[0].stSnsAttr, &self->gCams[0].stSnsClkAttr, &self->gCams[0].stDevAttr,
                                &self->gCams[0].stPipeAttr,
                                &self->gCams[0].stChnAttr);
        self->gCams[1].eSnsType = SONY_IMX327;
        COMMON_ISP_GetSnsConfig(SONY_IMX327, &self->gCams[1].stSnsAttr, &self->gCams[1].stSnsClkAttr, &self->gCams[1].stDevAttr,
                                &self->gCams[1].stPipeAttr,
                                &self->gCams[1].stChnAttr);
        self->gCams[2].eSnsType = MIPI_AHD_YUV;
        COMMON_ISP_GetSnsConfig(MIPI_AHD_YUV, &self->gCams[2].stSnsAttr, &self->gCams[2].stSnsClkAttr, &self->gCams[2].stDevAttr, &self->gCams[2].stPipeAttr,
                                &self->gCams[2].stChnAttr);
        self->tCommonArgs.nPoolCfgCnt = sizeof(gtSysCommPoolIMX327_IMX327_AHDYUV) / sizeof(gtSysCommPoolIMX327_IMX327_AHDYUV[0]);
        self->tCommonArgs.pPoolCfg = gtSysCommPoolIMX327_IMX327_AHDYUV;
        self->gCams[0].stSnsAttr.nFrameRate = 25;
        self->gCams[1].stSnsAttr.nFrameRate = 25;
        self->gCams[0].stSnsClkAttr.nSnsClkIdx = 0; /* mclk0 only by AX DEMO board, User defined */
        self->gCams[1].stSnsClkAttr.nSnsClkIdx = 1; /* mclk2 only by AX DEMO board, User defined */
        self->gCams[2].stSnsClkAttr.nSnsClkIdx = 2; /* mclk2 only by AX DEMO board, User defined */
    }
    else if (self->eSysCase == SYS_CASE_FOUR_SC230AI)
    {
        self->tCommonArgs.nCamCnt = 4;
        self->gCams[0].eSnsType = SMARTSENS_SC230AI;
        COMMON_ISP_GetSnsConfig(SMARTSENS_SC230AI, &self->gCams[0].stSnsAttr, &self->gCams[0].stSnsClkAttr, &self->gCams[0].stDevAttr,
                                &self->gCams[0].stPipeAttr,
                                &self->gCams[0].stChnAttr);
        self->gCams[1].eSnsType = SMARTSENS_SC230AI;
        COMMON_ISP_GetSnsConfig(SMARTSENS_SC230AI, &self->gCams[1].stSnsAttr, &self->gCams[1].stSnsClkAttr, &self->gCams[1].stDevAttr,
                                &self->gCams[1].stPipeAttr,
                                &self->gCams[1].stChnAttr);
        self->gCams[2].eSnsType = SMARTSENS_SC230AI;
        COMMON_ISP_GetSnsConfig(SMARTSENS_SC230AI, &self->gCams[2].stSnsAttr, &self->gCams[2].stSnsClkAttr, &self->gCams[2].stDevAttr, &self->gCams[2].stPipeAttr,
                                &self->gCams[2].stChnAttr);
        self->gCams[3].eSnsType = SMARTSENS_SC230AI;
        COMMON_ISP_GetSnsConfig(SMARTSENS_SC230AI, &self->gCams[3].stSnsAttr, &self->gCams[3].stSnsClkAttr, &self->gCams[3].stDevAttr, &self->gCams[3].stPipeAttr,
                                &self->gCams[3].stChnAttr);
        self->tCommonArgs.nPoolCfgCnt = sizeof(gtSysCommPoolFourSC230AI) / sizeof(gtSysCommPoolFourSC230AI[0]);
        self->tCommonArgs.pPoolCfg = gtSysCommPoolFourSC230AI;
        self->gCams[0].stSnsAttr.nFrameRate = 25;
        self->gCams[1].stSnsAttr.nFrameRate = 25;
        self->gCams[2].stSnsAttr.nFrameRate = 25;
        self->gCams[3].stSnsAttr.nFrameRate = 25;
        self->gCams[3].stSnsAttr.eMasterSlaveSel = 1;
        self->gCams[0].stSnsClkAttr.nSnsClkIdx = 0; /* mclk0 only by AX DEMO board, User defined */
        self->gCams[1].stSnsClkAttr.nSnsClkIdx = 1; /* mclk2 only by AX DEMO board, User defined */
        self->gCams[2].stSnsClkAttr.nSnsClkIdx = 2; /* mclk2 only by AX DEMO board, User defined */
        self->gCams[3].stSnsClkAttr.nSnsClkIdx = 2; /* mclk2 only by AX DEMO board, User defined */
    }
    else if (self->eSysCase == SYS_CASE_SINGLE_OV12D2Q)
    {
        self->tCommonArgs.nCamCnt = 1;
        self->gCams[0].eSnsType = OMNIVISION_OV12D2Q;
        COMMON_ISP_GetSnsConfig(OMNIVISION_OV12D2Q, &self->gCams[0].stSnsAttr, &self->gCams[0].stSnsClkAttr, &self->gCams[0].stDevAttr,
                                &self->gCams[0].stPipeAttr,
                                &self->gCams[0].stChnAttr);
        if (self->eHdrMode == AX_SNS_LINEAR_MODE)
        {
            self->tCommonArgs.nPoolCfgCnt = sizeof(gtSysCommPoolSingleOv12d2qSdr) / sizeof(gtSysCommPoolSingleOv12d2qSdr[0]);
            self->tCommonArgs.pPoolCfg = gtSysCommPoolSingleOv12d2qSdr;
            self->gCams[0].stDevAttr.ePixelFmt = AX_FORMAT_BAYER_RAW_10BPP;
            self->gCams[0].stPipeAttr.ePixelFmt = AX_FORMAT_BAYER_RAW_10BPP;
        }
        else
        {
            self->tCommonArgs.nPoolCfgCnt = sizeof(gtSysCommPoolSingleOv12d2qHdr) / sizeof(gtSysCommPoolSingleOv12d2qHdr[0]);
            self->tCommonArgs.pPoolCfg = gtSysCommPoolSingleOv12d2qHdr;
            self->gCams[0].stSnsAttr.eRawType = AX_RT_RAW10;
        }
    }
    else if (self->eSysCase == SYS_CASE_OS04A10_AND_MIPITX422)
    {
        self->tCommonArgs.nCamCnt = 1;
        self->gCams[0].eSnsType = OMNIVISION_OS04A10_AND_MIPITX422;
        COMMON_ISP_GetSnsConfig(OMNIVISION_OS04A10_AND_MIPITX422, &self->gCams[0].stSnsAttr, &self->gCams[0].stSnsClkAttr, &self->gCams[0].stDevAttr,
                                &self->gCams[0].stPipeAttr, &self->gCams[0].stChnAttr);
#ifdef SAMPLE_620U_NAND
        if (self->eHdrMode == AX_SNS_LINEAR_MODE)
        {
            self->tCommonArgs.nPoolCfgCnt = sizeof(gtSysCommPoolSingleOs04a10Sdru) / sizeof(gtSysCommPoolSingleOs04a10Sdru[0]);
            self->tCommonArgs.pPoolCfg = gtSysCommPoolSingleOs04a10Sdru;
            snprintf(self->gCams[0].szTuningFileName, MAX_FILE_NAME_CHAR_SIZE, "%s", OS04A10_SDR_620U_NO_PFR_BIN_FILE_NAME);
        }
        else if (self->eHdrMode == AX_SNS_HDR_2X_MODE)
        {
            self->tCommonArgs.nPoolCfgCnt = sizeof(gtSysCommPoolSingleOs04a10OnlineHdru) / sizeof(gtSysCommPoolSingleOs04a10OnlineHdru[0]);
            self->tCommonArgs.pPoolCfg = gtSysCommPoolSingleOs04a10OnlineHdru;
            snprintf(self->gCams[0].szTuningFileName, MAX_FILE_NAME_CHAR_SIZE, "%s", OS04A10_HDR_2X_620U_NO_PFR_BIN_FILE_NAME);
        }
#else
        if (self->eHdrMode == AX_SNS_LINEAR_MODE)
        {
            self->tCommonArgs.nPoolCfgCnt = sizeof(gtSysCommPoolSingleOs04a10Sdr) / sizeof(gtSysCommPoolSingleOs04a10Sdr[0]);
            self->tCommonArgs.pPoolCfg = gtSysCommPoolSingleOs04a10Sdr;
            snprintf(self->gCams[0].szTuningFileName, MAX_FILE_NAME_CHAR_SIZE, "%s", OS04A10_SDR_620A_NO_PFR_BIN_FILE_NAME);
        }
        else if (self->eHdrMode == AX_SNS_HDR_2X_MODE)
        {
            self->tCommonArgs.nPoolCfgCnt = sizeof(gtSysCommPoolSingleOs04a10Hdr) / sizeof(gtSysCommPoolSingleOs04a10Hdr[0]);
            self->tCommonArgs.pPoolCfg = gtSysCommPoolSingleOs04a10Hdr;
            snprintf(self->gCams[0].szTuningFileName, MAX_FILE_NAME_CHAR_SIZE, "%s", OS04A10_HDR_2X_620A_NO_PFR_BIN_FILE_NAME);
        }
#endif
        self->gCams[0].stChnAttr.tChnAttr[0].nDepth = 1;
        self->gCams[0].stChnAttr.tChnAttr[1].bEnable = 0;
        self->gCams[0].stChnAttr.tChnAttr[2].bEnable = 0;
        self->gCams[0].stPipeAttr.ePipeDataSrc = AX_PIPE_SOURCE_DEV_ONLINE;
        self->gCams[0].info_tx_en = self->infoTx;
    }
    else if (self->eSysCase == SYS_CASE_SINGLE_SC530AI_ONLINE)
    {
        self->tCommonArgs.nCamCnt = 1;
        self->gCams[0].eSnsType = SMARTSENS_SC530AI;
        COMMON_ISP_GetSnsConfig(SMARTSENS_SC530AI, &self->gCams[0].stSnsAttr, &self->gCams[0].stSnsClkAttr, &self->gCams[0].stDevAttr,
                                &self->gCams[0].stPipeAttr, &self->gCams[0].stChnAttr);
        if (self->eHdrMode == AX_SNS_LINEAR_MODE)
        {
            self->tCommonArgs.nPoolCfgCnt = sizeof(gtSysCommPoolSingleSc530aiOnlineSdr) / sizeof(gtSysCommPoolSingleSc530aiOnlineSdr[0]);
            self->tCommonArgs.pPoolCfg = gtSysCommPoolSingleSc530aiOnlineSdr;
            snprintf(self->gCams[0].szTuningFileName, MAX_FILE_NAME_CHAR_SIZE, "%s", SC530AI_SDR_620A_QUARTER_EIS_BIN_FILE_NAME);
        }
        else if (self->eHdrMode == AX_SNS_HDR_2X_MODE)
        {
            self->tCommonArgs.nPoolCfgCnt = sizeof(gtSysCommPoolSingleSc530aiOnlineHdr) / sizeof(gtSysCommPoolSingleSc530aiOnlineHdr[0]);
            self->tCommonArgs.pPoolCfg = gtSysCommPoolSingleSc530aiOnlineHdr;
            snprintf(self->gCams[0].szTuningFileName, MAX_FILE_NAME_CHAR_SIZE, "%s", SC530AI_HDR_2X_620A_QUARTER_EIS_BIN_FILE_NAME);
        }
        self->gCams[0].stPipeAttr.ePipeDataSrc = AX_PIPE_SOURCE_DEV_ONLINE;
        self->gCams[0].stChnAttr.tChnAttr[0].nDepth = 1;
        self->gCams[0].stChnAttr.tChnAttr[1].nDepth = 1;
        self->gCams[0].stChnAttr.tChnAttr[2].nDepth = 1;
    }
    else if (self->eSysCase == SYS_CASE_SINGLE_IMX327_ONLINE)
    {
        self->tCommonArgs.nCamCnt = 1;
        self->gCams[0].eSnsType = SONY_IMX327;
        COMMON_ISP_GetSnsConfig(SONY_IMX327, &self->gCams[0].stSnsAttr, &self->gCams[0].stSnsClkAttr, &self->gCams[0].stDevAttr,
                                &self->gCams[0].stPipeAttr, &self->gCams[0].stChnAttr);
#ifdef SAMPLE_620U_NAND
        if (self->eHdrMode == AX_SNS_LINEAR_MODE)
        {
            self->tCommonArgs.nPoolCfgCnt = sizeof(gtSysCommPoolSingleImx327OnlineSdru) / sizeof(gtSysCommPoolSingleImx327OnlineSdru[0]);
            self->tCommonArgs.pPoolCfg = gtSysCommPoolSingleImx327OnlineSdru;
            self->gCams[0].stDevAttr.ePixelFmt = AX_FORMAT_BAYER_RAW_12BPP;
            self->gCams[0].stPipeAttr.ePixelFmt = AX_FORMAT_BAYER_RAW_12BPP;
        }
        else if (self->eHdrMode == AX_SNS_HDR_2X_MODE)
        {
            self->tCommonArgs.nPoolCfgCnt = sizeof(gtSysCommPoolSingleImx327OnlineHdru) / sizeof(gtSysCommPoolSingleImx327OnlineHdru[0]);
            self->tCommonArgs.pPoolCfg = gtSysCommPoolSingleImx327OnlineHdru;
            self->gCams[0].stSnsAttr.eRawType = AX_RT_RAW10;
            self->gCams[0].stDevAttr.ePixelFmt = AX_FORMAT_BAYER_RAW_10BPP;
            self->gCams[0].stDevAttr.tMipiIntfAttr.eLineInfoMode = AX_VIN_DEV_MIPI_MODE_LINE_INFO_4PIXEL;
            self->gCams[0].stDevAttr.tMipiIntfAttr.tLineInfo[0].nLineInfoMask = 4028;
            self->gCams[0].stDevAttr.tMipiIntfAttr.tLineInfo[0].nLineInfoSet = 2308;
            self->gCams[0].stDevAttr.tMipiIntfAttr.tLineInfo[1].nLineInfoMask = 4028;
            self->gCams[0].stDevAttr.tMipiIntfAttr.tLineInfo[1].nLineInfoSet = 2312;
            self->gCams[0].stPipeAttr.ePixelFmt = AX_FORMAT_BAYER_RAW_10BPP;
        }
#else
        if (self->eHdrMode == AX_SNS_LINEAR_MODE)
        {
            self->tCommonArgs.nPoolCfgCnt = sizeof(gtSysCommPoolSingleImx327OnlineSdr) / sizeof(gtSysCommPoolSingleImx327OnlineSdr[0]);
            self->tCommonArgs.pPoolCfg = gtSysCommPoolSingleImx327OnlineSdr;
            self->gCams[0].stDevAttr.ePixelFmt = AX_FORMAT_BAYER_RAW_12BPP;
            self->gCams[0].stPipeAttr.ePixelFmt = AX_FORMAT_BAYER_RAW_12BPP;
            snprintf(self->gCams[0].szTuningFileName, MAX_FILE_NAME_CHAR_SIZE, "%s", IMX327_SDR_620A_QUARTER_EIS_BIN_FILE_NAME);
        }
        else if (self->eHdrMode == AX_SNS_HDR_2X_MODE)
        {
            self->tCommonArgs.nPoolCfgCnt = sizeof(gtSysCommPoolSingleImx327OnlineHdr) / sizeof(gtSysCommPoolSingleImx327OnlineHdr[0]);
            self->tCommonArgs.pPoolCfg = gtSysCommPoolSingleImx327OnlineHdr;
            self->gCams[0].stSnsAttr.eRawType = AX_RT_RAW10;
            self->gCams[0].stDevAttr.ePixelFmt = AX_FORMAT_BAYER_RAW_10BPP;
            self->gCams[0].stDevAttr.tMipiIntfAttr.eLineInfoMode = AX_VIN_DEV_MIPI_MODE_LINE_INFO_4PIXEL;
            self->gCams[0].stDevAttr.tMipiIntfAttr.tLineInfo[0].nLineInfoMask = 4028;
            self->gCams[0].stDevAttr.tMipiIntfAttr.tLineInfo[0].nLineInfoSet = 2308;
            self->gCams[0].stDevAttr.tMipiIntfAttr.tLineInfo[1].nLineInfoMask = 4028;
            self->gCams[0].stDevAttr.tMipiIntfAttr.tLineInfo[1].nLineInfoSet = 2312;
            self->gCams[0].stPipeAttr.ePixelFmt = AX_FORMAT_BAYER_RAW_10BPP;
            snprintf(self->gCams[0].szTuningFileName, MAX_FILE_NAME_CHAR_SIZE, "%s", IMX327_HDR_2X_620A_QUARTER_EIS_BIN_FILE_NAME);
        }
#endif
        self->gCams[0].stPipeAttr.ePipeDataSrc = AX_PIPE_SOURCE_DEV_ONLINE;
        self->gCams[0].stChnAttr.tChnAttr[0].nDepth = 1;
        self->gCams[0].stChnAttr.tChnAttr[1].nDepth = 1;
        self->gCams[0].stChnAttr.tChnAttr[2].nDepth = 1;
    }
    else if (self->eSysCase == SYS_CASE_DUAL_IMX327_ONLINE_U)
    {
        self->tCommonArgs.nCamCnt = 2;
        self->gCams[0].eSnsType = SONY_IMX327;
        self->gCams[1].eSnsType = SONY_IMX327;
        COMMON_ISP_GetSnsConfig(SONY_IMX327, &self->gCams[0].stSnsAttr, &self->gCams[0].stSnsClkAttr, &self->gCams[0].stDevAttr,
                                &self->gCams[0].stPipeAttr,
                                &self->gCams[0].stChnAttr);
        COMMON_ISP_GetSnsConfig(SONY_IMX327, &self->gCams[1].stSnsAttr, &self->gCams[1].stSnsClkAttr, &self->gCams[1].stDevAttr,
                                &self->gCams[1].stPipeAttr,
                                &self->gCams[1].stChnAttr);
        if (self->eHdrMode == AX_SNS_LINEAR_MODE)
        {
            self->tCommonArgs.nPoolCfgCnt = sizeof(gtSysCommPoolDoubleImx327OnlineSdru) / sizeof(gtSysCommPoolDoubleImx327OnlineSdru[0]);
            self->tCommonArgs.pPoolCfg = gtSysCommPoolDoubleImx327OnlineSdru;
        }
        else if (self->eHdrMode == AX_SNS_HDR_2X_MODE)
        {
            self->tCommonArgs.nPoolCfgCnt = sizeof(gtSysCommPoolDoubleImx327OnlineHdru) / sizeof(gtSysCommPoolDoubleImx327OnlineHdru[0]);
            self->tCommonArgs.pPoolCfg = gtSysCommPoolDoubleImx327OnlineHdru;
        }

        self->gCams[0].stPipeAttr.ePipeDataSrc = AX_PIPE_SOURCE_DEV_ONLINE;
        self->gCams[1].stPipeAttr.ePipeDataSrc = AX_PIPE_SOURCE_DEV_ONLINE;
        self->gCams[0].stPipeAttr.eDevSource = AX_DEV_SOURCE_SNS_0;
        self->gCams[1].stPipeAttr.eDevSource = AX_DEV_SOURCE_SNS_1;
        self->gCams[0].stPipeAttr.ePreOutput = AX_PRE_OUTPUT_FULL_MAIN;
        self->gCams[1].stPipeAttr.ePreOutput = AX_PRE_OUTPUT_LITE_MAIN;
        self->gCams[0].stSnsClkAttr.nSnsClkIdx = 0; /* mclk0 only by AX DEMO board, User defined */
        self->gCams[1].stSnsClkAttr.nSnsClkIdx = 1; /* mclk2 only by AX DEMO board, User defined */
    }
    else if (self->eSysCase == SYS_CASE_TRIPLE_IMX327)
    {
        self->tCommonArgs.nCamCnt = 3;
        self->gCams[0].eSnsType = SONY_IMX327;
        COMMON_ISP_GetSnsConfig(SONY_IMX327, &self->gCams[0].stSnsAttr, &self->gCams[0].stSnsClkAttr, &self->gCams[0].stDevAttr,
                                &self->gCams[0].stPipeAttr,
                                &self->gCams[0].stChnAttr);
        self->gCams[1].eSnsType = SONY_IMX327;
        COMMON_ISP_GetSnsConfig(SONY_IMX327, &self->gCams[1].stSnsAttr, &self->gCams[1].stSnsClkAttr, &self->gCams[1].stDevAttr,
                                &self->gCams[1].stPipeAttr,
                                &self->gCams[1].stChnAttr);
        self->gCams[2].eSnsType = SONY_IMX327;
        COMMON_ISP_GetSnsConfig(SONY_IMX327, &self->gCams[2].stSnsAttr, &self->gCams[2].stSnsClkAttr, &self->gCams[2].stDevAttr, &self->gCams[2].stPipeAttr,
                                &self->gCams[2].stChnAttr);
        self->tCommonArgs.nPoolCfgCnt = sizeof(gtSysCommPoolIMX327TripleSdr) / sizeof(gtSysCommPoolIMX327TripleSdr[0]);
        self->tCommonArgs.pPoolCfg = gtSysCommPoolIMX327TripleSdr;
        self->gCams[0].stSnsAttr.nFrameRate = 25;
        self->gCams[1].stSnsAttr.nFrameRate = 25;
        self->gCams[2].stSnsAttr.nFrameRate = 25;
        self->gCams[0].stSnsClkAttr.nSnsClkIdx = 0; /* mclk0 only by AX DEMO board, User defined */
        self->gCams[1].stSnsClkAttr.nSnsClkIdx = 1; /* mclk2 only by AX DEMO board, User defined */
        self->gCams[2].stSnsClkAttr.nSnsClkIdx = 2; /* mclk2 only by AX DEMO board, User defined */
    }
    else if (self->eSysCase == SYS_CASE_FOUR_IMX327)
    {
        self->tCommonArgs.nCamCnt = 4;
        self->gCams[0].eSnsType = SONY_IMX327;
        COMMON_ISP_GetSnsConfig(SONY_IMX327, &self->gCams[0].stSnsAttr, &self->gCams[0].stSnsClkAttr, &self->gCams[0].stDevAttr,
                                &self->gCams[0].stPipeAttr,
                                &self->gCams[0].stChnAttr);
        self->gCams[1].eSnsType = SONY_IMX327;
        COMMON_ISP_GetSnsConfig(SONY_IMX327, &self->gCams[1].stSnsAttr, &self->gCams[1].stSnsClkAttr, &self->gCams[1].stDevAttr,
                                &self->gCams[1].stPipeAttr,
                                &self->gCams[1].stChnAttr);
        self->gCams[2].eSnsType = SONY_IMX327;
        COMMON_ISP_GetSnsConfig(SONY_IMX327, &self->gCams[2].stSnsAttr, &self->gCams[2].stSnsClkAttr, &self->gCams[2].stDevAttr, &self->gCams[2].stPipeAttr,
                                &self->gCams[2].stChnAttr);
        self->gCams[3].eSnsType = SONY_IMX327;
        COMMON_ISP_GetSnsConfig(SONY_IMX327, &self->gCams[3].stSnsAttr, &self->gCams[3].stSnsClkAttr, &self->gCams[3].stDevAttr, &self->gCams[3].stPipeAttr,
                                &self->gCams[3].stChnAttr);
        self->tCommonArgs.nPoolCfgCnt = sizeof(gtSysCommPoolFourIMX327) / sizeof(gtSysCommPoolFourIMX327[0]);
        self->tCommonArgs.pPoolCfg = gtSysCommPoolFourIMX327;
        self->gCams[0].stSnsAttr.nFrameRate = 25;
        self->gCams[1].stSnsAttr.nFrameRate = 25;
        self->gCams[2].stSnsAttr.nFrameRate = 25;
        self->gCams[3].stSnsAttr.nFrameRate = 25;
        self->gCams[3].stSnsAttr.eMasterSlaveSel = 1;
        self->gCams[0].stSnsClkAttr.nSnsClkIdx = 0; /* mclk0 only by AX DEMO board, User defined */
        self->gCams[1].stSnsClkAttr.nSnsClkIdx = 1; /* mclk2 only by AX DEMO board, User defined */
        self->gCams[2].stSnsClkAttr.nSnsClkIdx = 2; /* mclk2 only by AX DEMO board, User defined */
        self->gCams[3].stSnsClkAttr.nSnsClkIdx = 2; /* mclk2 only by AX DEMO board, User defined */
    }
    else if (self->eSysCase == SYS_CASE_SINGLE_GC4653_ONLINE_U)
    {
        self->tCommonArgs.nCamCnt = 1;
        self->gCams[0].eSnsType = GALAXYCORE_GC4653;
        self->tCommonArgs.nPoolCfgCnt = sizeof(gtSysCommPoolSingleGc4653Onlineu) / sizeof(gtSysCommPoolSingleGc4653Onlineu[0]);
        self->tCommonArgs.pPoolCfg = gtSysCommPoolSingleGc4653Onlineu;
        COMMON_ISP_GetSnsConfig(GALAXYCORE_GC4653, &self->gCams[0].stSnsAttr, &self->gCams[0].stSnsClkAttr, &self->gCams[0].stDevAttr,
                                &self->gCams[0].stPipeAttr,
                                &self->gCams[0].stChnAttr);
        self->gCams[0].stPipeAttr.ePipeDataSrc = AX_PIPE_SOURCE_DEV_ONLINE;
        self->gCams[0].stChnAttr.tChnAttr[0].nDepth = 1;
        self->gCams[0].stChnAttr.tChnAttr[1].nDepth = 1;
        self->gCams[0].stChnAttr.tChnAttr[2].nDepth = 1;
    }
    else if (self->eSysCase == SYS_CASE_SINGLE_IMX464)
    {
        self->tCommonArgs.nCamCnt = 1;
        self->gCams[0].eSnsType = SONY_IMX464;
        COMMON_ISP_GetSnsConfig(SONY_IMX464, &self->gCams[0].stSnsAttr, &self->gCams[0].stSnsClkAttr, &self->gCams[0].stDevAttr,
                                &self->gCams[0].stPipeAttr, &self->gCams[0].stChnAttr);

        if (self->gCams[0].ePipeDataSrc == AX_PIPE_SOURCE_DEV_OFFLINE)
        {
            if (self->eHdrMode == AX_SNS_LINEAR_MODE)
            {
                self->tCommonArgs.nPoolCfgCnt = sizeof(gtSysCommPoolSingleImx464Sdr) / sizeof(gtSysCommPoolSingleImx464Sdr[0]);
                self->tCommonArgs.pPoolCfg = gtSysCommPoolSingleImx464Sdr;
            }
            else if (self->eHdrMode == AX_SNS_HDR_2X_MODE)
            {
                self->tCommonArgs.nPoolCfgCnt = sizeof(gtSysCommPoolSingleImx464Hdr) / sizeof(gtSysCommPoolSingleImx464Hdr[0]);
                self->tCommonArgs.pPoolCfg = gtSysCommPoolSingleImx464Hdr;
            }
        }
        else
        {
            if (self->eHdrMode == AX_SNS_LINEAR_MODE)
            {
                self->tCommonArgs.nPoolCfgCnt = sizeof(gtSysCommPoolSingleImx464OnlineSdr) / sizeof(gtSysCommPoolSingleImx464OnlineSdr[0]);
                self->tCommonArgs.pPoolCfg = gtSysCommPoolSingleImx464OnlineSdr;
                snprintf(self->gCams[0].szTuningFileName, MAX_FILE_NAME_CHAR_SIZE, "%s", IMX464_SDR_620A_QUARTER_EIS_BIN_FILE_NAME);
            }
            else if (self->eHdrMode == AX_SNS_HDR_2X_MODE)
            {
                self->tCommonArgs.nPoolCfgCnt = sizeof(gtSysCommPoolSingleImx464OnlineHdr) / sizeof(gtSysCommPoolSingleImx464OnlineHdr[0]);
                self->tCommonArgs.pPoolCfg = gtSysCommPoolSingleImx464OnlineHdr;
                snprintf(self->gCams[0].szTuningFileName, MAX_FILE_NAME_CHAR_SIZE, "%s", IMX464_HDR_2X_620A_QUARTER_EIS_BIN_FILE_NAME);
            }
            self->gCams[0].stPipeAttr.ePipeDataSrc = AX_PIPE_SOURCE_DEV_ONLINE;
            self->gCams[0].stChnAttr.tChnAttr[0].nDepth = 1;
            self->gCams[0].stChnAttr.tChnAttr[1].nDepth = 1;
            self->gCams[0].stChnAttr.tChnAttr[2].nDepth = 1;
            self->gCams[0].stSnsAttr.eRawType = AX_RT_RAW12;
            self->gCams[0].stDevAttr.ePixelFmt = AX_FORMAT_BAYER_RAW_12BPP;
            self->gCams[0].stPipeAttr.ePixelFmt = AX_FORMAT_BAYER_RAW_12BPP;
        }
    }
    else if (self->eSysCase == SYS_CASE_SINGLE_IMX415)
    {
        self->tCommonArgs.nCamCnt = 1;
        self->gCams[0].eSnsType = SONY_IMX415;
        COMMON_ISP_GetSnsConfig(SONY_IMX415, &self->gCams[0].stSnsAttr, &self->gCams[0].stSnsClkAttr, &self->gCams[0].stDevAttr,
                                &self->gCams[0].stPipeAttr, &self->gCams[0].stChnAttr);

        if (self->gCams[0].ePipeDataSrc == AX_PIPE_SOURCE_DEV_OFFLINE)
        {
            if (self->eHdrMode == AX_SNS_LINEAR_MODE)
            {
                self->tCommonArgs.nPoolCfgCnt = sizeof(gtSysCommPoolSingleImx415Sdru) / sizeof(gtSysCommPoolSingleImx415Sdru[0]);
                self->tCommonArgs.pPoolCfg = gtSysCommPoolSingleImx415Sdru;
            }
            else if (self->eHdrMode == AX_SNS_HDR_2X_MODE)
            {
                self->tCommonArgs.nPoolCfgCnt = sizeof(gtSysCommPoolSingleImx415Hdru) / sizeof(gtSysCommPoolSingleImx415Hdru[0]);
                self->tCommonArgs.pPoolCfg = gtSysCommPoolSingleImx415Hdru;
            }
        }
        else
        {
            if (self->eHdrMode == AX_SNS_LINEAR_MODE)
            {
                self->tCommonArgs.nPoolCfgCnt = sizeof(gtSysCommPoolSingleImx415OnlineSdru) / sizeof(gtSysCommPoolSingleImx415OnlineSdru[0]);
                self->tCommonArgs.pPoolCfg = gtSysCommPoolSingleImx415OnlineSdru;
            }
            else if (self->eHdrMode == AX_SNS_HDR_2X_MODE)
            {
                self->tCommonArgs.nPoolCfgCnt = sizeof(gtSysCommPoolSingleImx415OnlineHdru) / sizeof(gtSysCommPoolSingleImx415OnlineHdru[0]);
                self->tCommonArgs.pPoolCfg = gtSysCommPoolSingleImx415OnlineHdru;
            }
            self->gCams[0].stPipeAttr.ePipeDataSrc = AX_PIPE_SOURCE_DEV_ONLINE;
            self->gCams[0].stChnAttr.tChnAttr[0].nDepth = 1;
            self->gCams[0].stChnAttr.tChnAttr[1].nDepth = 1;
            self->gCams[0].stChnAttr.tChnAttr[2].nDepth = 1;
        }
    }
    else if (self->eSysCase == SYS_CASE_SYNC_DUAL_OS04A10_PTS)
    {
        self->tCommonArgs.nCamCnt = 2;
        self->gCams[0].eSnsType = OMNIVISION_OS04A10;
        self->gCams[1].eSnsType = OMNIVISION_OS04A10;
        COMMON_ISP_GetSnsConfig(OMNIVISION_OS04A10, &self->gCams[0].stSnsAttr, &self->gCams[0].stSnsClkAttr, &self->gCams[0].stDevAttr, &self->gCams[0].stPipeAttr,
                                &self->gCams[0].stChnAttr);
        COMMON_ISP_GetSnsConfig(OMNIVISION_OS04A10, &self->gCams[1].stSnsAttr, &self->gCams[1].stSnsClkAttr, &self->gCams[1].stDevAttr, &self->gCams[1].stPipeAttr,
                                &self->gCams[1].stChnAttr);
        self->tCommonArgs.nPoolCfgCnt = sizeof(gtSysCommPoolDoubleOnlineOs04a10) / sizeof(gtSysCommPoolDoubleOnlineOs04a10[0]);
        self->tCommonArgs.pPoolCfg = gtSysCommPoolDoubleOnlineOs04a10;
        self->gCams[0].stSnsClkAttr.nSnsClkIdx = 0; /* mclk0 only by AX DEMO board, User defined */
        self->gCams[1].stSnsClkAttr.nSnsClkIdx = 1; /* mclk1 only by AX DEMO board, User defined */
        self->gCams[0].stPipeAttr.ePipeDataSrc = AX_PIPE_SOURCE_DEV_ONLINE;
        self->gCams[0].stChnAttr.tChnAttr[0].nDepth = 1;
        self->gCams[0].stChnAttr.tChnAttr[1].nDepth = 1;
        self->gCams[0].stChnAttr.tChnAttr[2].nDepth = 1;
        self->gCams[1].stPipeAttr.ePipeDataSrc = AX_PIPE_SOURCE_DEV_ONLINE;
        self->gCams[0].stPipeAttr.eDevSource = AX_DEV_SOURCE_SNS_0;
        self->gCams[1].stPipeAttr.eDevSource = AX_DEV_SOURCE_SNS_1;
        self->gCams[0].stPipeAttr.ePreOutput = AX_PRE_OUTPUT_FULL_MAIN;
        self->gCams[1].stPipeAttr.ePreOutput = AX_PRE_OUTPUT_LITE_MAIN;
        self->gCams[1].stChnAttr.tChnAttr[0].nDepth = 1;
        self->gCams[1].stChnAttr.tChnAttr[1].nDepth = 1;
        self->gCams[1].stChnAttr.tChnAttr[2].nDepth = 1;
        self->gCams[0].stSnsAttr.eMasterSlaveSel = 3;
        self->gCams[1].stSnsAttr.eMasterSlaveSel = 2;
        for (int i = 0; i < self->tCommonArgs.nCamCnt; i++)
        {
            self->gCams[i].stSnsAttr.eSnsMode = self->eHdrMode;
            self->gCams[i].stDevAttr.eSnsMode = self->eHdrMode;
            self->gCams[i].stPipeAttr.eSnsMode = self->eHdrMode;
            if (i == 0)
            {
                self->gCams[i].nDevId = 0;
                self->gCams[i].nRxDev = AX_MIPI_RX_DEV_0;
                self->gCams[i].nPipeId = 0;
            }
            else if (i == 1)
            {
                self->gCams[i].nDevId = 1;
                self->gCams[i].nRxDev = AX_MIPI_RX_DEV_1;
                self->gCams[i].nPipeId = 1;
            }
        }
    }
    else if (self->eSysCase == SYS_CASE_SYNC_DUAL_SC230AI_PTS)
    {
        self->tCommonArgs.nCamCnt = 2;
        self->gCams[0].eSnsType = SMARTSENS_SC230AI;
        self->gCams[1].eSnsType = SMARTSENS_SC230AI;
        COMMON_ISP_GetSnsConfig(SMARTSENS_SC230AI, &self->gCams[0].stSnsAttr, &self->gCams[0].stSnsClkAttr, &self->gCams[0].stDevAttr, &self->gCams[0].stPipeAttr,
                                &self->gCams[0].stChnAttr);
        COMMON_ISP_GetSnsConfig(SMARTSENS_SC230AI, &self->gCams[1].stSnsAttr, &self->gCams[1].stSnsClkAttr, &self->gCams[1].stDevAttr, &self->gCams[1].stPipeAttr,
                                &self->gCams[1].stChnAttr);
        self->tCommonArgs.nPoolCfgCnt = sizeof(gtSysCommPoolDualSC230AI) / sizeof(gtSysCommPoolDualSC230AI[0]);
        self->tCommonArgs.pPoolCfg = gtSysCommPoolDualSC230AI;
        self->gCams[0].stSnsClkAttr.nSnsClkIdx = 0; /* mclk0 only by AX DEMO board, User defined */
        self->gCams[1].stSnsClkAttr.nSnsClkIdx = 1; /* mclk1 only by AX DEMO board, User defined */
        self->gCams[0].stPipeAttr.ePipeDataSrc = AX_PIPE_SOURCE_DEV_ONLINE;
        self->gCams[0].stChnAttr.tChnAttr[0].nDepth = 1;
        self->gCams[0].stChnAttr.tChnAttr[1].nDepth = 1;
        self->gCams[0].stChnAttr.tChnAttr[2].nDepth = 1;
        self->gCams[1].stPipeAttr.ePipeDataSrc = AX_PIPE_SOURCE_DEV_ONLINE;
        self->gCams[0].stPipeAttr.eDevSource = AX_DEV_SOURCE_SNS_0;
        self->gCams[1].stPipeAttr.eDevSource = AX_DEV_SOURCE_SNS_1;
        self->gCams[0].stPipeAttr.ePreOutput = AX_PRE_OUTPUT_FULL_MAIN;
        self->gCams[1].stPipeAttr.ePreOutput = AX_PRE_OUTPUT_LITE_MAIN;
        self->gCams[1].stChnAttr.tChnAttr[0].nDepth = 1;
        self->gCams[1].stChnAttr.tChnAttr[1].nDepth = 1;
        self->gCams[1].stChnAttr.tChnAttr[2].nDepth = 1;
        self->gCams[0].stSnsAttr.eMasterSlaveSel = 2;
        self->gCams[1].stSnsAttr.eMasterSlaveSel = 3;
        for (int i = 0; i < self->tCommonArgs.nCamCnt; i++)
        {
            self->gCams[i].stSnsAttr.eSnsMode = self->eHdrMode;
            self->gCams[i].stDevAttr.eSnsMode = self->eHdrMode;
            self->gCams[i].stPipeAttr.eSnsMode = self->eHdrMode;
            if (i == 0)
            {
                self->gCams[i].nDevId = 0;
                self->gCams[i].nRxDev = AX_MIPI_RX_DEV_0;
                self->gCams[i].nPipeId = 0;
            }
            else if (i == 1)
            {
                self->gCams[i].nDevId = 1;
                self->gCams[i].nRxDev = AX_MIPI_RX_DEV_1;
                self->gCams[i].nPipeId = 1;
            }
        }
    }

    else if (self->eSysCase == SYS_CASE_OS04A10_AND_YUV_ONLINE)
    {
        self->tCommonArgs.nCamCnt = 2;
        self->gCams[0].eSnsType = OMNIVISION_OS04A10;
        self->gCams[1].eSnsType = MIPI_AHD_YUV;
        COMMON_ISP_GetSnsConfig(OMNIVISION_OS04A10, &self->gCams[0].stSnsAttr, &self->gCams[0].stSnsClkAttr, &self->gCams[0].stDevAttr,
                                &self->gCams[0].stPipeAttr,
                                &self->gCams[0].stChnAttr);
        COMMON_ISP_GetSnsConfig(MIPI_AHD_YUV, &self->gCams[1].stSnsAttr, &self->gCams[1].stSnsClkAttr, &self->gCams[1].stDevAttr,
                                &self->gCams[1].stPipeAttr,
                                &self->gCams[1].stChnAttr);
        self->tCommonArgs.nPoolCfgCnt = sizeof(gtSysCommPoolOs04a10AndAhd) / sizeof(gtSysCommPoolOs04a10AndAhd[0]);
        self->tCommonArgs.pPoolCfg = gtSysCommPoolOs04a10AndAhd;

        self->gCams[0].stPipeAttr.ePipeDataSrc = AX_PIPE_SOURCE_DEV_ONLINE;
        self->gCams[1].stPipeAttr.ePipeDataSrc = AX_PIPE_SOURCE_DEV_OFFLINE;
        self->gCams[0].stPipeAttr.eDevSource = AX_DEV_SOURCE_SNS_0;
        self->gCams[1].stPipeAttr.eDevSource = AX_DEV_SOURCE_SNS_1;
        self->gCams[0].stPipeAttr.ePreOutput = AX_PRE_OUTPUT_FULL_MAIN;
        self->gCams[1].stPipeAttr.ePreOutput = AX_PRE_OUTPUT_LITE_MAIN;
        self->gCams[0].stSnsClkAttr.nSnsClkIdx = 0; /* mclk0 only by AX DEMO board, User defined */
        self->gCams[1].stSnsClkAttr.nSnsClkIdx = 2; /* mclk2 only by AX DEMO board, User defined */
    }
    else if (SYS_CASE_IMX327_IMX327_AND_YUV_ONLINE == self->eSysCase)
    {
        self->tCommonArgs.nCamCnt = 3;
        self->gCams[0].eSnsType = SONY_IMX327;
        COMMON_ISP_GetSnsConfig(self->gCams[0].eSnsType, &self->gCams[0].stSnsAttr, &self->gCams[0].stSnsClkAttr, &self->gCams[0].stDevAttr,
                                &self->gCams[0].stPipeAttr,
                                &self->gCams[0].stChnAttr);
        self->gCams[1].eSnsType = SONY_IMX327;
        COMMON_ISP_GetSnsConfig(self->gCams[1].eSnsType, &self->gCams[1].stSnsAttr, &self->gCams[1].stSnsClkAttr, &self->gCams[1].stDevAttr,
                                &self->gCams[1].stPipeAttr,
                                &self->gCams[1].stChnAttr);
        self->gCams[2].eSnsType = MIPI_AHD_YUV;
        COMMON_ISP_GetSnsConfig(self->gCams[2].eSnsType, &self->gCams[2].stSnsAttr, &self->gCams[2].stSnsClkAttr, &self->gCams[2].stDevAttr,
                                &self->gCams[2].stPipeAttr,
                                &self->gCams[2].stChnAttr);

        self->tCommonArgs.nPoolCfgCnt = sizeof(gtSysCommPoolIMX327_IMX327_AHDYUV) / sizeof(gtSysCommPoolIMX327_IMX327_AHDYUV[0]);
        self->tCommonArgs.pPoolCfg = gtSysCommPoolIMX327_IMX327_AHDYUV;

        self->gCams[0].stSnsClkAttr.nSnsClkIdx = 0; /* mclk0 only by AX DEMO board, User defined */
        self->gCams[1].stSnsClkAttr.nSnsClkIdx = 1; /* mclk1 only by AX DEMO board, User defined */
        self->gCams[2].stSnsClkAttr.nSnsClkIdx = 2; /* mclk2 only by AX DEMO board, User defined */
        self->eHdrMode = AX_SNS_LINEAR_MODE;
        self->gCams[0].stPipeAttr.ePipeDataSrc = AX_PIPE_SOURCE_DEV_ONLINE;
        self->gCams[0].stPipeAttr.ePreOutput = AX_PRE_OUTPUT_FULL_MAIN;

        self->gCams[1].stPipeAttr.ePipeDataSrc = AX_PIPE_SOURCE_DEV_ONLINE;
        self->gCams[1].stPipeAttr.ePreOutput = AX_PRE_OUTPUT_LITE_MAIN;

        self->gCams[2].stPipeAttr.ePipeDataSrc = AX_PIPE_SOURCE_DEV_OFFLINE;
        self->gCams[2].stPipeAttr.ePreOutput = AX_PRE_OUTPUT_FULL_MAIN;
    }
    else if (self->eSysCase == SYS_CASE_SINGLE_OS04A10_DCG_HDR)
    {
        self->tCommonArgs.nCamCnt = 1;
        self->gCams[0].eSnsType = OMNIVISION_OS04A10;
        COMMON_ISP_GetSnsConfig(OMNIVISION_OS04A10, &self->gCams[0].stSnsAttr, &self->gCams[0].stSnsClkAttr, &self->gCams[0].stDevAttr,
                                &self->gCams[0].stPipeAttr, &self->gCams[0].stChnAttr);

        self->tCommonArgs.nPoolCfgCnt = sizeof(gtSysCommPoolSingleOs04a10Hdr) / sizeof(gtSysCommPoolSingleOs04a10Hdr[0]);
        self->tCommonArgs.pPoolCfg = gtSysCommPoolSingleOs04a10Hdr;
    }

    for (int i = 0; i < self->tCommonArgs.nCamCnt; i++)
    {
        AX_VIN_CHN_ATTR_T *tmp_pChnAttr = &self->gCams[i].stChnAttr;
        for (int ix = 0; ix < AX_YUV_SOURCE_ID_MAX; ix++)
        {
            AX_VIN_CHN_DEV_T *tmp_p = &tmp_pChnAttr->tChnAttr[ix];
            tmp_p->nWidth = self->tCommonArgs.pPoolCfg[ix + 2].nWidth;
            tmp_p->nHeight = self->tCommonArgs.pPoolCfg[ix + 2].nHeight;
            tmp_p->nWidthStride = self->tCommonArgs.pPoolCfg[ix + 2].nWidthStride;
            tmp_p->eImgFormat = self->tCommonArgs.pPoolCfg[ix + 2].nFmt;
        }

        if (SYS_CASE_SINGLE_OS04A10_SPLITE_OFFLINE == self->eSysCase || SYS_CASE_SINGLE_OS08A20_SPLITE_ONLINE == self->eSysCase)
        {
            self->gCams[i].stSnsAttr.eSnsMode = self->eHdrMode;
            self->gCams[i].stDevAttr.eSnsMode = self->eHdrMode;
            self->gCams[i].stPipeAttr.eSnsMode = AX_SNS_LINEAR_MODE;
        }
        else if (SYS_CASE_OS04A10_AND_YUV_ONLINE == self->eSysCase)
        {
            self->gCams[0].stSnsAttr.eSnsMode = AX_SNS_HDR_2X_MODE;
            self->gCams[0].stDevAttr.eSnsMode = AX_SNS_HDR_2X_MODE;
            self->gCams[0].stPipeAttr.eSnsMode = AX_SNS_HDR_2X_MODE;

            self->gCams[1].stSnsAttr.eSnsMode = AX_SNS_LINEAR_MODE;
            self->gCams[1].stDevAttr.eSnsMode = AX_SNS_LINEAR_MODE;
            self->gCams[1].stPipeAttr.eSnsMode = AX_SNS_LINEAR_MODE;
        }
        else if (SYS_CASE_SINGLE_OS04A10_DCG_HDR == self->eSysCase)
        {
            self->gCams[i].stSnsAttr.eSnsMode = AX_SNS_HDR_2X_MODE;
            self->gCams[i].stSnsAttr.eSnsOutputMode = AX_SNS_DCG_HDR;
            self->gCams[i].stDevAttr.eSnsMode = AX_SNS_HDR_2X_MODE;
            self->gCams[i].stPipeAttr.eSnsMode = AX_SNS_HDR_2X_MODE;
        }
        else
        {
            self->gCams[i].stSnsAttr.eSnsMode = self->eHdrMode;
            self->gCams[i].stDevAttr.eSnsMode = self->eHdrMode;
            self->gCams[i].stPipeAttr.eSnsMode = self->eHdrMode;
        }

        if (SYS_CASE_SINGLE_OS04A10_SPLITE_OFFLINE == self->eSysCase || SYS_CASE_SINGLE_OS08A20_SPLITE_ONLINE == self->eSysCase)
        {
            self->gCams[0].ePipeWorkMode = AX_VIN_PIPE_MODE_SPLIT_LONG_FRAME;
            self->gCams[1].ePipeWorkMode = AX_VIN_PIPE_MODE_SPLIT_SHORT_FRAME;
            self->gCams[i].nPipeId = i;
            self->gCams[i].nDevId = 0;
            self->gCams[i].nRxDev = AX_MIPI_RX_DEV_0;
            /*frameRateCtrl*/
            self->gCams[i].stPipeAttr.tFrameRateCtrl.nSrcFrameRate = 30;
            if (i == 0)
            {
                self->gCams[i].stPipeAttr.tFrameRateCtrl.nDstFrameRate = 12;
            }
            else
            {
                if (SYS_CASE_SINGLE_OS08A20_SPLITE_ONLINE == self->eSysCase)
                {
                    self->gCams[i].stPipeAttr.tFrameRateCtrl.nDstFrameRate = 20;
                }
                else
                {
                    self->gCams[i].stPipeAttr.tFrameRateCtrl.nDstFrameRate = 30;
                }
            }
        }
        else if (SYS_CASE_SINGLE_YUV == self->eSysCase)
        {
            self->gCams[i].ePipeWorkMode = AX_VIN_PIPE_MODE_NORMAL;
            if (i == 0)
            {
                self->gCams[i].nDevId = 2;
                self->gCams[i].nRxDev = AX_MIPI_RX_DEV_2;
                self->gCams[i].nPipeId = 2;
            }
        }
        else if (SYS_CASE_IMX327_IMX327_AND_YUV == self->eSysCase || SYS_CASE_FOUR_SC230AI == self->eSysCase || SYS_CASE_TRIPLE_IMX327 == self->eSysCase || SYS_CASE_FOUR_IMX327 == self->eSysCase || SYS_CASE_IMX327_IMX327_AND_YUV_ONLINE == self->eSysCase)
        {
            self->gCams[i].ePipeWorkMode = AX_VIN_PIPE_MODE_NORMAL;
            self->gCams[i].nDevId = i;
            self->gCams[i].nRxDev = i;
            self->gCams[i].nPipeId = i;
        }
        else if (SYS_CASE_OS04A10_AND_MIPITX422 == self->eSysCase)
        {
            self->gCams[i].ePipeWorkMode = AX_VIN_PIPE_MODE_NORMAL;
            self->gCams[i].nDevId = 0;
            self->gCams[i].nRxDev = AX_MIPI_RX_DEV_0;
            self->gCams[i].nTxDev = AX_MIPI_TX_DEV_0;
            self->gCams[i].nPipeId = 0;
        }
        else if (SYS_CASE_OS04A10_YUV422_RX == self->eSysCase)
        {
            self->gCams[i].ePipeWorkMode = AX_VIN_PIPE_MODE_NORMAL;
            self->gCams[i].nDevId = 0;
            self->gCams[i].nRxDev = AX_MIPI_RX_DEV_0;
            self->gCams[i].nPipeId = 0;
        }
        else if (SYS_CASE_DUAL_OS04A10_ONLINE == self->eSysCase || SYS_CASE_DUAL_IMX327_ONLINE_U == self->eSysCase || SYS_CASE_SYNC_DUAL_OS04A10_PTS == self->eSysCase || SYS_CASE_SYNC_DUAL_SC230AI_PTS == self->eSysCase)
        {
            self->gCams[i].ePipeWorkMode = AX_VIN_PIPE_MODE_NORMAL;
            if (i == 0)
            {
                self->gCams[i].nDevId = 0;
                self->gCams[i].nRxDev = AX_MIPI_RX_DEV_0;
                self->gCams[i].nPipeId = 0;
            }
            else if (i == 1)
            {
                self->gCams[i].nDevId = 1;
                self->gCams[i].nRxDev = AX_MIPI_RX_DEV_1;
                self->gCams[i].nPipeId = 1;
            }
        }
        else
        {
            self->gCams[i].ePipeWorkMode = AX_VIN_PIPE_MODE_NORMAL;
            if (i == 0)
            {
                self->gCams[i].nDevId = 0;
                self->gCams[i].nRxDev = AX_MIPI_RX_DEV_0;
                self->gCams[i].nPipeId = 0;
            }
            else if (i == 1)
            {
                self->gCams[i].nDevId = 2;
                self->gCams[i].nRxDev = AX_MIPI_RX_DEV_2;
                self->gCams[i].nPipeId = 2;
            }
        }
    }
}



static void set_Sensor_mode_par_0(struct ax_sensor_hal_t *self)
{
    // self->eSysCase = SYS_CASE_SINGLE_OS04A10_ONLINE;
    self->eHdrMode = AX_SNS_LINEAR_MODE;
    self->eSnsDataOutMode = AX_SNS_HDR_WITH_IN_MODE;
    self->ePipeDataSrc = AX_PIPE_SOURCE_DEV_ONLINE;
    ax_sernsor_cam_init(self);
    // self->gCams[0].stChnAttr.tChnAttr[0].nDepth = 0;
    // self->gCams[0].stChnAttr.tChnAttr[0].bEnable = 0;
    // self->gCams[0].stChnAttr.tChnAttr[1].nDepth = 0;
    // self->gCams[0].stChnAttr.tChnAttr[1].bEnable = 0;
}
static void set_Sensor_mode_par_1(struct ax_sensor_hal_t *self)
{
    // self->eSysCase = SYS_CASE_SINGLE_OS04A10_ONLINE;
    self->eHdrMode = AX_SNS_LINEAR_MODE;
    self->eSnsDataOutMode = AX_SNS_HDR_WITH_IN_MODE;
    self->ePipeDataSrc = AX_PIPE_SOURCE_DEV_OFFLINE;
    ax_sernsor_cam_init(self);
    // self->gCams[0].stChnAttr.tChnAttr[0].nDepth = 0;
    // self->gCams[0].stChnAttr.tChnAttr[0].bEnable = 0;
    // self->gCams[0].stChnAttr.tChnAttr[1].nDepth = 0;
    // self->gCams[0].stChnAttr.tChnAttr[1].bEnable = 0;
}

static void *IspRun(void *par)
{
    CAMERA_T *self = (CAMERA_T *)par;
    // COMM_ISP_PRT("cam %d is running...\n", i);
    while (self->bOpen)
    {
        AX_ISP_Run(self->nPipeId);
    }
    return NULL;
}

static int ax_OpenSensor(struct ax_sensor_hal_t *self, int sensor_id, int mode)
{
    AX_S32 axRet;
    // self->sensor_id = sensor_id;
    self->eSysCase = sensor_id;
    self->set_Sensor_mode_par[mode](self);

    self->sNpuAttr.eHardMode = AX_NPU_VIRTUAL_1_1;
    axRet = AX_NPU_SDK_EX_Init_with_attr(&self->sNpuAttr);
    if (0 != axRet)
    {
        COMM_ISP_PRT("AX_NPU_SDK_EX_Init_with_attr failed, ret=0x%x.\n", axRet);
        return -1;
    }
    self->status = AX_SENSOR_AI_ISP;
    axRet = COMMON_SYS_Init(&self->tCommonArgs);
    if (axRet)
    {
        COMM_ISP_PRT("isp sys init fail\n");
        return -2;
    }
    self->status = AX_SENSOR_SYS;
    COMMON_CAM_Init();
    if (self->eSysCase == SYS_CASE_OS04A10_AND_MIPITX422)
    {
        axRet = COMMON_ISP_InitTx();
        if (0 != axRet)
        {
            COMM_ISP_PRT(" failed, ret=0x%x.\n", axRet);
            return -2;
        }
    }
    self->status = AX_SENSOR_CAM_INIT;
    for (int i = 0; i < self->tCommonArgs.nCamCnt; i++)
    {
        if ((MIPI_YUV == self->gCams[i].eSnsType) || (SENSOR_DVP == self->gCams[i].eSnsType) || (SENSOR_BT601 == self->gCams[i].eSnsType) || (SENSOR_BT656 == self->gCams[i].eSnsType) || (SENSOR_BT1120 == self->gCams[i].eSnsType) || (OMNIVISION_OS04A10_YUV422_RX == self->gCams[i].eSnsType))
        {
            axRet = COMMON_CAM_DVP_Open(&self->gCams[i]);
        }
        else if ((MIPI_YUV_THERMAL == self->gCams[i].eSnsType) || (MIPI_AHD_YUV == self->gCams[i].eSnsType))
        {
            axRet = COMMON_CAM_Thermal_Open(&self->gCams[i]);
        }
        else
        {
            axRet = COMMON_CAM_Open(&self->gCams[i]);
        }
        if (axRet)
        {
            return -3;
        }
        self->gCams[i].bOpen = AX_TRUE;
        COMM_ISP_PRT("camera %d is open\n", i);
    }
    self->status = AX_SENSOR_CAM_OPEN;
#ifdef TUNING_CTRL
    /* Net Preview */
    COMM_ISP_PRT("Start the service on the tuning device side.\n");

    axRet = AX_NT_StreamInit(6000);
    if (0 != axRet)
    {
        COMM_ISP_PRT("AX_NT_StreamInit failed, ret=0x%x.\n", axRet);
        return -1;
    }
    axRet = AX_NT_CtrlInit(8082);
    if (0 != axRet)
    {
        COMM_ISP_PRT("AX_NT_CtrlInit failed, ret=0x%x.\n", axRet);
        return -1;
    }

    for (i = 0; i < self->tCommonArgs.nCamCnt; i++)
    {
        AX_NT_SetStreamSource(self->gCams[i].nPipeId);
    }
    COMM_ISP_PRT("tuning runing.\n");
#endif
    self->status = AX_SENSOR_TUNING_CTRL;
    // self->g_isp_force_loop_exit = 0;
    for (int i = 0; i < MAX_CAMERAS; i++)
    {
        if (self->gCams[i].bOpen)
        {
            pthread_create(&self->gCams[i].tIspProcThread, NULL, IspRun, &self->gCams[i]);
            for (int i = 0; i < AX_MAX_SENSOR_CHN_; i++)
            {
                self->dev[i].nPipeId = self->gCams[i].nPipeId;
            }
        }
        COMM_ISP_PRT("get self->gCams[i].eSnsType:%d", self->gCams[i].eSnsType);
    }
    self->status = AX_SENSOR_ISP_RUN;
    return 0;
}
static int ax_CloseSensor(struct ax_sensor_hal_t *self)
{
    if (self->status == AX_SENSOR_ISP_RUN)
    {
        for (int i = 0; i < MAX_CAMERAS; i++)
        {
            if (self->gCams[i].bOpen)
            {
                self->gCams[i].bOpen = AX_FALSE;
                // if(OMNIVISION_OS04A10_AND_MIPITX422 == self->gCams[i].eSnsType) {
                //     AxMipiTx422ThreadExit(&self->gCams[i]);
                // }
                pthread_cancel(self->gCams[i].tIspProcThread);
                pthread_join(self->gCams[i].tIspProcThread, NULL);
                self->gCams[i].bOpen = AX_TRUE;
            }
        }
        self->status = AX_SENSOR_TUNING_CTRL;
    }

#ifdef SAMPLE_IRCUT
    axRet = pthread_join(self->gCams[0].tIspIrThread, NULL);
    if (axRet < 0)
    {
        COMM_ISP_PRT(" isp ir run thread exit failed, ret=0x%x.\n", axRet);
    }
#endif

    if (self->status == AX_SENSOR_TUNING_CTRL)
    {
#ifdef TUNING_CTRL
        AX_NT_CtrlDeInit();
        AX_NT_StreamDeInit();
#endif
        self->status = AX_SENSOR_CAM_OPEN;
    }
    if (self->status == AX_SENSOR_CAM_OPEN)
    {
        for (int i = self->tCommonArgs.nCamCnt; i >= 0; i--)
        {
            if (self->gCams[i].bOpen)
            {
                COMM_ISP_PRT("COMMON_CAM_Close\n");
                COMMON_CAM_Close(&self->gCams[i]);
            }
        }
        self->status = AX_SENSOR_CAM_INIT;
    }

    if (self->status == AX_SENSOR_CAM_INIT)
    {
        COMMON_CAM_Deinit();
        self->status = AX_SENSOR_SYS;
    }
    if (self->status == AX_SENSOR_CAM_INIT)
    {
        COMMON_SYS_DeInit();
        self->status = AX_SENSOR_AI_ISP;
    }
    self->status = AX_SENSOR_NONT;
    return 0;
}

static void *get_img_thread(void *par)
{
    ax_sensor_dev_info *self = (ax_sensor_dev_info *)par;

    while (self->status)
    {
        // AX_YUV_SOURCE_ID_E ni = AX_YUV_SOURCE_ID_MAIN;
        AX_S32 axRet = AX_VIN_GetYuvFrame(self->nPipeId, (AX_YUV_SOURCE_ID_E)self->Chn, &self->ax_img, 500);
        if (axRet == 0)
        {
            sernsor_calculateFrameRate(self);

            self->ax_img.tFrameInfo.stVFrame.u64VirAddr[0] = (AX_U32)AX_POOL_GetBlockVirAddr(self->ax_img.tFrameInfo.stVFrame.u32BlkId[0]);
            self->ax_img.tFrameInfo.stVFrame.u64PhyAddr[0] = AX_POOL_Handle2PhysAddr(self->ax_img.tFrameInfo.stVFrame.u32BlkId[0]);

            // Sensor_Frame buff;
            // buff.pipeid = self->nPipeId;
            // buff.n_width = self->ax_img.tFrameInfo.stVFrame.u32Width;
            // buff.n_height = self->ax_img.tFrameInfo.stVFrame.u32Height;
            // buff.n_stride = self->ax_img.tFrameInfo.stVFrame.u32PicStride;
            // buff.n_size = self->ax_img.tFrameInfo.stVFrame.u32FrameSize;
            // buff.p_vir = self->ax_img.tFrameInfo.stVFrame.u64VirAddr[0];
            // buff.p_phy = self->ax_img.tFrameInfo.stVFrame.u64PhyAddr[0];
            // buff.m_output_type = self->ax_img.tFrameInfo.stVFrame.enImgFormat;
            if (self->out_farm)
            {
                // self->out_farm(&buff);
                self->out_farm(&self->ax_img);
            }
            AX_VIN_ReleaseYuvFrame(self->nPipeId, self->Chn, &self->ax_img);
        }
        else
        {
            ALOGD("get ax img error! code:0x%x", axRet);
            usleep(10 * 1000);
        }
    }
    return NULL;
}

static int ax_SetSensorOut(struct ax_sensor_hal_t *self, int Chn, void (*out_farm)(Sensor_Frame *))
{
    self->dev[Chn].out_farm = out_farm;
}

static void ax_start(struct ax_sensor_hal_t *self, int Chn)
{
    if(self->status == AX_SENSOR_ISP_RUN)
    {
        self->dev[Chn].status = 1;
        self->dev[Chn].Chn = Chn;
        pthread_create(&self->dev[Chn].farm_pthread_p, NULL, get_img_thread, &self->dev[Chn]);
    }
}
static void ax_stop(struct ax_sensor_hal_t *self, int Chn)
{
    self->dev[Chn].status = 0;
    // pthread_cancel(self->dev[Chn].farm_pthread_p);
    pthread_join(self->dev[Chn].farm_pthread_p, NULL);
}

static int ax_GetFrameRate(struct ax_sensor_hal_t* self, int chn)
{
    return self->dev[chn].sernsor_frame_rate;
}


static int private_flage = 0;
int ax_create_sensor(ax_sensor_hal *sensor_dev)
{
    AX_S32 axRet;
    if (private_flage)
        return -1;
    memset(sensor_dev, 0, sizeof(ax_sensor_hal));
    private_flage = 1;
    sensor_dev->OpenSensor = ax_OpenSensor;
    sensor_dev->CloseSensor = ax_CloseSensor;
    sensor_dev->SetSensorOut = ax_SetSensorOut;
    sensor_dev->start = ax_start;
    sensor_dev->stop = ax_stop;
    sensor_dev->GetFrameRate = ax_GetFrameRate;

    sensor_dev->set_Sensor_mode_par[0] = set_Sensor_mode_par_0;
    sensor_dev->set_Sensor_mode_par[1] = set_Sensor_mode_par_1;

    return 0;
}
void ax_destroy_sensor(ax_sensor_hal *sensor_dev)
{
    if (private_flage == 0)
        return;
    for (int i = 0; i < sizeof(sensor_dev->dev) / sizeof(sensor_dev->dev[0]); i++)
    {
        if (sensor_dev->dev[i].status)
        {
            sensor_dev->stop(sensor_dev, i);
        }
    }
    if(sensor_dev->status != AX_SENSOR_NONT)
    {
        sensor_dev->CloseSensor(sensor_dev);
    }
    private_flage = 0;
}
