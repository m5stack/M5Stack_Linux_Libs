/*
 * @Copyright: iflytek
 * @Autor: zghong
 * @Date: 2020-04-07 12:18:54
 * @Description: 利用FFmpeg，对常见的编码格式文件进行编解码
 *
 * 编解码封装在vcodec类中，接口为encode()和decode()函数。
 * 使用时需要指出<源文件>，<目标文件>及<编解码器名>。其中编解码器名可以通过`ffmpeg -encoders, ffmpeg -decoders`查看。
 */

#ifndef _VCODEC_HPP
#define _VCODEC_HPP

#include <string>
#include <streambuf>
#include <fstream>
#ifdef __cplusplus
extern "C"
{
#endif

#include "libavcodec/avcodec.h"
#include "libavutil/opt.h"
#include "libavutil/imgutils.h"
#include "libswscale/swscale.h"

#ifdef __cplusplus
}
#endif

/**********************************************************************
 * vcodec类定义部分
 *
 * 进行视频编解码类，实例化时需要指出<源文件>，<目标文件>及<编解码器名>
 * 对外的接口如下：
 * 1、encode()，编码
 * 2、decode()，解码
 **********************************************************************
 */
class vcodec
{
public:
    enum
    {
        FILE_READ_CALL,
        FILE_READ_WRITE,
        FILE_READ_WRITE_CALL,
        IOSTREAM_READ_CALL,
        IOSTREAM_READ_WRITE,
        IOSTREAM_READ_WRITE_CALL,
        CUSTOM_READ_CALL,
        CUSTOM_READ_WRITE,
        CUSTOM_READ_WRITE_CALL,
    };
    typedef void (*callback_get_ones_frame_t)(size_t, void *, size_t);
    typedef int (*callback_inface_fun_t)(void *, size_t);
    typedef struct _frame_rgb_data
    {
        char *data;
        size_t len;
        size_t h;
        size_t w;
    } frame_rgb_data_t;
    frame_rgb_data_t rgb;
    vcodec(std::string in_path, std::string _codec_name, callback_get_ones_frame_t callback);
    vcodec(std::string in_path, std::string out_path, std::string _codec_name);
    vcodec(std::string in_path, std::string out_path, std::string _codec_name, callback_get_ones_frame_t callback);
    vcodec(std::iostream *sin, std::string _codec_name, callback_get_ones_frame_t callback);
    vcodec(std::iostream *sin, std::iostream *sout, std::string _codec_name);
    vcodec(std::iostream *sin, std::iostream *sout, std::string _codec_name, callback_get_ones_frame_t callback);
    vcodec(callback_inface_fun_t read, callback_inface_fun_t eof, std::string _codec_name, callback_get_ones_frame_t callback);
    vcodec(callback_inface_fun_t read, callback_inface_fun_t eof, callback_inface_fun_t write, std::string _codec_name);
    vcodec(callback_inface_fun_t read, callback_inface_fun_t eof, callback_inface_fun_t write, std::string _codec_name, callback_get_ones_frame_t callback);

    void set_callback(callback_get_ones_frame_t callback);

    callback_inface_fun_t custom_write, custom_read, custom_eof;

    int yuv2rgb(void *_frame, int mode);
    int yuv2rgb(void *_frame);
    void data_init(callback_inface_fun_t read, callback_inface_fun_t eof, callback_inface_fun_t write, std::string in_path, std::string out_path, std::iostream *sin, std::iostream *sout, std::string _codec_name, callback_get_ones_frame_t callback);
    void clear(void);
    ~vcodec();
    int encode();
    int decode();

private:
    int mode;
    std::fstream fin;
    std::fstream fout;
    int use_file;
    std::iostream *_in;
    std::iostream *_out;
    std::string codec_name;

    AVPicture g_oPicture = {0};
    struct SwsContext *g_pScxt = NULL;

    AVFrame *frame;
    AVPacket *packet;
    AVCodec *codec;
    AVCodecContext *codec_ctx;
    AVCodecParserContext *parser_ctx;
    callback_get_ones_frame_t fun;
    int encode_frame2packet();
    int decode_packet2frame();
    int c_write(void *buf, size_t n);
    int c_read(void *buf, size_t n);
    int c_eof(void *buf, size_t n);
};

/**********************************************************************
 * vcodec类实现部分
 **********************************************************************
 */
void vcodec::data_init(callback_inface_fun_t read, callback_inface_fun_t eof, callback_inface_fun_t write, std::string in_path, std::string out_path, std::iostream *sin, std::iostream *sout, std::string _codec_name, callback_get_ones_frame_t callback)
{
    this->custom_read = read;
    this->custom_eof = eof;
    this->custom_write = write;
    this->rgb.data = NULL;
    this->_in = sin;
    this->_out = sout;
    this->fun = callback;
    this->use_file = 0;
    this->codec_name = _codec_name;
    if (in_path.length() != 0)
    {
        this->fin.open(in_path.c_str(), std::ios::binary | std::ios::in);
        if (!this->fin.is_open())
        {
            fprintf(stderr, "[ERROR] Failed to open <%s>\n", in_path.c_str());
            this->fin.close();
            exit(-1);
        }
        this->_in = &this->fin;
        this->use_file = 1;
    }
    if (out_path.length() != 0)
    {
        this->fout.open(out_path.c_str(), std::ios::binary | std::ios::out | std::ios::trunc);
        if (!this->fout.is_open())
        {
            this->fin.close();
            this->fout.close();
            fprintf(stderr, "[ERROR] Failed to open <%s>\n", out_path.c_str());
            exit(-1);
        }
        this->_out = &this->fout;
        this->use_file = 2;
    }
}
vcodec::vcodec(std::string in_path, std::string _codec_name, callback_get_ones_frame_t callback)
{
    std::string __in_path = in_path;
    std::string __out_path;
    std::iostream *__sin = NULL;
    std::iostream *__sout = NULL;
    std::string __codec_name = _codec_name;
    callback_get_ones_frame_t __callback = callback;
    data_init(NULL, NULL, NULL, __in_path, __out_path, __sin, __sout, __codec_name, __callback);
    this->mode = FILE_READ_CALL;
}

/**
 * @brief: vcodec类构造函数
 * @param in_path 输入文件路径
 * @param out_path 输出文件路径
 * @param codec_name 编解码器名
 */
vcodec::vcodec(std::string in_path, std::string out_path, std::string _codec_name)
{
    std::string __in_path = in_path;
    std::string __out_path = out_path;
    std::iostream *__sin = NULL;
    std::iostream *__sout = NULL;
    std::string __codec_name = _codec_name;
    callback_get_ones_frame_t __callback = NULL;
    data_init(NULL, NULL, NULL, __in_path, __out_path, __sin, __sout, __codec_name, __callback);
    this->mode = FILE_READ_WRITE;
}
vcodec::vcodec(std::string in_path, std::string out_path, std::string _codec_name, callback_get_ones_frame_t callback)
{
    std::string __in_path = in_path;
    std::string __out_path = out_path;
    std::iostream *__sin = NULL;
    std::iostream *__sout = NULL;
    std::string __codec_name = _codec_name;
    callback_get_ones_frame_t __callback = callback;
    data_init(NULL, NULL, NULL, __in_path, __out_path, __sin, __sout, __codec_name, __callback);
    this->mode = FILE_READ_WRITE_CALL;
}
vcodec::vcodec(std::iostream *sin, std::iostream *sout, std::string _codec_name)
{
    std::string __in_path;
    std::string __out_path;
    std::iostream *__sin = sin;
    std::iostream *__sout = sout;
    std::string __codec_name = _codec_name;
    callback_get_ones_frame_t __callback = NULL;
    data_init(NULL, NULL, NULL, __in_path, __out_path, __sin, __sout, __codec_name, __callback);
    this->mode = IOSTREAM_READ_WRITE;
}
vcodec::vcodec(std::iostream *sin, std::string _codec_name, callback_get_ones_frame_t callback)
{
    std::string __in_path;
    std::string __out_path;
    std::iostream *__sin = sin;
    std::iostream *__sout = NULL;
    std::string __codec_name = _codec_name;
    callback_get_ones_frame_t __callback = callback;
    data_init(NULL, NULL, NULL, __in_path, __out_path, __sin, __sout, __codec_name, __callback);
    this->mode = IOSTREAM_READ_CALL;
}
vcodec::vcodec(std::iostream *sin, std::iostream *sout, std::string _codec_name, callback_get_ones_frame_t callback)
{
    std::string __in_path;
    std::string __out_path;
    std::iostream *__sin = sin;
    std::iostream *__sout = sout;
    std::string __codec_name = _codec_name;
    callback_get_ones_frame_t __callback = callback;
    data_init(NULL, NULL, NULL, __in_path, __out_path, __sin, __sout, __codec_name, __callback);
    this->mode = IOSTREAM_READ_WRITE_CALL;
}

vcodec::vcodec(callback_inface_fun_t read, callback_inface_fun_t eof, std::string _codec_name, callback_get_ones_frame_t callback)
{
    std::string __in_path;
    std::string __out_path;
    std::iostream *__sin = NULL;
    std::iostream *__sout = NULL;
    std::string __codec_name = _codec_name;
    callback_get_ones_frame_t __callback = callback;
    data_init(read, eof, NULL, __in_path, __out_path, __sin, __sout, __codec_name, __callback);
    this->mode = CUSTOM_READ_CALL;
}
vcodec::vcodec(callback_inface_fun_t read, callback_inface_fun_t eof, callback_inface_fun_t write, std::string _codec_name)
{
    std::string __in_path;
    std::string __out_path;
    std::iostream *__sin = NULL;
    std::iostream *__sout = NULL;
    std::string __codec_name = _codec_name;
    callback_get_ones_frame_t __callback = NULL;
    data_init(read, eof, write, __in_path, __out_path, __sin, __sout, __codec_name, __callback);
    this->mode = CUSTOM_READ_WRITE;
}
vcodec::vcodec(callback_inface_fun_t read, callback_inface_fun_t eof, callback_inface_fun_t write, std::string _codec_name, callback_get_ones_frame_t callback)
{
    std::string __in_path;
    std::string __out_path;
    std::iostream *__sin = NULL;
    std::iostream *__sout = NULL;
    std::string __codec_name = _codec_name;
    callback_get_ones_frame_t __callback = callback;
    data_init(read, eof, write, __in_path, __out_path, __sin, __sout, __codec_name, __callback);
    this->mode = CUSTOM_READ_WRITE_CALL;
}

void vcodec::set_callback(callback_get_ones_frame_t callback)
{
    this->fun = callback;
}

int vcodec::yuv2rgb(void *_frame, int mode)
{
    AVFrame *pFrame = (AVFrame *)_frame;
    int width = pFrame->width;
    int height = pFrame->height;

    if (g_pScxt == NULL)
    {
        g_pScxt = sws_getContext(width, height, (AVPixelFormat)pFrame->format,
                                 width, height, (AVPixelFormat)mode,
                                 NULL, NULL, NULL, NULL);
        // AVFrame需要填充，还是这个方便。
        avpicture_alloc(&g_oPicture, (AVPixelFormat)mode, width, height);
    }
    int num_bytes = av_image_get_buffer_size((AVPixelFormat)mode, width, height, 32);
    int retsws = sws_scale(g_pScxt, pFrame->data, pFrame->linesize, 0, height,
                           g_oPicture.data, g_oPicture.linesize);
    // //注意，RGB数据是这样复制的
    if (this->rgb.data == NULL)
    {
        this->rgb.data = (char *)malloc(num_bytes);
    }
    if (rgb.len != num_bytes)
    {
        free(this->rgb.data);
        this->rgb.data = (char *)malloc(num_bytes);
    }
    memcpy(this->rgb.data, g_oPicture.data[0], num_bytes);
    rgb.w = width;
    rgb.h = height;
    rgb.len = num_bytes;
    return 0;
}

int vcodec::yuv2rgb(void *_frame)
{
    return yuv2rgb(_frame, AV_PIX_FMT_RGB24);
}

void vcodec::clear(void)
{
    switch (this->use_file)
    {
    case 2:
        this->fout.close();
    case 1:
        this->fin.close();
        break;
    default:
        break;
    }
    if (this->g_pScxt != NULL)
    {
        avpicture_free(&this->g_oPicture);
        this->g_oPicture = {0};
        sws_freeContext(this->g_pScxt);
        this->g_pScxt = NULL;
        free(this->rgb.data);
        this->rgb.data = NULL;
    }
}
/**
 * @brief: vcodec类析构函数
 */
vcodec::~vcodec()
{
    clear();
}

/**
 * @brief: 使用给定的编解码器对输入文件编码，将结果写入输出文件
 * @return: 0 for ok, -1 for error
 */
int vcodec::encode()
{
    // allocation
    this->frame = av_frame_alloc();
    if (!this->frame)
    {
        fprintf(stderr, "[ERROR] Failed to allocate frame\n");
        return -1;
    }
    this->packet = av_packet_alloc();
    if (!this->packet)
    {
        fprintf(stderr, "[ERROR] Failed to allocate packet\n");
        return -1;
    }

    this->codec = avcodec_find_encoder_by_name(this->codec_name.c_str());
    if (!this->codec)
    {
        fprintf(stderr, "[ERROR] Failed to find codec\n");
        return -1;
    }
    this->codec_ctx = avcodec_alloc_context3(this->codec);
    if (!this->codec_ctx)
    {
        fprintf(stderr, "[ERROR] Faild to allocate codec context\n");
        return -1;
    }

    // encoder settings
    this->codec_ctx->codec_id = this->codec->id;
    this->codec_ctx->codec_type = AVMEDIA_TYPE_VIDEO;
    this->codec_ctx->pix_fmt = AV_PIX_FMT_YUV420P;
    this->codec_ctx->width = 640;
    this->codec_ctx->height = 480;
    this->codec_ctx->time_base = (AVRational){1, 30}; // fps
    // vbr设置
    // int64_t br = 468000;
    // this->codec_ctx->bit_rate = br;
    // this->codec_ctx->rc_min_rate = br;
    // this->codec_ctx->rc_max_rate = br;
    // this->codec_ctx->bit_rate_tolerance = br;
    // this->codec_ctx->rc_buffer_size = br;
    // this->codec_ctx->rc_initial_buffer_occupancy = this->codec_ctx->rc_buffer_size * 3 / 4;
    this->codec_ctx->bit_rate = 468000;
    this->codec_ctx->gop_size = 250;
    this->codec_ctx->max_b_frames = 0; // no b-frame
    if (this->codec_ctx->codec_id == AV_CODEC_ID_H264)
    {
        av_opt_set(this->codec_ctx->priv_data, "preset", "slow", 0);
        av_opt_set(this->codec_ctx->priv_data, "tune", "zerolatency", 0);
    }
    else if (this->codec_ctx->codec_id == AV_CODEC_ID_H265)
    {
        av_opt_set(this->codec_ctx->priv_data, "preset", "ultrafast", 0);
        av_opt_set(this->codec_ctx->priv_data, "tune", "zero-latency", 0);
    }

    this->frame->format = this->codec_ctx->pix_fmt;
    this->frame->width = this->codec_ctx->width;
    this->frame->height = this->codec_ctx->height;

    // open the codec
    if (avcodec_open2(this->codec_ctx, this->codec, NULL) < 0)
    {
        fprintf(stderr, "[ERROR] Failed to open codec\n");
        return -1;
    }
    else
    {
        fprintf(stdout, "[INFO] Open codec successfully\n");
    }

    // encode 1 second of video
    int frame_size = av_image_get_buffer_size(this->codec_ctx->pix_fmt, this->codec_ctx->width, this->codec_ctx->height, 1);
    uint8_t *frame_buf = (uint8_t *)av_malloc(frame_size);
    int y_size = this->codec_ctx->width * this->codec_ctx->height; // size of Y
    av_image_fill_arrays(this->frame->data, this->frame->linesize, frame_buf, this->codec_ctx->pix_fmt, this->codec_ctx->width, this->codec_ctx->height, 1);
    int i = 0;
    while (!this->c_eof(NULL, 0))
    {
        int num = this->c_read(frame_buf, (y_size * 3 / 2));
        if (num < 0)
        {
            break;
        }
        else if (num == 0)
        {
            continue;
        }

        // read yuv data from source file into AVFrame
        this->frame->data[0] = frame_buf;                  // Y
        this->frame->data[1] = frame_buf + y_size;         // U
        this->frame->data[2] = frame_buf + y_size * 5 / 4; // V
        this->frame->pts = i++;

        // encode frame into packet
        if (this->encode_frame2packet() != 0)
        {
            return -1;
        }
    }

    // flush the encoder
    this->frame = NULL;
    this->encode_frame2packet();

    av_frame_free(&this->frame);
    av_packet_free(&this->packet);
    avcodec_free_context(&this->codec_ctx);

    fprintf(stdout, "[SUCCESS] Encode file successfully\n");
    return 0;
}

/**
 * @brief: 使用给定的编解码器对输入文件解码，将结果写入输出文件
 * @return: 0 for ok, -1 for error
 */
int vcodec::decode()
{
    // allocation
    this->frame = av_frame_alloc();
    if (!this->frame)
    {
        fprintf(stderr, "[ERROR] Failed to allocate frame\n");
        return -1;
    }
    this->packet = av_packet_alloc();
    if (!this->packet)
    {
        fprintf(stderr, "[ERROR] Failed to allocate packet\n");
        return -1;
    }
    this->codec = avcodec_find_decoder_by_name(this->codec_name.c_str());
    if (!this->codec)
    {
        fprintf(stderr, "[ERROR] Failed to find codec\n");
        return -1;
    }
    this->codec_ctx = avcodec_alloc_context3(this->codec);
    if (!this->codec_ctx)
    {
        fprintf(stderr, "[ERROR] Faild to allocate codec context\n");
        return -1;
    }
    this->parser_ctx = av_parser_init(this->codec->id);
    if (!this->parser_ctx)
    {
        fprintf(stderr, "[ERROR] Failed to find parser of codec\n");
        return -1;
    }

    // open the codec
    if (avcodec_open2(this->codec_ctx, this->codec, NULL) < 0)
    {
        fprintf(stderr, "[ERROR] Failed to open codec\n");
        return -1;
    }
    else
    {
        fprintf(stdout, "[INFO] Open codec successfully\n");
    }

    // start to decode
    int input_buf_size = 4096;
    uint8_t input_buf[input_buf_size + AV_INPUT_BUFFER_PADDING_SIZE] = {0};
    while (!this->c_eof(NULL, 0))
    {
        // read source data
        // this->_in->read((char *)input_buf, input_buf_size);
        // int read_size = this->_in->gcount();
        int read_size = this->c_read(input_buf, input_buf_size);
        if (read_size < 0)
        {
            break;
        }
        else if (read_size == 0)
        {
            continue;
        }
        uint8_t *input_buf_pos = input_buf;
        while (read_size > 0)
        {
            // parse data into packet
            int parsed_size = av_parser_parse2(this->parser_ctx, this->codec_ctx,
                                               &this->packet->data, &this->packet->size, input_buf_pos, read_size,
                                               AV_NOPTS_VALUE, AV_NOPTS_VALUE, 0);
            if (parsed_size < 0)
            {
                fprintf(stderr, "Error while parsing\n");
                return -1;
            }
            // update position of buffer and size of remained data
            input_buf_pos += parsed_size;
            read_size -= parsed_size;

            if (this->packet->size)
            {
                // decode packet into frame
                if (this->decode_packet2frame() != 0)
                {
                    fprintf(stderr, "Error while packet2frame\n");
                    return -1;
                }
            }
        }
    }

    fprintf(stdout, "[SUCCESS] Decode file successfully\n");
    return 0;
}

/**
 * @brief: 将数据帧编码成数据包
 * @return: 0 for ok, -1 for error
 */
int vcodec::encode_frame2packet()
{
    int ret = avcodec_send_frame(this->codec_ctx, this->frame);
    if (ret < 0)
    {
        fprintf(stderr, "[ERROR] Failed to send a frame for encoding\n");
        return -1;
    }
    while (ret >= 0)
    {
        ret = avcodec_receive_packet(this->codec_ctx, this->packet);
        if (ret == AVERROR(EAGAIN) || ret == AVERROR_EOF)
        {
            return 0;
        }
        else if (ret < 0)
        {
            fprintf(stderr, "[ERROR] Error during encoding, code <%d>\n", ret);
            return -1;
        }
        static size_t ecount = 0;
        if (this->fun)
            this->fun(ecount++, this->packet->data, this->packet->size);
        if (this->_out)
            this->c_write((this->packet->data), (this->packet->size));
        // this->_out->write((char *)this->packet->data, this->packet->size);
        fprintf(stdout, "[INFO] Saving packet %3" PRId64 " (size=%5d)\n", this->packet->pts, this->packet->size);
        av_packet_unref(this->packet);
    }
    return 0;
}

/**
 * @brief: 将数据包解码成数据帧
 * @return: 0 for ok, -1 for error
 */
int vcodec::decode_packet2frame()
{
    int ret = avcodec_send_packet(this->codec_ctx, this->packet);
    if (ret < 0)
    {
        fprintf(stderr, "[ERROR] Failed to send a packet for decoding\n");
        return -1;
    }
    while (ret >= 0)
    {
        ret = avcodec_receive_frame(this->codec_ctx, frame);
        if (ret == AVERROR(EAGAIN) || ret == AVERROR_EOF)
        {
            return 0;
        }
        else if (ret < 0)
        {
            fprintf(stderr, "[ERROR] Error during decoding, code <%d>\n", ret);
            return -1;
        }
        static size_t dcount = 0;
        if (this->fun)
            this->fun(dcount++, frame, 0);
        // save the frame into file
        // Y, U, V
        if (this->_out)
        {

            for (int i = 0; i < frame->height; i++)
            {
                // this->_out->write((char *)(frame->data[0] + frame->linesize[0] * i), frame->width);
                this->c_write((frame->data[0] + frame->linesize[0] * i), (frame->width));
            }
            for (int i = 0; i < frame->height / 2; i++)
            {
                // this->_out->write((char *)(frame->data[1] + frame->linesize[1] * i), frame->width / 2);
                this->c_write((frame->data[1] + frame->linesize[1] * i), (frame->width / 2));
            }
            for (int i = 0; i < frame->height / 2; i++)
            {
                // this->_out->write((char *)(frame->data[2] + frame->linesize[2] * i), frame->width / 2);
                this->c_write((frame->data[2] + frame->linesize[2] * i), (frame->width / 2));
            }
        }

        fprintf(stdout, "[INFO] Saving frame %3d\n", this->codec_ctx->frame_number);
        av_frame_unref(this->frame);
    }
    return 0;
}
int vcodec::c_write(void *buf, size_t n)
{
    switch (mode)
    {
    case FILE_READ_WRITE:
    case FILE_READ_WRITE_CALL:
    case IOSTREAM_READ_WRITE:
    case IOSTREAM_READ_WRITE_CALL:
    {
        this->_out->write((char *)buf, n);
    }
    break;
    case CUSTOM_READ_WRITE:
    case CUSTOM_READ_WRITE_CALL:
    {
        this->custom_write(buf, n);
    }
    break;
    default:
        break;
    }
    return 0;
}
int vcodec::c_read(void *buf, size_t n)
{
    switch (mode)
    {
    case FILE_READ_CALL:
    case FILE_READ_WRITE:
    case FILE_READ_WRITE_CALL:
    case IOSTREAM_READ_CALL:
    case IOSTREAM_READ_WRITE:
    case IOSTREAM_READ_WRITE_CALL:
    {
        this->_in->read((char *)buf, n);
        int mk = this->_in->gcount();
        return mk;
    }
    break;
    case CUSTOM_READ_CALL:
    case CUSTOM_READ_WRITE:
    case CUSTOM_READ_WRITE_CALL:
    {
        return this->custom_read(buf, n);
    }
    break;
    default:
        break;
    }
}
int vcodec::c_eof(void *buf, size_t n)
{
    switch (mode)
    {
    case FILE_READ_CALL:
    case FILE_READ_WRITE:
    case FILE_READ_WRITE_CALL:
    case IOSTREAM_READ_CALL:
    case IOSTREAM_READ_WRITE:
    case IOSTREAM_READ_WRITE_CALL:
    {
        return (int)this->_in->eof();
    }
    break;
    case CUSTOM_READ_CALL:
    case CUSTOM_READ_WRITE:
    case CUSTOM_READ_WRITE_CALL:
    {
        return this->custom_eof(NULL, 0);
    }
    break;
    default:
        break;
    }
}

#endif