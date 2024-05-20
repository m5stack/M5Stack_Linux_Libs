#ifndef _NEON_UTILITIES_H_
#define _NEON_UTILITIES_H_
#include <stdio.h>
#include <stdint.h>
#include <arm_neon.h>
#include <stdio.h>
#include <stdint.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>

void rgb888_to_rgb565(uint8_t *in, uint8_t *out, int h, int v)
{
    uint16_t *d        = (uint16_t *)out;
    const uint8_t *s   = in;
    const uint8_t *end = s + h * v * 3;

    while (s < end) {
        const int r = *s++;
        const int g = *s++;
        const int b = *s++;
        *d++        = (b >> 3) | ((g & 0xFC) << 3) | ((r & 0xF8) << 8);
    }
}

void rgb888_to_rgb565_neon(uint8_t *in, uint8_t *out, int h, int v)
{
    uint8_t *src = in;
    uint16_t *dst = (uint16_t *)out;
    int count = h * v;
    if(count < 8)
    {
        rgb888_to_rgb565(in, out, h, v);
    }
    else
    {
        int count_f = count % 8 ;
        int count_c = count - count_f ;
        while (count_c >= 8) {
            uint8x8x3_t vsrc;
            uint16x8_t vdst;

            vsrc = vld3_u8(src);

            vdst = vshll_n_u8(vsrc.val[0], 8);
            vdst = vsriq_n_u16(vdst, vshll_n_u8(vsrc.val[1], 8), 5);
            vdst = vsriq_n_u16(vdst, vshll_n_u8(vsrc.val[2], 8), 11);

            vst1q_u16(dst, vdst);

            dst += 8;
            src += 8*3;
            count_c -= 8;
        }
        for (int i = count - count_f; i < count; i++)
        {
            const int r = *src++;
            const int g = *src++;
            const int b = *src++;
            *dst++        = (b >> 3) | ((g & 0xFC) << 3) | ((r & 0xF8) << 8);
        }
    }

}
















#endif /* _NEON_UTILITIES_H_ */
