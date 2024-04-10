/*
 * @Copyright: iflytek
 * @Autor: zghong
 * @Date: 2020-04-02 16:52:55
 * @Description: 测试vcodec类
 * 
 * libx264和libx265第三方库集成在`./include, ./lib`中。
 * 已经测试的编解码算法有：h264, h265
 */

#include <stdio.h>
#include <string>
#include "vcodec.hpp"
// #include <opencv2/opencv.hpp>
#include <iostream>

using namespace std;
vcodec *adecoder;
void get_pictor(size_t count, void *data, size_t len)
{
    adecoder->yuv2rgb(data);
    printf("get c:%d h:%d   w:%d    len:%d\r\n",count,adecoder->rgb.h, adecoder->rgb.w, adecoder->rgb.len);
    // cv::Mat src(adecoder->rgb.h,adecoder->rgb.w, CV_8UC3, (void*)(adecoder->rgb.data));
    // cv::Mat dst;
    // cv::cvtColor(src, dst, cv::COLOR_RGB2BGR);
    // cv::imshow("123",dst);
    // cv::waitKey(3);
}




int main()
{
    // decoder_h264
    vcodec decoder_h264 = vcodec("../bin/video/input.h264", "./decoder_h264.yuv", "h264",get_pictor);
    adecoder = &decoder_h264;
    decoder_h264.decode();
    // encoder_h264
    vcodec encoder_h264 = vcodec("./decoder_h264.yuv", "./encoder_h264.h264", "libx264");
    encoder_h264.encode();
    // encoder_h265
    vcodec encoder_h265 = vcodec("./decoder_h264.yuv", "./encoder_h265.h265", "libx265");
    encoder_h265.encode();
    // decoder_h265
    vcodec decoder_h265 = vcodec("./encoder_h265.h265", "./decoder_h265.yuv", "hevc", get_pictor);
    adecoder = &decoder_h265;
    decoder_h265.decode();
    return 0;
}