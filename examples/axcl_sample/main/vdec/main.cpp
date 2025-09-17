/**************************************************************************************************
 *
 * Copyright (c) 2019-2024 Axera Semiconductor Co., Ltd. All Rights Reserved.
 *
 * This source file is the property of Axera Semiconductor Co., Ltd. and
 * may not be copied or distributed in any isomorphic form without the prior
 * written consent of Axera Semiconductor Co., Ltd.
 *
 **************************************************************************************************/

#include <signal.h>
#include <stdio.h>
#include <chrono>
#include <functional>
#include <memory>
#include <string>
#include <thread>
#include "axcl.h"
#include "cmdline.h"
#include "demux/ffmpeg.hpp"
#include "dma_buffer.hpp"
#include "event.hpp"
#include "sample_vdec.hpp"
#include "sys/sample_sys.hpp"
#include "threadx.hpp"
#include "utils/def.h"
#include "utils/logger.h"

#define MAX_STREAM_COUNT (32)
static int g_exit = 0;
static void signal_handler(int s) {
    printf("\n====================== caught signal: %d ======================\n", s);
    g_exit = 1;
}
std::string output_to_file;

static void on_receive_demux_stream_data(const struct stream_data *data, uint64_t userdata);
static SAMPLE_VDEC_ATTR sample_get_vdec_attr_from_stream_info(const struct stream_info *info);
static void sample_get_decoded_image_thread(AX_VDEC_GRP grp, int32_t device_id, axcl::event *eof_event, SAMPLE_VDEC_CHN_INFO chn_info,
                                            int32_t dump);
static AX_S32 sample_vdec_set_attr(SAMPLE_VDEC_ATTR *vdec_attr, int32_t chn_id, int32_t w, int32_t h);

int main(int argc, char *argv[]) {
    SAMPLE_LOG_I("============== %s sample started %s %s ==============\n", AXCL_BUILD_VERSION, __DATE__, __TIME__);

    signal(SIGINT, signal_handler);

    cmdline::parser a;
    a.add<std::string>("url", 'i', "mp4|.264|.265 file path", true);
    a.add<uint32_t>("device", 'd', "device index from 0 to connected device num - 1", false, 0,
                    cmdline::range(0, AXCL_MAX_DEVICE_COUNT - 1));
    a.add<int32_t>("count", '\0', "grp count", false, 1, cmdline::range(1, MAX_STREAM_COUNT));
    a.add<std::string>("json", '\0', "axcl.json path", false, "./axcl.json");
    a.add<int32_t>("width", 'w', "frame width", false, 1920, cmdline::range(AX_VDEC_MIN_WIDTH, AX_VDEC_MAX_WIDTH));
    a.add<int32_t>("height", 'h', "frame height", false, 1080, cmdline::range(AX_VDEC_MIN_HEIGHT, AX_VDEC_MAX_HEIGHT));
    a.add<int32_t>("VdChn", '\0', "channel id", false, 0);
    a.add<int32_t>("yuv", '\0', "transfer nv12 from device", false, 0);
    a.add<std::string>("output", 'o', "output to file", false, "");
    a.parse_check(argc, argv);
    const uint32_t device_index = a.get<uint32_t>("device");
    const std::string url = a.get<std::string>("url");
    const int32_t count = a.get<int32_t>("count");
    const std::string json = a.get<std::string>("json");
    int32_t w = a.get<int32_t>("width");
    int32_t h = a.get<int32_t>("height");
    int32_t chn_id = a.get<int32_t>("VdChn");
    const int32_t dump = a.get<int32_t>("yuv");
    output_to_file = a.get<std::string>("output");
    /* step01: axcl initialize */
    SAMPLE_LOG_I("json: %s", json.c_str());
    if (axclError ret = axclInit(json.c_str()); AXCL_SUCC != ret) {
        SAMPLE_LOG_E("axcl init fail, ret = 0x%x", ret);
        return ret;
    }

    axclrtDeviceList device_list;
    if (axclError ret = axclrtGetDeviceList(&device_list); AXCL_SUCC != ret || 0 == device_list.num) {
        SAMPLE_LOG_E("no device is connected");
        axclFinalize();
        return ret;
    }

    if (device_index >= device_list.num) {
        SAMPLE_LOG_E("device index %d is out of connected device num %d", device_index, device_list.num);
        axclFinalize();
        return 1;
    }

    const int32_t device_id = device_list.devices[device_index];
    SAMPLE_LOG_I("device index: %d, bus number: %d", device_index, device_id);

    /* step02: active device */
    if (axclError ret = axclrtSetDevice(device_id); AXCL_SUCC != ret) {
        SAMPLE_LOG_E("active device, ret = 0x%x", ret);
        axclFinalize();
        return ret;
    }

    ffmpeg_demuxer demuxers[MAX_STREAM_COUNT];

    /* delegating demuxer memory destorier by smart pointer */
    std::function<void(void *)> demux_deleter = [](void *p) { ffmpeg_destory_demuxer(static_cast<ffmpeg_demuxer>(p)); };
    std::unique_ptr<void, std::function<void(void *)>> demuxers_sp[MAX_STREAM_COUNT];

    /* step03: create ffmpeg demuxers */
    for (int32_t i = 0; i < count; ++i) {
        const AX_VDEC_GRP grp = static_cast<AX_VDEC_GRP>(i);
        if (0 != ffmpeg_create_demuxer(&demuxers[i], url.c_str(), grp, device_id, {on_receive_demux_stream_data}, 0)) {
            axclrtResetDevice(device_id);
            axclFinalize();
            return -1;
        }

        demuxers_sp[i] = std::unique_ptr<void, std::function<void(void *)>>(demuxers[i], demux_deleter);
    }

    /* step04: init sys module */
    SAMPLE_LOG_I("init sys");
    if (AX_S32 ret = sample_sys_init(); 0 != ret) {
        SAMPLE_LOG_E("init sys, ret = 0x%x", ret);
        axclrtResetDevice(device_id);
        axclFinalize();
        return ret;
    }

    /* step05: init vdec module */
    SAMPLE_LOG_I("init vdec");
    if (AX_S32 ret = sample_vdec_init(); 0 != ret) {
        SAMPLE_LOG_E("init vdec, ret = 0x%x", ret);
        sample_sys_deinit();
        axclrtResetDevice(device_id);
        axclFinalize();
        return ret;
    }

    /* step06: start vdec */
    axcl::threadx decode_threads[MAX_STREAM_COUNT];
    axcl::event decode_eof_events[MAX_STREAM_COUNT];
    for (int32_t i = 0; i < count; ++i) {
        const AX_VDEC_GRP grp = static_cast<AX_VDEC_GRP>(i);
        SAMPLE_LOG_I("start decoder %d", grp);

        SAMPLE_VDEC_ATTR attr = sample_get_vdec_attr_from_stream_info(ffmpeg_get_stream_info(demuxers[i]));

        if (chn_id) {
            sample_vdec_set_attr(&attr, chn_id, w, h);
        }

        if (AX_S32 ret = sample_vdec_start(grp, &attr); 0 != ret) {
            SAMPLE_LOG_E("start vdec %d fail, ret = 0x%x", grp, ret);

            for (int32_t j = 0; j < i; ++j) {
                sample_vdec_stop(j);
            }

            sample_vdec_deinit();
            sample_sys_deinit();
            axclrtResetDevice(device_id);
            axclFinalize();
        }

        /* step07: start decoded get thread */
        char name[32];
        SAMPLE_VDEC_CHN_INFO chn_info;
        memset(&chn_info, 0, sizeof(chn_info));
        chn_info.u32ChnId = chn_id;
        for (int32_t j = 0; j < AX_VDEC_MAX_CHN_NUM; ++j) {
            if (attr.chn_attr[j].enable) {
                chn_info.u32PicWidth = attr.chn_attr[j].attr.u32PicWidth;
                chn_info.u32PicHeight = attr.chn_attr[j].attr.u32PicHeight;
                break;
            }
        }
        sprintf(name, "decode%d", grp);
        decode_threads[i].start(name, sample_get_decoded_image_thread, grp, device_id, &decode_eof_events[i], chn_info, dump);

        /* step08: start to demux video */
        SAMPLE_LOG_I("start demuxer %d", i);
        ffmpeg_start_demuxer(demuxers[i]);
    }

    int32_t eofs[MAX_STREAM_COUNT];
    memset(eofs, 0, sizeof(eofs));
    int32_t eof_count = 0;

    while (!g_exit) {
        /* wait video eof */
        for (int32_t i = 0; i < count; ++i) {
            if (eofs[i]) {
                continue;
            }

            if (0 == ffmpeg_wait_demuxer_eof(demuxers[i], 0)) {
                if (!decode_eof_events[i].wait(1000)) {
                    continue;
                }

                const AX_VDEC_GRP grp = static_cast<AX_VDEC_GRP>(i);
                SAMPLE_LOG_I("stop decoder %d", grp);
                sample_vdec_stop(grp);

                eofs[i] = 1;
                ++eof_count;
                SAMPLE_LOG_I("decoder %d is eof", grp);
            }
        }

        if (eof_count == count) {
            /* all streams and decoders are eof */
            g_exit = 1;
            break;
        }

        std::this_thread::sleep_for(std::chrono::seconds(1));
    }

    /* step09: stop vdec and demuxer */
    for (int32_t i = 0; i < count; ++i) {
        /**
         * bugfix:
         * Stop vdec first; otherwise `sample_vdec_send_stream(-1)` may hang.
         */
        if (!eofs[i]) {
            const AX_VDEC_GRP grp = static_cast<AX_VDEC_GRP>(i);
            SAMPLE_LOG_I("stop decoder %d", grp);
            sample_vdec_stop(grp);
        }

        SAMPLE_LOG_I("stop demuxer %d", i);
        ffmpeg_stop_demuxer(demuxers[i]);

        decode_threads[i].stop();
        decode_threads[i].join();
    }

    /* step10: deinit vdec module */
    SAMPLE_LOG_I("deinit vdec");
    sample_vdec_deinit();

    /* step11: deinit sys module */
    SAMPLE_LOG_I("deinit sys");
    sample_sys_deinit();

    /* step12: deinit axcl */
    SAMPLE_LOG_I("axcl deinit");
    axclrtResetDevice(device_id);
    axclFinalize();

    SAMPLE_LOG_I("============== %s sample exited %s %s ==============\n", AXCL_BUILD_VERSION, __DATE__, __TIME__);
    return 0;
}

static SAMPLE_VDEC_ATTR sample_get_vdec_attr_from_stream_info(const struct stream_info *info) {
    SAMPLE_LOG_I("stream info: %dx%d payload %d fps %d", info->video.width, info->video.height, info->video.payload, info->video.fps);

    SAMPLE_VDEC_ATTR vdec_attr;
    memset(&vdec_attr, 0, sizeof(vdec_attr));

    vdec_attr.decoded_mode = VIDEO_DEC_MODE_IPB;
    vdec_attr.output_order = AX_VDEC_OUTPUT_ORDER_DEC;
    vdec_attr.display_mode = AX_VDEC_DISPLAY_MODE_PLAYBACK;

    AX_VDEC_GRP_ATTR_T &grp_attr = vdec_attr.grp_attr;
    grp_attr.enCodecType = info->video.payload;
    grp_attr.enInputMode = AX_VDEC_INPUT_MODE_FRAME;
    grp_attr.u32MaxPicWidth = ALIGN_UP(info->video.width, 16);
    grp_attr.u32MaxPicHeight = ALIGN_UP(info->video.height, 16);
    grp_attr.u32StreamBufSize = grp_attr.u32MaxPicWidth * grp_attr.u32MaxPicHeight * 2;
    grp_attr.bSdkAutoFramePool = AX_TRUE;

    vdec_attr.chn_attr[0].enable = AX_TRUE;
    vdec_attr.chn_attr[1].enable = AX_FALSE;
    vdec_attr.chn_attr[2].enable = AX_FALSE;
    AX_VDEC_CHN_ATTR_T &chn_attr = vdec_attr.chn_attr[0].attr;
    chn_attr.u32PicWidth = info->video.width;
    chn_attr.u32PicHeight = info->video.height;
    chn_attr.u32FrameStride = ALIGN_UP(chn_attr.u32PicWidth, VDEC_STRIDE_ALIGN);
    chn_attr.u32OutputFifoDepth = 3;
    chn_attr.u32FrameBufCnt = 8;
    chn_attr.stCompressInfo.enCompressMode = AX_COMPRESS_MODE_NONE;
    chn_attr.stCompressInfo.u32CompressLevel = 0;
    chn_attr.enOutputMode = AX_VDEC_OUTPUT_ORIGINAL;
    chn_attr.enImgFormat = AX_FORMAT_YUV420_SEMIPLANAR;
    chn_attr.u32FrameBufSize = sample_vdec_calc_blk_size(chn_attr.u32PicWidth, chn_attr.u32PicHeight, info->video.payload,
                                                         &chn_attr.stCompressInfo, chn_attr.enImgFormat);

    return vdec_attr;
}

static AX_S32 sample_vdec_set_attr(SAMPLE_VDEC_ATTR *vdec_attr, int32_t chn_id, int32_t w, int32_t h) {
    AX_VDEC_GRP_ATTR_T &grp_attr = vdec_attr->grp_attr;

    vdec_attr->chn_attr[0].enable = AX_FALSE;
    vdec_attr->chn_attr[chn_id].enable = AX_TRUE;

    AX_VDEC_CHN_ATTR_T &chn_attr = vdec_attr->chn_attr[chn_id].attr;
    chn_attr.u32PicWidth = w;
    chn_attr.u32PicHeight = h;
    chn_attr.u32FrameStride = ALIGN_UP(chn_attr.u32PicWidth, VDEC_STRIDE_ALIGN);
    chn_attr.u32OutputFifoDepth = 3;
    chn_attr.u32FrameBufCnt = 8;
    chn_attr.stCompressInfo.enCompressMode = AX_COMPRESS_MODE_NONE;
    chn_attr.stCompressInfo.u32CompressLevel = 0;
    chn_attr.enOutputMode = AX_VDEC_OUTPUT_SCALE;
    chn_attr.enImgFormat = AX_FORMAT_YUV420_SEMIPLANAR;
    chn_attr.u32FrameBufSize = sample_vdec_calc_blk_size(chn_attr.u32PicWidth, chn_attr.u32PicHeight, grp_attr.enCodecType,
                                                         &chn_attr.stCompressInfo, chn_attr.enImgFormat);
    return 0;
}

void NV12toI420_stride(
    const uint8_t* nv12_ptr,   // NV12原始数据指针
    uint8_t* i420_ptr,         // I420输出数据指针
    int width,                 // 图像有效宽度
    int height,                // 图像有效高度
    int picStride              // 每行实际存储字节数（对齐）
) {
    int y_size = width * height;
    int uv_size = (width / 2) * (height / 2);

    const uint8_t* src_y = nv12_ptr;
    const uint8_t* src_uv = nv12_ptr + picStride * height;
    uint8_t* dst_y = i420_ptr;
    uint8_t* dst_u = i420_ptr + y_size;
    uint8_t* dst_v = i420_ptr + y_size + uv_size;

    // 1. 拷贝Y分量
    for (int y = 0; y < height; y++) {
        memcpy(dst_y + y * width, src_y + y * picStride, width);
    }

    // 2. 拆分UV分量为U、V分量
    for (int j = 0; j < height / 2; j++) {
        const uint8_t* uv_line = src_uv + j * picStride;
        for (int i = 0; i < width / 2; i++) {
            dst_u[j * (width / 2) + i] = uv_line[2 * i];     // U
            dst_v[j * (width / 2) + i] = uv_line[2 * i + 1]; // V
        }
    }
}

static void sample_get_decoded_image_thread(AX_VDEC_GRP grp, int32_t device_id, axcl::event *eof_event, SAMPLE_VDEC_CHN_INFO chn_info,
                                            int32_t dump) {
    SAMPLE_LOG_I("[decoder %2d] decode thread +++", grp);

    /* step01: create thread context */
    axclrtContext context;
    if (axclError ret = axclrtCreateContext(&context, device_id); AXCL_SUCC != ret) {
        return;
    }

    const AX_VDEC_CHN chn = chn_info.u32ChnId;
    AX_S32 ret;

    const size_t size = ALIGN_UP(chn_info.u32PicWidth, VDEC_STRIDE_ALIGN) * chn_info.u32PicHeight * 3 / 2;
    dma_buffer allocator(device_id);

    if (dump) {
        if (size <= 0) {
            axclrtDestroyContext(context);
            SAMPLE_LOG_E("[decoder %2d] decode nv12 frame size = %ld", grp, size);
            return;
        }

        if (!allocator.alloc(size)) {
            SAMPLE_LOG_E("[decoder %2d] alloc cma mem size %ld fail", grp, size);
            axclrtDestroyContext(context);
            return;
        }

        SAMPLE_LOG_I("[decoder %2d] alloc cma mem size %ld", grp, size);
    }

    const struct dma_mem &mem = allocator.get();
    FILE *output_file = NULL;
    if(!output_to_file.empty())
    {
        output_file = fopen(output_to_file.c_str(), "wb");
    }
    
    AX_U64 count = 0;
    AX_VIDEO_FRAME_INFO_T frame;
    memset(&frame, 0, sizeof(frame));
    while (!g_exit) {
        /* step02: get decoded image */
        ret = sample_vdec_get_frame(grp, chn, &frame, -1);
        if (0 != ret) {
            if (AX_ERR_VDEC_UNEXIST == ret) {
                SAMPLE_LOG_I("[decoder %2d] grp is destoried", grp);
                break;
            } else if (AX_ERR_VDEC_STRM_ERROR == ret) {
                SAMPLE_LOG_W("[decoder %2d] stream is undecodeable", grp);
                continue;
            } else if (AX_ERR_VDEC_FLOW_END == ret) {
                SAMPLE_LOG_W("[decoder %2d] flow end", grp);
                eof_event->set();
                break;
            } else {
                if (g_exit) {
                    break;
                }

                SAMPLE_LOG_E("[decoder %2d] get frame fail, ret = 0x%x", grp, ret);
                continue;
            }
        }

        /* TODO: */
        if (0 == ret) {
            ++count;

            if (dump) {
                if (axclError Ret =
                        axclrtMemcpy(reinterpret_cast<void *>(mem.blks[0].phy), (void*)frame.stVFrame.u64PhyAddr[0],
                                     size, AXCL_MEMCPY_DEVICE_TO_HOST_PHY);
                    AXCL_SUCC != Ret) {
                    SAMPLE_LOG_E("axclrt memcpy device phy to host phy error grp:%d, chn:%d", grp, chn);
                }
            }
            // int data_size = chn_info.u32PicWidth * chn_info.u32PicHeight * 3 / 2;
            SAMPLE_LOG_I("[decoder %d] got frame %lld, %dx%d stride %d size %u, pts %lld, phy %llx", grp, frame.stVFrame.u64SeqNum,
                         frame.stVFrame.u32Width, frame.stVFrame.u32Height, frame.stVFrame.u32PicStride[0], frame.stVFrame.u32FrameSize,
                         frame.stVFrame.u64PTS, frame.stVFrame.u64PhyAddr[0]);
            if(output_file)
            {
                std::vector<uint8_t> yuv_buf(frame.stVFrame.u32FrameSize);
                if (axclError Ret =
                    axclrtMemcpy((void*)yuv_buf.data(), reinterpret_cast<void *>(frame.stVFrame.u64PhyAddr[0]),frame.stVFrame.u32FrameSize, AXCL_MEMCPY_DEVICE_TO_HOST);
                AXCL_SUCC != Ret) {
                    SAMPLE_LOG_E("axclrt memcpy device phy to host phy error grp:%d, chn:%d", grp, chn);
                }
                std::vector<uint8_t> i420_buf((frame.stVFrame.u32Height * 3 / 2)*frame.stVFrame.u32Width);
                NV12toI420_stride(yuv_buf.data(), i420_buf.data(), frame.stVFrame.u32Width, frame.stVFrame.u32Height, frame.stVFrame.u32PicStride[0]);


                // for(int y = 0; y < frame.stVFrame.u32Height * 3 / 2; y++)
                // {
                //     fwrite(yuv_buf.data() + y * frame.stVFrame.u32PicStride[0], 1, frame.stVFrame.u32Width, output_file);
                // }


                // for(int y = 0; y < frame.stVFrame.u32Height; y++)
                // {
                //     fwrite(yuv_buf.data() + y * frame.stVFrame.u32PicStride[0], 1, frame.stVFrame.u32Width, output_file);
                // }

                // for(int y = frame.stVFrame.u32Height; y < frame.stVFrame.u32Height * 3 / 2; y++)
                // {
                //     fwrite(yuv_buf.data() + y * frame.stVFrame.u32PicStride[0], 1, frame.stVFrame.u32Width, output_file);
                // }
                fwrite(i420_buf.data(), 1, i420_buf.size(), output_file);
            }

            /* step03: release decoded image */
            sample_vdec_release_frame(grp, chn, &frame);
        }
    }
    if(output_file)
    {
        fclose(output_file);
    }
    SAMPLE_LOG_I("[decoder %2d] total decode %lld frames", grp, count);

    /* step04: destory thread context */
    axclrtDestroyContext(context);
    allocator.free();

    SAMPLE_LOG_I("[decoder %2d] dfecode thread ---", grp);
}

static void on_receive_demux_stream_data(const struct stream_data *data, uint64_t userdata) {
    AX_VDEC_GRP grp = static_cast<AX_VDEC_GRP>(data->cookie);
    AX_VDEC_STREAM_T stream;
    memset(&stream, 0, sizeof(stream));
    stream.u64PTS = data->video.pts;
    stream.u32StreamPackLen = data->video.size;
    stream.pu8Addr = data->video.data;
    stream.bEndOfFrame = AX_TRUE;
    if (0 == data->video.size) {
        stream.bEndOfStream = AX_TRUE;
    }

    AX_S32 ret = sample_vdec_send_stream(grp, &stream, -1);
    if (0 != ret && !g_exit) {
        SAMPLE_LOG_E("[decoder %2d] send stream (id: %ld, size: %u) fail, ret = 0x%x", grp, data->video.seq_num, data->video.size, ret);
    }
}
