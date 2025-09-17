/**************************************************************************************************
 *
 * Copyright (c) 2019-2024 Axera Semiconductor Co., Ltd. All Rights Reserved.
 *
 * This source file is the property of Axera Semiconductor Co., Ltd. and
 * may not be copied or distributed in any isomorphic form without the prior
 * written consent of Axera Semiconductor Co., Ltd.
 *
 **************************************************************************************************/

#pragma once

#include "axcl_vdec.h"

#define AX_SHIFT_LEFT_ALIGN(a) (1 << (a))
#define VDEC_STRIDE_ALIGN AX_SHIFT_LEFT_ALIGN(8) /* VDEC stride align 256 */

typedef struct {
    AX_BOOL enable;
    AX_VDEC_CHN_ATTR_T attr;
} SAMPLE_VDEC_CHN_ATTR;

typedef struct {
    AX_VDEC_MODE_E decoded_mode;
    AX_VDEC_OUTPUT_ORDER_E output_order;
    AX_VDEC_DISPLAY_MODE_E display_mode;
    AX_VDEC_GRP_ATTR_T grp_attr;
    SAMPLE_VDEC_CHN_ATTR chn_attr[AX_VDEC_MAX_CHN_NUM];
} SAMPLE_VDEC_ATTR;

typedef struct {
    AX_U32 u32ChnId;
    AX_U32 u32PicWidth;
    AX_U32 u32PicHeight;
} SAMPLE_VDEC_CHN_INFO;

AX_S32 sample_vdec_init();
AX_S32 sample_vdec_deinit();

AX_U32 sample_vdec_calc_blk_size(AX_U32 width, AX_U32 height, AX_PAYLOAD_TYPE_E payload, AX_FRAME_COMPRESS_INFO_T *fbc,
                                 AX_IMG_FORMAT_E pix_fmt);

AX_S32 sample_vdec_start(AX_VDEC_GRP grp, const SAMPLE_VDEC_ATTR *attr);
AX_S32 sample_vdec_stop(AX_VDEC_GRP grp);

AX_S32 sample_vdec_send_stream(AX_VDEC_GRP grp, const AX_VDEC_STREAM_T *stream, AX_S32 timeout);
AX_S32 sample_vdec_get_frame(AX_VDEC_GRP grp, AX_VDEC_CHN chn, AX_VIDEO_FRAME_INFO_T *frame, AX_S32 timeout);
AX_S32 sample_vdec_release_frame(AX_VDEC_GRP grp, AX_VDEC_CHN chn, const AX_VIDEO_FRAME_INFO_T *frame);

/**
 * wait all sent streams are decoded finish.
*/
AX_S32 sample_vdec_wait_decoded_eof(AX_VDEC_GRP grp, AX_S32 timeout);
