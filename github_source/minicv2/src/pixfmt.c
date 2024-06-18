#include "imlib.h"
#if defined(__ARM_NEON) || defined(__ARM_NEON__)
#include <arm_neon.h>
#endif

void imlib_pixfmt_binary_to_binary(image_t *dst, image_t *src, rectangle_t *roi) {
    for (int dst_y_index = 0, roi_y_index = roi->y; dst_y_index < dst->h; ++dst_y_index, ++roi_y_index) {
        uint32_t *src_row_ptr = IMAGE_COMPUTE_BINARY_PIXEL_ROW_PTR(src, roi_y_index);
        uint32_t *dst_row_ptr = IMAGE_COMPUTE_BINARY_PIXEL_ROW_PTR(dst, dst_y_index);
        for (int dst_x_index = 0, roi_x_index = roi->x; dst_x_index < dst->w; ++dst_x_index, ++roi_x_index) {
            int pixel = IMAGE_GET_BINARY_PIXEL_FAST(src_row_ptr, roi_x_index);
            IMAGE_PUT_BINARY_PIXEL_FAST(dst_row_ptr, dst_x_index, pixel);
        }
    }
}
void imlib_pixfmt_binary_to_grayscale(image_t *dst, image_t *src, rectangle_t *roi) {
    for (int dst_y_index = 0, roi_y_index = roi->y; dst_y_index < dst->h; ++dst_y_index, ++roi_y_index) {
        uint32_t *src_row_ptr = IMAGE_COMPUTE_BINARY_PIXEL_ROW_PTR(src, roi_y_index);
        uint8_t *dst_row_ptr  = IMAGE_COMPUTE_GRAYSCALE_PIXEL_ROW_PTR(dst, dst_y_index);
        for (int dst_x_index = 0, roi_x_index = roi->x; dst_x_index < dst->w; ++dst_x_index, ++roi_x_index) {
            int pixel = COLOR_BINARY_TO_GRAYSCALE(IMAGE_GET_BINARY_PIXEL_FAST(src_row_ptr, roi_x_index));
            IMAGE_PUT_GRAYSCALE_PIXEL_FAST(dst_row_ptr, dst_x_index, pixel);
        }
    }
}
void imlib_pixfmt_binary_to_rgb565(image_t *dst, image_t *src, rectangle_t *roi) {
    for (int dst_y_index = 0, roi_y_index = roi->y; dst_y_index < dst->h; ++dst_y_index, ++roi_y_index) {
        uint32_t *src_row_ptr = IMAGE_COMPUTE_BINARY_PIXEL_ROW_PTR(src, roi_y_index);
        uint16_t *dst_row_ptr = IMAGE_COMPUTE_RGB565_PIXEL_ROW_PTR(dst, dst_y_index);
        for (int dst_x_index = 0, roi_x_index = roi->x; dst_x_index < dst->w; ++dst_x_index, ++roi_x_index) {
            int pixel = COLOR_BINARY_TO_RGB565(IMAGE_GET_BINARY_PIXEL_FAST(src_row_ptr, roi_x_index));
            IMAGE_PUT_RGB565_PIXEL_FAST(dst_row_ptr, dst_x_index, pixel);
        }
    }
}
void imlib_pixfmt_binary_to_rgb888(image_t *dst, image_t *src, rectangle_t *roi) {
    for (int dst_y_index = 0, roi_y_index = roi->y; dst_y_index < dst->h; ++dst_y_index, ++roi_y_index) {
        uint32_t *src_row_ptr  = IMAGE_COMPUTE_BINARY_PIXEL_ROW_PTR(src, roi_y_index);
        pixel24_t *dst_row_ptr = IMAGE_COMPUTE_RGB888_PIXEL_ROW_PTR(dst, dst_y_index);
        for (int dst_x_index = 0, roi_x_index = roi->x; dst_x_index < dst->w; ++dst_x_index, ++roi_x_index) {
            int pixel = COLOR_BINARY_TO_RGB888(IMAGE_GET_BINARY_PIXEL_FAST(src_row_ptr, roi_x_index));
            IMAGE_PUT_RGB888_PIXEL_FAST(dst_row_ptr, dst_x_index, pixel);
        }
    }
}
void imlib_pixfmt_grayscale_to_binary(image_t *dst, image_t *src, rectangle_t *roi) {
    for (int dst_y_index = 0, roi_y_index = roi->y; dst_y_index < dst->h; ++dst_y_index, ++roi_y_index) {
        uint8_t *src_row_ptr  = IMAGE_COMPUTE_GRAYSCALE_PIXEL_ROW_PTR(src, roi_y_index);
        uint32_t *dst_row_ptr = IMAGE_COMPUTE_BINARY_PIXEL_ROW_PTR(dst, dst_y_index);
        for (int dst_x_index = 0, roi_x_index = roi->x; dst_x_index < dst->w; ++dst_x_index, ++roi_x_index) {
            int pixel = COLOR_GRAYSCALE_TO_BINARY(IMAGE_GET_GRAYSCALE_PIXEL_FAST(src_row_ptr, roi_x_index));
            IMAGE_PUT_BINARY_PIXEL_FAST(dst_row_ptr, dst_x_index, pixel);
        }
    }
}
void imlib_pixfmt_grayscale_to_grayscale(image_t *dst, image_t *src, rectangle_t *roi) {
    for (int dst_y_index = 0, roi_y_index = roi->y; dst_y_index < dst->h; ++dst_y_index, ++roi_y_index) {
        uint8_t *src_row_ptr = IMAGE_COMPUTE_GRAYSCALE_PIXEL_ROW_PTR(src, roi_y_index);
        uint8_t *dst_row_ptr = IMAGE_COMPUTE_GRAYSCALE_PIXEL_ROW_PTR(dst, dst_y_index);
        for (int dst_x_index = 0, roi_x_index = roi->x; dst_x_index < dst->w; ++dst_x_index, ++roi_x_index) {
            int pixel = IMAGE_GET_GRAYSCALE_PIXEL_FAST(src_row_ptr, roi_x_index);
            IMAGE_PUT_GRAYSCALE_PIXEL_FAST(dst_row_ptr, dst_x_index, pixel);
        }
    }
}
void imlib_pixfmt_grayscale_to_rgb565(image_t *dst, image_t *src, rectangle_t *roi) {
    for (int dst_y_index = 0, roi_y_index = roi->y; dst_y_index < dst->h; ++dst_y_index, ++roi_y_index) {
        uint8_t *src_row_ptr  = IMAGE_COMPUTE_GRAYSCALE_PIXEL_ROW_PTR(src, roi_y_index);
        uint16_t *dst_row_ptr = IMAGE_COMPUTE_RGB565_PIXEL_ROW_PTR(dst, dst_y_index);
        for (int dst_x_index = 0, roi_x_index = roi->x; dst_x_index < dst->w; ++dst_x_index, ++roi_x_index) {
            int pixel = COLOR_GRAYSCALE_TO_RGB565(IMAGE_GET_GRAYSCALE_PIXEL_FAST(src_row_ptr, roi_x_index));
            IMAGE_PUT_RGB565_PIXEL_FAST(dst_row_ptr, dst_x_index, pixel);
        }
    }
}
void imlib_pixfmt_grayscale_to_rgb888(image_t *dst, image_t *src, rectangle_t *roi) {
    for (int dst_y_index = 0, roi_y_index = roi->y; dst_y_index < dst->h; ++dst_y_index, ++roi_y_index) {
        uint8_t *src_row_ptr   = IMAGE_COMPUTE_GRAYSCALE_PIXEL_ROW_PTR(src, roi_y_index);
        pixel24_t *dst_row_ptr = IMAGE_COMPUTE_RGB888_PIXEL_ROW_PTR(dst, dst_y_index);
        for (int dst_x_index = 0, roi_x_index = roi->x; dst_x_index < dst->w; ++dst_x_index, ++roi_x_index) {
            int pixel = COLOR_GRAYSCALE_TO_RGB888(IMAGE_GET_GRAYSCALE_PIXEL_FAST(src_row_ptr, roi_x_index));
            IMAGE_PUT_RGB888_PIXEL_FAST(dst_row_ptr, dst_x_index, pixel);
        }
    }
}
void imlib_pixfmt_rgb565_to_binary(image_t *dst, image_t *src, rectangle_t *roi) {
    for (int dst_y_index = 0, roi_y_index = roi->y; dst_y_index < dst->h; ++dst_y_index, ++roi_y_index) {
        uint16_t *src_row_ptr = IMAGE_COMPUTE_RGB565_PIXEL_ROW_PTR(src, roi_y_index);
        uint32_t *dst_row_ptr = IMAGE_COMPUTE_BINARY_PIXEL_ROW_PTR(dst, dst_y_index);
        for (int dst_x_index = 0, roi_x_index = roi->x; dst_x_index < dst->w; ++dst_x_index, ++roi_x_index) {
            int pixel = COLOR_RGB565_TO_BINARY(IMAGE_GET_RGB565_PIXEL_FAST(src_row_ptr, roi_x_index));
            IMAGE_PUT_BINARY_PIXEL_FAST(dst_row_ptr, dst_x_index, pixel);
        }
    }
}
void imlib_pixfmt_rgb565_to_grayscale(image_t *dst, image_t *src, rectangle_t *roi) {
    for (int dst_y_index = 0, roi_y_index = roi->y; dst_y_index < dst->h; ++dst_y_index, ++roi_y_index) {
        uint16_t *src_row_ptr = IMAGE_COMPUTE_RGB565_PIXEL_ROW_PTR(src, roi_y_index);
        uint8_t *dst_row_ptr  = IMAGE_COMPUTE_GRAYSCALE_PIXEL_ROW_PTR(dst, dst_y_index);
        for (int dst_x_index = 0, roi_x_index = roi->x; dst_x_index < dst->w; ++dst_x_index, ++roi_x_index) {
            int pixel = COLOR_RGB565_TO_GRAYSCALE(IMAGE_GET_RGB565_PIXEL_FAST(src_row_ptr, roi_x_index));
            IMAGE_PUT_GRAYSCALE_PIXEL_FAST(dst_row_ptr, dst_x_index, pixel);
        }
    }
}
void imlib_pixfmt_rgb565_to_rgb565(image_t *dst, image_t *src, rectangle_t *roi) {
    for (int dst_y_index = 0, roi_y_index = roi->y; dst_y_index < dst->h; ++dst_y_index, ++roi_y_index) {
        uint16_t *src_row_ptr = IMAGE_COMPUTE_RGB565_PIXEL_ROW_PTR(src, roi_y_index);
        uint16_t *dst_row_ptr = IMAGE_COMPUTE_RGB565_PIXEL_ROW_PTR(dst, dst_y_index);
        for (int dst_x_index = 0, roi_x_index = roi->x; dst_x_index < dst->w; ++dst_x_index, ++roi_x_index) {
            int pixel = IMAGE_GET_RGB565_PIXEL_FAST(src_row_ptr, roi_x_index);
            IMAGE_PUT_RGB565_PIXEL_FAST(dst_row_ptr, dst_x_index, pixel);
        }
    }
}
void imlib_pixfmt_rgb565_to_rgb888(image_t *dst, image_t *src, rectangle_t *roi) {
    for (int dst_y_index = 0, roi_y_index = roi->y; dst_y_index < dst->h; ++dst_y_index, ++roi_y_index) {
        uint16_t *src_row_ptr  = IMAGE_COMPUTE_RGB565_PIXEL_ROW_PTR(src, roi_y_index);
        pixel24_t *dst_row_ptr = IMAGE_COMPUTE_RGB888_PIXEL_ROW_PTR(dst, dst_y_index);
        for (int dst_x_index = 0, roi_x_index = roi->x; dst_x_index < dst->w; ++dst_x_index, ++roi_x_index) {
            int pixel           = IMAGE_GET_RGB565_PIXEL_FAST(src_row_ptr, roi_x_index);
            pixel24_t pixel_out = {.red   = COLOR_RGB565_TO_R8(pixel),
                                   .green = COLOR_RGB565_TO_G8(pixel),
                                   .blue  = COLOR_RGB565_TO_B8(pixel)};
            IMAGE_PUT_RGB888_PIXEL_FAST_(dst_row_ptr, dst_x_index, pixel_out);
        }
    }
}
void imlib_pixfmt_rgb888_to_binary(image_t *dst, image_t *src, rectangle_t *roi) {
    for (int dst_y_index = 0, roi_y_index = roi->y; dst_y_index < dst->h; ++dst_y_index, ++roi_y_index) {
        pixel24_t *src_row_ptr = IMAGE_COMPUTE_RGB888_PIXEL_ROW_PTR(src, roi_y_index);
        uint32_t *dst_row_ptr  = IMAGE_COMPUTE_BINARY_PIXEL_ROW_PTR(dst, dst_y_index);
        for (int dst_x_index = 0, roi_x_index = roi->x; dst_x_index < dst->w; ++dst_x_index, ++roi_x_index) {
            int pixel = COLOR_RGB888_TO_BINARY(IMAGE_GET_RGB888_PIXEL_FAST(src_row_ptr, roi_x_index));
            IMAGE_PUT_BINARY_PIXEL_FAST(dst_row_ptr, dst_x_index, pixel);
        }
    }
}
void imlib_pixfmt_rgb888_to_grayscale(image_t *dst, image_t *src, rectangle_t *roi) {
    for (int dst_y_index = 0, roi_y_index = roi->y; dst_y_index < dst->h; ++dst_y_index, ++roi_y_index) {
        pixel24_t *src_row_ptr = IMAGE_COMPUTE_RGB888_PIXEL_ROW_PTR(src, roi_y_index);
        uint8_t *dst_row_ptr   = IMAGE_COMPUTE_GRAYSCALE_PIXEL_ROW_PTR(dst, dst_y_index);
        for (int dst_x_index = 0, roi_x_index = roi->x; dst_x_index < dst->w; ++dst_x_index, ++roi_x_index) {
            pixel24_t s_pixel = IMAGE_GET_RGB888_PIXEL_FAST_(src_row_ptr, roi_x_index);
            int pixel         = COLOR_RGB888_TO_Y_(s_pixel.red, s_pixel.green, s_pixel.blue);
            IMAGE_PUT_GRAYSCALE_PIXEL_FAST(dst_row_ptr, dst_x_index, pixel);
        }
    }
}
void imlib_pixfmt_rgb888_to_rgb565(image_t *dst, image_t *src, rectangle_t *roi) {
    for (int dst_y_index = 0, roi_y_index = roi->y; dst_y_index < dst->h; ++dst_y_index, ++roi_y_index) {
        pixel24_t *src_row_ptr = IMAGE_COMPUTE_RGB888_PIXEL_ROW_PTR(src, roi_y_index);
        uint16_t *dst_row_ptr  = IMAGE_COMPUTE_RGB565_PIXEL_ROW_PTR(dst, dst_y_index);
        for (int dst_x_index = 0, roi_x_index = roi->x; dst_x_index < dst->w; ++dst_x_index, ++roi_x_index) {
            pixel24_t src_pixel = IMAGE_GET_RGB888_PIXEL_FAST_(src_row_ptr, roi_x_index);
            int pixel           = COLOR_R8_G8_B8_TO_RGB565(src_pixel.red, src_pixel.green, src_pixel.blue);
            IMAGE_PUT_RGB565_PIXEL_FAST(dst_row_ptr, dst_x_index, pixel);
        }
    }
}
void imlib_pixfmt_rgb888_to_rgb888(image_t *dst, image_t *src, rectangle_t *roi) {
    for (int dst_y_index = 0, roi_y_index = roi->y; dst_y_index < dst->h; ++dst_y_index, ++roi_y_index) {
        pixel24_t *src_row_ptr = IMAGE_COMPUTE_RGB888_PIXEL_ROW_PTR(src, roi_y_index);
        pixel24_t *dst_row_ptr = IMAGE_COMPUTE_RGB888_PIXEL_ROW_PTR(dst, dst_y_index);
        for (int dst_x_index = 0, roi_x_index = roi->x; dst_x_index < dst->w; ++dst_x_index, ++roi_x_index) {
            pixel24_t pixel = IMAGE_GET_RGB888_PIXEL_FAST_(src_row_ptr, roi_x_index);
            IMAGE_PUT_RGB888_PIXEL_FAST_(dst_row_ptr, dst_x_index, pixel);
        }
    }
}
void imlib_pixfmt_yuv422_to_binary(image_t *dst, image_t *src, rectangle_t *roi) {}
void imlib_pixfmt_yuv422_to_grayscale(image_t *dst, image_t *src, rectangle_t *roi) {}

void imlib_pixfmt_yuv422_to_rgb565(image_t *dst, image_t *src, rectangle_t *roi) {
    if (roi->x == 0 && roi->y == 0 && roi->w == dst->w && roi->h == dst->h && dst->w == src->w && dst->h == src->h) {
        uint8_t *pYuv = src->data;
        uint8_t *pRgb = dst->data;
        uint32_t Pixel32;
        uint16_t *pPixel = (uint16_t *)&Pixel32;
        // Resolution describes the number of pixels, and each YUV pixel occupies 4
        // characters, so here the total number of characters needs to be multiplied
        // by 2.
        uint16_t *pRgb16 = (uint16_t *)pRgb;
#if defined(__ARM_NEON) || defined(__ARM_NEON__)
                int32x4_t _VR, _VG, _VB;
                int32x4_t _VRuv, _VGuv, _VBuv;
                int32x4_t _VDelta;
                int32x4_t _VY0;
                int32x4_t _VU;
                int32x4_t _VY1;
                int32x4_t _VV;
                int32_t _tmp[16];
#endif
        for (int count = 0; count < src->size;) {
#if defined(__ARM_NEON) || defined(__ARM_NEON__)
            if (count + 16 < src->size) {
                _tmp[0 ] = pYuv[0 ];_tmp[1 ] = pYuv[4 ];_tmp[2 ] = pYuv[8 ];_tmp[3 ] = pYuv[12];
                _tmp[4 ] = pYuv[1 ];_tmp[5 ] = pYuv[5 ];_tmp[6 ] = pYuv[9 ];_tmp[7 ] = pYuv[13];
                _tmp[8 ] = pYuv[2 ];_tmp[9 ] = pYuv[6 ];_tmp[10] = pYuv[10];_tmp[11] = pYuv[14];
                _tmp[12] = pYuv[3 ];_tmp[13] = pYuv[7 ];_tmp[14] = pYuv[11];_tmp[15] = pYuv[15];

                _VY0 = vld1q_s32(_tmp + 0 );
                _VU  = vld1q_s32(_tmp + 4 );
                _VY1 = vld1q_s32(_tmp + 8 );
                _VV  = vld1q_s32(_tmp + 12);
                
                _VRuv = vsubq_s32(_VV, vdupq_n_s32(128));
                _VBuv = vsubq_s32(_VU, vdupq_n_s32(128));

                _VGuv = vaddq_s32(vmulq_s32(vdupq_n_s32(-389), _VBuv), vmulq_s32(vdupq_n_s32(833), _VRuv));
                _VRuv = vmulq_s32(vdupq_n_s32(1187), _VRuv);
                _VBuv = vmulq_s32(vdupq_n_s32(2066), _VBuv);

                _VDelta = vmulq_s32(vdupq_n_s32(1192), vsubq_s32(_VY0, vdupq_n_s32(16)));

                _VR = vshrq_n_s32(vaddq_s32(_VDelta, _VRuv), 10);
                _VG = vshrq_n_s32(vsubq_s32(_VDelta, _VGuv), 10);
                _VB = vshrq_n_s32(vaddq_s32(_VDelta, _VBuv), 10);

                _VR = vminq_s32(vmaxq_s32(_VR, vdupq_n_s32(0)), vdupq_n_s32(255));
                _VG = vminq_s32(vmaxq_s32(_VG, vdupq_n_s32(0)), vdupq_n_s32(255));
                _VB = vminq_s32(vmaxq_s32(_VB, vdupq_n_s32(0)), vdupq_n_s32(255));

                vst1q_s32(_tmp + 0, _VR);
                vst1q_s32(_tmp + 4, _VG);
                vst1q_s32(_tmp + 8, _VB);
                
                pRgb16[0] = COLOR_R8_G8_B8_TO_RGB565(_tmp[0], _tmp[4], _tmp[8]);
                pRgb16[2] = COLOR_R8_G8_B8_TO_RGB565(_tmp[1], _tmp[5], _tmp[9]);
                pRgb16[4] = COLOR_R8_G8_B8_TO_RGB565(_tmp[2], _tmp[6], _tmp[10]);
                pRgb16[6] = COLOR_R8_G8_B8_TO_RGB565(_tmp[3], _tmp[7], _tmp[11]);

                _VDelta = vmulq_s32(vdupq_n_s32(1164), vsubq_s32(_VY1, vdupq_n_s32(16)));

                _VR = vshrq_n_s32(vaddq_s32(_VDelta, _VRuv), 10);
                _VG = vshrq_n_s32(vsubq_s32(_VDelta, _VGuv), 10);
                _VB = vshrq_n_s32(vaddq_s32(_VDelta, _VBuv), 10);

                _VR = vminq_s32(vmaxq_s32(_VR, vdupq_n_s32(0)), vdupq_n_s32(255));
                _VG = vminq_s32(vmaxq_s32(_VG, vdupq_n_s32(0)), vdupq_n_s32(255));
                _VB = vminq_s32(vmaxq_s32(_VB, vdupq_n_s32(0)), vdupq_n_s32(255));

                vst1q_s32(_tmp + 0, _VR);
                vst1q_s32(_tmp + 4, _VG);
                vst1q_s32(_tmp + 8, _VB);

                pRgb16[1] = COLOR_R8_G8_B8_TO_RGB565(_tmp[0], _tmp[4], _tmp[8]);
                pRgb16[3] = COLOR_R8_G8_B8_TO_RGB565(_tmp[1], _tmp[5], _tmp[9]);
                pRgb16[5] = COLOR_R8_G8_B8_TO_RGB565(_tmp[2], _tmp[6], _tmp[10]);
                pRgb16[7] = COLOR_R8_G8_B8_TO_RGB565(_tmp[3], _tmp[7], _tmp[11]);
                pRgb16 += 8;
                pYuv += 16;
                count += 16;

                continue;
            }
#endif
            Pixel32         = imlib_yuv442_to_rgb(pYuv[0], pYuv[1], pYuv[2], pYuv[3]);
            pRgb16[0] = pPixel[1];
            pRgb16[1] = pPixel[0];
            pRgb16 += 2;
            pYuv += 4;
            count += 4;
        }
    }
}
void imlib_pixfmt_yuv422_to_rgb888(image_t *dst, image_t *src, rectangle_t *roi) {
    if (roi->x == 0 && roi->y == 0 && roi->w == dst->w && roi->h == dst->h && dst->w == src->w && dst->h == src->h) {
        uint8_t *pYuv = src->data;
        uint8_t *pRgb = dst->data;
        uint64_t Pixel64;
        uint32_t *pPixel = (uint32_t *)&Pixel64;
        // Resolution describes the number of pixels, and each YUV pixel occupies 4
        // characters, so here the total number of characters needs to be multiplied
        // by 2.
        pixel24_t *pRgb24 = (pixel24_t *)pRgb;
#if defined(__ARM_NEON) || defined(__ARM_NEON__)
                int32x4_t _VR, _VG, _VB;
                int32x4_t _VRuv, _VGuv, _VBuv;
                int32x4_t _VDelta;
                int32x4_t _VY0;
                int32x4_t _VU;
                int32x4_t _VY1;
                int32x4_t _VV;
                int32_t _tmp[16];
#endif
        for (int count = 0; count < src->size;) {
#if defined(__ARM_NEON) || defined(__ARM_NEON__)
            if (count + 16 < src->size) {
                _tmp[0 ] = pYuv[0 ];_tmp[1 ] = pYuv[4 ];_tmp[2 ] = pYuv[8 ];_tmp[3 ] = pYuv[12];
                _tmp[4 ] = pYuv[1 ];_tmp[5 ] = pYuv[5 ];_tmp[6 ] = pYuv[9 ];_tmp[7 ] = pYuv[13];
                _tmp[8 ] = pYuv[2 ];_tmp[9 ] = pYuv[6 ];_tmp[10] = pYuv[10];_tmp[11] = pYuv[14];
                _tmp[12] = pYuv[3 ];_tmp[13] = pYuv[7 ];_tmp[14] = pYuv[11];_tmp[15] = pYuv[15];

                _VY0 = vld1q_s32(_tmp + 0 );
                _VU  = vld1q_s32(_tmp + 4 );
                _VY1 = vld1q_s32(_tmp + 8 );
                _VV  = vld1q_s32(_tmp + 12);
                

                _VRuv = vsubq_s32(_VV, vdupq_n_s32(128));
                _VBuv = vsubq_s32(_VU, vdupq_n_s32(128));

                _VGuv = vaddq_s32(vmulq_s32(vdupq_n_s32(-389), _VBuv), vmulq_s32(vdupq_n_s32(833), _VRuv));
                _VRuv = vmulq_s32(vdupq_n_s32(1187), _VRuv);
                _VBuv = vmulq_s32(vdupq_n_s32(2066), _VBuv);

                _VDelta = vmulq_s32(vdupq_n_s32(1192), vsubq_s32(_VY0, vdupq_n_s32(16)));

                _VR = vshrq_n_s32(vaddq_s32(_VDelta, _VRuv), 10);
                _VG = vshrq_n_s32(vsubq_s32(_VDelta, _VGuv), 10);
                _VB = vshrq_n_s32(vaddq_s32(_VDelta, _VBuv), 10);

                _VR = vminq_s32(vmaxq_s32(_VR, vdupq_n_s32(0)), vdupq_n_s32(255));
                _VG = vminq_s32(vmaxq_s32(_VG, vdupq_n_s32(0)), vdupq_n_s32(255));
                _VB = vminq_s32(vmaxq_s32(_VB, vdupq_n_s32(0)), vdupq_n_s32(255));

                vst1q_s32(_tmp + 0, _VR);
                vst1q_s32(_tmp + 4, _VG);
                vst1q_s32(_tmp + 8, _VB);
                
                pRgb24[0] = (pixel24_t){.red = _tmp[0],.green = _tmp[4], .blue = _tmp[8 ]} ; // COLOR_R8_G8_B8_TO_RGB888(_tmp[0], _tmp[4], _tmp[8]) ;
                pRgb24[2] = (pixel24_t){.red = _tmp[1],.green = _tmp[5], .blue = _tmp[9 ]} ; // COLOR_R8_G8_B8_TO_RGB888(_tmp[1], _tmp[5], _tmp[9]) ;
                pRgb24[4] = (pixel24_t){.red = _tmp[2],.green = _tmp[6], .blue = _tmp[10]} ; // COLOR_R8_G8_B8_TO_RGB888(_tmp[2], _tmp[6], _tmp[10]);
                pRgb24[6] = (pixel24_t){.red = _tmp[3],.green = _tmp[7], .blue = _tmp[11]} ; // COLOR_R8_G8_B8_TO_RGB888(_tmp[3], _tmp[7], _tmp[11]);

                _VDelta = vmulq_s32(vdupq_n_s32(1164), vsubq_s32(_VY1, vdupq_n_s32(16)));

                _VR = vshrq_n_s32(vaddq_s32(_VDelta, _VRuv), 10);
                _VG = vshrq_n_s32(vsubq_s32(_VDelta, _VGuv), 10);
                _VB = vshrq_n_s32(vaddq_s32(_VDelta, _VBuv), 10);

                _VR = vminq_s32(vmaxq_s32(_VR, vdupq_n_s32(0)), vdupq_n_s32(255));
                _VG = vminq_s32(vmaxq_s32(_VG, vdupq_n_s32(0)), vdupq_n_s32(255));
                _VB = vminq_s32(vmaxq_s32(_VB, vdupq_n_s32(0)), vdupq_n_s32(255));

                vst1q_s32(_tmp + 0, _VR);
                vst1q_s32(_tmp + 4, _VG);
                vst1q_s32(_tmp + 8, _VB);

                pRgb24[1] = (pixel24_t){.red = _tmp[0],.green = _tmp[4], .blue = _tmp[8 ]} ;//COLOR_R8_G8_B8_TO_RGB888(_tmp[0], _tmp[4], _tmp[8]);
                pRgb24[3] = (pixel24_t){.red = _tmp[1],.green = _tmp[5], .blue = _tmp[9 ]} ;//COLOR_R8_G8_B8_TO_RGB888(_tmp[1], _tmp[5], _tmp[9]);
                pRgb24[5] = (pixel24_t){.red = _tmp[2],.green = _tmp[6], .blue = _tmp[10]} ;//COLOR_R8_G8_B8_TO_RGB888(_tmp[2], _tmp[6], _tmp[10]);
                pRgb24[7] = (pixel24_t){.red = _tmp[3],.green = _tmp[7], .blue = _tmp[11]} ;//COLOR_R8_G8_B8_TO_RGB888(_tmp[3], _tmp[7], _tmp[11]);
                pRgb24 += 8;
                pYuv += 16;
                count += 16;

                continue;
            }
#endif            
            Pixel64         = imlib_yuv442_to_rgb888(pYuv[0], pYuv[1], pYuv[2], pYuv[3]);
            pRgb24[0] = pixel32224(pPixel[1]);
            pRgb24[1] = pixel32224(pPixel[0]);
            pYuv += 4;
            pRgb24 += 2;
            count += 4;
        }
    }
}

#define PIXFORMAT_ID_BINARY_NULL NULL
#define PIXFORMAT_ID_GRAY_NULL   NULL
#define PIXFORMAT_ID_RGB565_NULL NULL
#define PIXFORMAT_ID_BAYER_NULL  NULL
#define PIXFORMAT_ID_YUV422_NULL NULL
#define PIXFORMAT_ID_JPEG_NULL   NULL
#define PIXFORMAT_ID_RGB888_NULL NULL
#define PIXFORMAT_ID_PNG_NULL    NULL
#define PIXFORMAT_ID_ARGB8_NULL  NULL

static void (*_pixfmt_to[10][10])(image_t *, image_t *, rectangle_t *) = {
    {NULL},
    {NULL, imlib_pixfmt_binary_to_binary, imlib_pixfmt_binary_to_grayscale, imlib_pixfmt_binary_to_rgb565,
     PIXFORMAT_ID_BAYER_NULL, PIXFORMAT_ID_YUV422_NULL, PIXFORMAT_ID_JPEG_NULL, imlib_pixfmt_binary_to_rgb888,
     PIXFORMAT_ID_PNG_NULL, PIXFORMAT_ID_ARGB8_NULL},
    {NULL, imlib_pixfmt_grayscale_to_binary, imlib_pixfmt_grayscale_to_grayscale, imlib_pixfmt_grayscale_to_rgb565,
     PIXFORMAT_ID_BAYER_NULL, PIXFORMAT_ID_YUV422_NULL, PIXFORMAT_ID_JPEG_NULL, imlib_pixfmt_grayscale_to_rgb888,
     PIXFORMAT_ID_PNG_NULL, PIXFORMAT_ID_ARGB8_NULL},
    {NULL, imlib_pixfmt_rgb565_to_binary, imlib_pixfmt_rgb565_to_grayscale, imlib_pixfmt_rgb565_to_rgb565,
     PIXFORMAT_ID_BAYER_NULL, PIXFORMAT_ID_YUV422_NULL, PIXFORMAT_ID_JPEG_NULL, imlib_pixfmt_rgb565_to_rgb888,
     PIXFORMAT_ID_PNG_NULL, PIXFORMAT_ID_ARGB8_NULL},
    {NULL, PIXFORMAT_ID_BINARY_NULL, PIXFORMAT_ID_GRAY_NULL, PIXFORMAT_ID_RGB565_NULL, PIXFORMAT_ID_BAYER_NULL,
     PIXFORMAT_ID_YUV422_NULL, PIXFORMAT_ID_JPEG_NULL, PIXFORMAT_ID_RGB888_NULL, PIXFORMAT_ID_PNG_NULL,
     PIXFORMAT_ID_ARGB8_NULL},
    {NULL, imlib_pixfmt_yuv422_to_binary, imlib_pixfmt_yuv422_to_grayscale, imlib_pixfmt_yuv422_to_rgb565,
     PIXFORMAT_ID_BAYER_NULL, PIXFORMAT_ID_YUV422_NULL, PIXFORMAT_ID_JPEG_NULL, imlib_pixfmt_yuv422_to_rgb888,
     PIXFORMAT_ID_PNG_NULL, PIXFORMAT_ID_ARGB8_NULL},
    {NULL, imlib_pixfmt_rgb888_to_binary, imlib_pixfmt_rgb888_to_grayscale, imlib_pixfmt_rgb888_to_rgb565,
     PIXFORMAT_ID_BAYER_NULL, PIXFORMAT_ID_YUV422_NULL, PIXFORMAT_ID_JPEG_NULL, imlib_pixfmt_rgb888_to_rgb888,
     PIXFORMAT_ID_PNG_NULL, PIXFORMAT_ID_ARGB8_NULL},
    {NULL, PIXFORMAT_ID_BINARY_NULL, PIXFORMAT_ID_GRAY_NULL, PIXFORMAT_ID_RGB565_NULL, PIXFORMAT_ID_BAYER_NULL,
     PIXFORMAT_ID_YUV422_NULL, PIXFORMAT_ID_JPEG_NULL, PIXFORMAT_ID_RGB888_NULL, PIXFORMAT_ID_PNG_NULL,
     PIXFORMAT_ID_ARGB8_NULL},
    {NULL, PIXFORMAT_ID_BINARY_NULL, PIXFORMAT_ID_GRAY_NULL, PIXFORMAT_ID_RGB565_NULL, PIXFORMAT_ID_BAYER_NULL,
     PIXFORMAT_ID_YUV422_NULL, PIXFORMAT_ID_JPEG_NULL, PIXFORMAT_ID_RGB888_NULL, PIXFORMAT_ID_PNG_NULL,
     PIXFORMAT_ID_ARGB8_NULL}};

void imlib_pixfmt_to(image_t *dst, image_t *src, rectangle_t *roi_i) {
    rectangle_t *roi, tmp_roi;
    if (roi_i == NULL) {
        roi    = &tmp_roi;
        roi->x = 0;
        roi->y = 0;
        roi->w = dst->w;
        roi->h = dst->h;
    } else {
        roi = roi_i;
    }
    _pixfmt_to[src->pixfmt_id][dst->pixfmt_id](dst, src, roi);
}