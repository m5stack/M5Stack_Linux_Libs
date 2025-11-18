/**************************************************************************************************
 *
 * Copyright (c) 2019-2024 Axera Semiconductor Co., Ltd. All Rights Reserved.
 *
 * This source file is the property of Axera Semiconductor Co., Ltd. and
 * may not be copied or distributed in any isomorphic form without the prior
 * written consent of Axera Semiconductor Co., Ltd.
 *
 **************************************************************************************************/

#include "sample_vdec.hpp"
#include <string.h>
#include <sys/time.h>
#include <time.h>
#include <unistd.h>
#include "ax_buffer_tool.h"
#include "axcl_base.h"

static AX_U64 get_ticks() {
    struct timespec ts;
    clock_gettime(CLOCK_MONOTONIC, &ts);
    return (ts.tv_sec * 1000 + ts.tv_nsec / 1000000);
}

AX_U32 sample_vdec_calc_blk_size(AX_U32 width, AX_U32 height, AX_PAYLOAD_TYPE_E payload, AX_FRAME_COMPRESS_INFO_T *fbc,
                                 AX_IMG_FORMAT_E pix_fmt) {
    return AX_VDEC_GetPicBufferSize(width, height, pix_fmt, fbc, payload);
}

AX_S32 sample_vdec_init() {
    AX_VDEC_MOD_ATTR_T attr;
    memset(&attr, 0, sizeof(attr));
    attr.u32MaxGroupCount = 32;
    attr.enDecModule = AX_ENABLE_ONLY_VDEC;
    return AXCL_VDEC_Init(&attr);
}

AX_S32 sample_vdec_deinit() {
    return AXCL_VDEC_Deinit();
}

static AX_S32 sample_vdec_create_grp(AX_VDEC_GRP grp, const SAMPLE_VDEC_ATTR *attr) {
    axclError ret;
    if (ret = AXCL_VDEC_CreateGrp(grp, &attr->grp_attr); AXCL_SUCC != ret) {
        return ret;
    }

    do {
        AX_VDEC_GRP_PARAM_T param;
        memset(&param, 0, sizeof(param));
        param.stVdecVideoParam.enVdecMode = attr->decoded_mode;
        param.stVdecVideoParam.enOutputOrder = attr->output_order;
        if (ret = AXCL_VDEC_SetGrpParam(grp, &param); AXCL_SUCC != ret) {
            break;
        }

        if (ret = AXCL_VDEC_SetDisplayMode(grp, attr->display_mode); AXCL_SUCC != ret) {
            break;
        }

        for (AX_VDEC_CHN chn = 0; chn < AX_VDEC_MAX_CHN_NUM; ++chn) {
            if (attr->chn_attr[chn].enable) {
                if (ret = AXCL_VDEC_SetChnAttr(grp, chn, &attr->chn_attr[chn].attr); AXCL_SUCC != ret) {
                    break;
                }

                if (ret = AXCL_VDEC_EnableChn(grp, chn); AXCL_SUCC != ret) {
                    break;
                }
            }
        }

        if (AXCL_SUCC != ret) {
            break;
        }

        return 0;

    } while (0);

    AXCL_VDEC_DestroyGrp(grp);
    return ret;
}

static AX_S32 sample_vdec_destory_grp(AX_VDEC_GRP grp) {
    axclError ret;
    for (AX_VDEC_CHN chn = 0; chn < AX_VDEC_MAX_CHN_NUM; ++chn) {
        if (ret = AXCL_VDEC_DisableChn(grp, chn); AXCL_SUCC != ret) {
            return ret;
        }
    }

    if (ret = AXCL_VDEC_DestroyGrp(grp); AXCL_SUCC != ret) {
        return ret;
    }

    return 0;
}

AX_S32 sample_vdec_start(AX_VDEC_GRP grp, const SAMPLE_VDEC_ATTR *attr) {
    if (AX_S32 ret = sample_vdec_create_grp(grp, attr); 0 != ret) {
        return ret;
    }

    AX_VDEC_RECV_PIC_PARAM_T param;
    memset(&param, 0, sizeof(param));
    param.s32RecvPicNum = -1;
    if (axclError ret = AXCL_VDEC_StartRecvStream(grp, &param); AXCL_SUCC != ret) {
        sample_vdec_destory_grp(grp);
        return ret;
    }

    return 0;
}

AX_S32 sample_vdec_stop(AX_VDEC_GRP grp) {
    axclError ret;
    if (ret = AXCL_VDEC_StopRecvStream(grp); AXCL_SUCC != ret) {
        return ret;
    }

    int32_t retry = 0;
    while (++retry <= 10) {
        if (ret = AXCL_VDEC_ResetGrp(grp); AXCL_SUCC != ret) {
            usleep(40 * 1000);
            continue;
        } else {
            break;
        }
    }

    return sample_vdec_destory_grp(grp);
}

AX_S32 sample_vdec_send_stream(AX_VDEC_GRP grp, const AX_VDEC_STREAM_T *stream, AX_S32 timeout) {
    return AXCL_VDEC_SendStream(grp, stream, timeout);
}

AX_S32 sample_vdec_get_frame(AX_VDEC_GRP grp, AX_VDEC_CHN chn, AX_VIDEO_FRAME_INFO_T *frame, AX_S32 timeout) {
    return AXCL_VDEC_GetChnFrame(grp, chn, frame, timeout);
}

AX_S32 sample_vdec_release_frame(AX_VDEC_GRP grp, AX_VDEC_CHN chn, const AX_VIDEO_FRAME_INFO_T *frame) {
    return AXCL_VDEC_ReleaseChnFrame(grp, chn, frame);
}

AX_S32 sample_vdec_wait_decoded_eof(AX_VDEC_GRP grp, AX_S32 timeout) {
    AX_U64 start = get_ticks();
    do {
        AX_VDEC_GRP_STATUS_T status;
        if (axclError ret = AXCL_VDEC_QueryStatus(grp, &status); AXCL_SUCC == ret) {
            if (0 == (status.u32LeftStreamFrames + status.u32LeftPics[0] + status.u32LeftPics[1] + status.u32LeftPics[2])) {
                return 0;
            }
        }

        if (0 != timeout) {
            usleep(100 * 1000);
        }

    } while (timeout < 0 || ((int32_t)(get_ticks() - start) < timeout));

    return -1;
}