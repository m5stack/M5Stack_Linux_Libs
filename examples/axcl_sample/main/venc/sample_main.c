/**************************************************************************************************
 *
 * Copyright (c) 2019-2024 Axera Semiconductor Co., Ltd. All Rights Reserved.
 *
 * This source file is the property of Axera Semiconductor Co., Ltd. and
 * may not be copied or distributed in any isomorphic form without the prior
 * written consent of Axera Semiconductor Co., Ltd.
 *
 **************************************************************************************************/

#include <errno.h>
#include <pthread.h>
#include <signal.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#include "common_venc.h"
#include "sample_cmd_params.h"
#include "sample_pool.h"
#include "common_venc_log.h"
#include "sample_global.h"
#include "axcl.h"


SAMPLE_VENC_CMD_PARA_T stCmdPara;

AX_VENC_MOD_ATTR_T stModAttr = {
    .enVencType = AX_VENC_MULTI_ENCODER,
    .stModThdAttr.u32TotalThreadNum = 1,
    .stModThdAttr.bExplicitSched = AX_FALSE,
};

extern AX_BOOL gVencLogLevel;

int main(int argc, char *argv[])
{
    SAMPLE_LOG("Build at %s %s\n", __DATE__, __TIME__);

    AX_S32 s32Ret = -1;

    SampleSetDefaultParams(&stCmdPara);

    s32Ret = SampleCmdLineParse(argc, argv, &stCmdPara);
    if (AX_SUCCESS != s32Ret) {
        SAMPLE_LOG_ERR("Invalid input argument!\n");
        return -1;
    }

    /* axcl init */
    if (stCmdPara.json) {
        SAMPLE_LOG_INFO("json: %s\n", stCmdPara.json);
    }
    s32Ret = axclInit(stCmdPara.json);
    if (AXCL_SUCC != s32Ret) {
        SAMPLE_LOG_ERR("axcl init err, ret=0x%x.\n", s32Ret);
        return -1;
    }

    axclrtDeviceList deviceList;
    s32Ret = axclrtGetDeviceList(&deviceList);
    if (AXCL_SUCC != s32Ret || 0 == deviceList.num) {
        SAMPLE_LOG_ERR("no device is connected\n");
        axclFinalize();
        return -1;
    }

    if (stCmdPara.devId < 0 || stCmdPara.devId >= (AX_S32)deviceList.num) {
        SAMPLE_LOG_ERR("device index %d is out of connected device num %d\n", stCmdPara.devId, deviceList.num);
        axclFinalize();
        return -1;
    }

    SAMPLE_LOG_INFO("device index: %d, bus number: %d\n", stCmdPara.devId, deviceList.devices[stCmdPara.devId]);
    stCmdPara.devId = deviceList.devices[stCmdPara.devId];

    s32Ret = axclrtSetDevice(stCmdPara.devId);
    if (AXCL_SUCC != s32Ret) {
        SAMPLE_LOG_ERR("axclrtSetDevice err, ret=0x%x.\n", s32Ret);
        goto FREE_AXCL;
    }

    gVencLogLevel = stCmdPara.logLevel;
    s32Ret = SampleMemInit(&stCmdPara);
    if (s32Ret) {
        SAMPLE_LOG_ERR("sample memory init err.\n");
        goto RESET_DEV;
    }

    s32Ret = AXCL_VENC_Init(&stModAttr);
    if (AX_SUCCESS != s32Ret) {
        SAMPLE_LOG_ERR("AXCL_VENC_Init error.\n");
        goto FREE_MEM;
    }

    SampleEncode(&stCmdPara);

    s32Ret = AXCL_VENC_Deinit();
    if (AX_SUCCESS != s32Ret)
        SAMPLE_LOG_ERR("AXCL_VENC_Deinit failed! Error Code:0x%X\n", s32Ret);

    SampleMemDeinit(&stCmdPara);

    axclrtResetDevice(stCmdPara.devId);
    axclFinalize();

    return 0;

FREE_MEM:
    SampleMemDeinit(&stCmdPara);
RESET_DEV:
    axclrtResetDevice(stCmdPara.devId);
FREE_AXCL:
    axclFinalize();

    return -1;
}
