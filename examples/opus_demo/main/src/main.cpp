#include <stdio.h>
#include <stdlib.h>
#include <opus/opus.h>

int main() {
    int err;

    // 创建编码器和解码器
    OpusEncoder *encoder = opus_encoder_create(48000, 1, OPUS_APPLICATION_VOIP, &err);
    OpusDecoder *decoder = opus_decoder_create(48000, 1, &err);

    if (err < 0) {
        fprintf(stderr, "Failed to create an encoder or decoder: %s\n", opus_strerror(err));
        return EXIT_FAILURE;
    }

    // 假设我们有一些输入音频数据要编码
    // 这里只是一个示例，实际上你需要有一些 PCM 数据
    opus_int16 in[960]; // 20ms 的音频帧 @ 48kHz

    // 编码输出缓冲区
    unsigned char out[4000];

    // 编码音频帧
    int nbytes = opus_encode(encoder, in, 960, out, sizeof(out));
    if (nbytes < 0) {
        fprintf(stderr, "Encode failed: %s\n", opus_strerror(nbytes));
        return EXIT_FAILURE;
    }

    // 解码音频帧
    int frame_size = opus_decode(decoder, out, nbytes, in, 960, 0);
    if (frame_size < 0) {
        fprintf(stderr, "Decode failed: %s\n", opus_strerror(frame_size));
        return EXIT_FAILURE;
    }

    // 清理
    opus_encoder_destroy(encoder);
    opus_decoder_destroy(decoder);

    printf("Encoding and decoding were successful!\n");
    return EXIT_SUCCESS;
}
