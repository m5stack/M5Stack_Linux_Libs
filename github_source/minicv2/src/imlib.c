/*
 * This file is part of the OpenMV project.
 *
 * Copyright (c) 2013-2021 Ibrahim Abdelkader <iabdalkader@openmv.io>
 * Copyright (c) 2013-2021 Kwabena W. Agyeman <kwagyeman@openmv.io>
 *
 * This work is licensed under the MIT license, see the file LICENSE for details.
 *
 * Image library.
 */
#include <stdlib.h>
// #include "py/obj.h"
// #include "py/runtime.h"

#include "font.h"
#include "array.h"
// #include "ff_wrapper.h"
#include "imlib.h"
#include "common.h"
#include "omv_boardconfig.h"


void imlib_init_all()
{
    #if (OMV_HARDWARE_JPEG == 1)
    imlib_jpeg_compress_init();
    #endif
    fb_alloc_init0();
    fmath_init();
}

void imlib_deinit_all()
{
    #ifdef IMLIB_ENABLE_DMA2D
    imlib_draw_row_deinit_all();
    #endif
    #if (OMV_HARDWARE_JPEG == 1)
    imlib_jpeg_compress_deinit();
    #endif
    fb_alloc_close0();
}

/////////////////
// Point Stuff //
/////////////////

void point_init(point_t *ptr, int x, int y)
{
    ptr->x = x;
    ptr->y = y;
}

void point_copy(point_t *dst, point_t *src)
{
    memcpy(dst, src, sizeof(point_t));
}

bool point_equal_fast(point_t *ptr0, point_t *ptr1)
{
    return !memcmp(ptr0, ptr1, sizeof(point_t));
}

int point_quadrance(point_t *ptr0, point_t *ptr1)
{
    int delta_x = ptr0->x - ptr1->x;
    int delta_y = ptr0->y - ptr1->y;
    return (delta_x * delta_x) + (delta_y * delta_y);
}

int point_quadrance_i(point_t *ptr0, point_t *ptr1)
{
    int delta_m = point_quadrance(ptr0, ptr1);
    return (int)fast_sqrtf((float)delta_m);
}

void point_rotate(int x, int y, float r, int center_x, int center_y, int16_t *new_x, int16_t *new_y)
{
    x -= center_x;
    y -= center_y;
    *new_x = (x * cosf(r)) - (y * sinf(r)) + center_x;
    *new_y = (x * sinf(r)) + (y * cosf(r)) + center_y;
}

void point_min_area_rectangle(point_t *corners, point_t *new_corners, int corners_len) // Corners need to be sorted!
{
    int i_min = 0;
    int i_min_area = INT_MAX;
    int i_x0 = 0, i_y0 = 0;
    int i_x1 = 0, i_y1 = 0;
    int i_x2 = 0, i_y2 = 0;
    int i_x3 = 0, i_y3 = 0;
    float i_r = 0;

    // This algorithm aligns the 4 edges produced by the 4 corners to the x axis and then computes the
    // min area rect for each alignment. The smallest rect is choosen and then re-rotated and returned.
    for (int i = 0; i < corners_len; i++) {
        int16_t x0 = corners[i].x, y0 = corners[i].y;
        int x_diff = corners[(i+1)%corners_len].x - corners[i].x;
        int y_diff = corners[(i+1)%corners_len].y - corners[i].y;
        float r = -fast_atan2f(y_diff, x_diff);

        int16_t x1[corners_len-1];
        int16_t y1[corners_len-1];
        for (int j = 0, jj = corners_len - 1; j < jj; j++) {
            point_rotate(corners[(i+j+1)%corners_len].x, corners[(i+j+1)%corners_len].y, r, x0, y0, x1 + j, y1 + j);
        }

        int minx = x0;
        int maxx = x0;
        int miny = y0;
        int maxy = y0;
        for (int j = 0, jj = corners_len - 1; j < jj; j++) {
            minx = IM_MIN(minx, x1[j]);
            maxx = IM_MAX(maxx, x1[j]);
            miny = IM_MIN(miny, y1[j]);
            maxy = IM_MAX(maxy, y1[j]);
        }

        int area = (maxx - minx + 1) * (maxy - miny + 1);
        if (area < i_min_area) {
            i_min = i;
            i_min_area = area;
            i_x0 = minx, i_y0 = miny;
            i_x1 = maxx, i_y1 = miny;
            i_x2 = maxx, i_y2 = maxy;
            i_x3 = minx, i_y3 = maxy;
            i_r = r;
        }
    }

    point_rotate(i_x0, i_y0, -i_r, corners[i_min].x, corners[i_min].y, &new_corners[0].x, &new_corners[0].y);
    point_rotate(i_x1, i_y1, -i_r, corners[i_min].x, corners[i_min].y, &new_corners[1].x, &new_corners[1].y);
    point_rotate(i_x2, i_y2, -i_r, corners[i_min].x, corners[i_min].y, &new_corners[2].x, &new_corners[2].y);
    point_rotate(i_x3, i_y3, -i_r, corners[i_min].x, corners[i_min].y, &new_corners[3].x, &new_corners[3].y);
}

////////////////
// Line Stuff //
////////////////

// http://www.skytopia.com/project/articles/compsci/clipping.html
bool lb_clip_line(line_t *l, int x, int y, int w, int h) // line is drawn if this returns true
{
    int xdelta = l->x2 - l->x1, ydelta = l->y2 - l->y1, p[4], q[4];
    float umin = 0, umax = 1;

    p[0] = -(xdelta);
    p[1] = +(xdelta);
    p[2] = -(ydelta);
    p[3] = +(ydelta);

    q[0] = l->x1 - (x);
    q[1] = (x + w - 1) - l->x1;
    q[2] = l->y1 - (y);
    q[3] = (y + h - 1) - l->y1;

    for (int i = 0; i < 4; i++) {
        if (p[i]) {
            float u = ((float) q[i]) / ((float) p[i]);

            if (p[i] < 0) { // outside to inside
                if (u > umax) return false;
                if (u > umin) umin = u;
            }

            if (p[i] > 0) { // inside to outside
                if (u < umin) return false;
                if (u < umax) umax = u;
            }

        } else if (q[i] < 0) {
            return false;
        }
    }

    if (umax < umin) return false;

    int x1_c = l->x1 + (xdelta * umin);
    int y1_c = l->y1 + (ydelta * umin);
    int x2_c = l->x1 + (xdelta * umax);
    int y2_c = l->y1 + (ydelta * umax);
    l->x1 = x1_c;
    l->y1 = y1_c;
    l->x2 = x2_c;
    l->y2 = y2_c;

    return true;
}

/////////////////////
// Rectangle Stuff //
/////////////////////

void rectangle_init(rectangle_t *ptr, int x, int y, int w, int h)
{
    ptr->x = x;
    ptr->y = y;
    ptr->w = w;
    ptr->h = h;
}

void rectangle_copy(rectangle_t *dst, rectangle_t *src)
{
    memcpy(dst, src, sizeof(rectangle_t));
}

bool rectangle_equal_fast(rectangle_t *ptr0, rectangle_t *ptr1)
{
    return !memcmp(ptr0, ptr1, sizeof(rectangle_t));
}

bool rectangle_overlap(rectangle_t *ptr0, rectangle_t *ptr1)
{
    int x0 = ptr0->x;
    int y0 = ptr0->y;
    int w0 = ptr0->w;
    int h0 = ptr0->h;
    int x1 = ptr1->x;
    int y1 = ptr1->y;
    int w1 = ptr1->w;
    int h1 = ptr1->h;
    return (x0 < (x1 + w1)) && (y0 < (y1 + h1)) && (x1 < (x0 + w0)) && (y1 < (y0 + h0));
}

void rectangle_intersected(rectangle_t *dst, rectangle_t *src)
{
    int leftX = IM_MAX(dst->x, src->x);
    int topY = IM_MAX(dst->y, src->y);
    int rightX = IM_MIN(dst->x + dst->w, src->x + src->w);
    int bottomY = IM_MIN(dst->y + dst->h, src->y + src->h);
    dst->x = leftX;
    dst->y = topY;
    dst->w = rightX - leftX;
    dst->h = bottomY - topY;
}

void rectangle_united(rectangle_t *dst, rectangle_t *src)
{
    int leftX = IM_MIN(dst->x, src->x);
    int topY = IM_MIN(dst->y, src->y);
    int rightX = IM_MAX(dst->x + dst->w, src->x + src->w);
    int bottomY = IM_MAX(dst->y + dst->h, src->y + src->h);
    dst->x = leftX;
    dst->y = topY;
    dst->w = rightX - leftX;
    dst->h = bottomY - topY;
}

/////////////////
// Image Stuff //
/////////////////

void imlib_image_init(image_t *ptr, int w, int h, pixformat_t pixfmt, uint32_t size, void *pixels)
{
    ptr->w = w;
    ptr->h = h;
    ptr->pixfmt = pixfmt;
    ptr->size   = size;
    ptr->pixels = pixels;
    ptr->is_data_alloc = false;
}

image_t* imlib_image_create(int w, int h, pixformat_t pixfmt, uint32_t size, void *pixels, bool is_data_alloc)
{
    image_t *ptr = xalloc(sizeof(image_t));
    ptr->w = w;
    ptr->h = h;
    ptr->pixfmt = pixfmt;
    ptr->size   = size;
    ptr->pixels = pixels;
    ptr->is_data_alloc = is_data_alloc;
    if(ptr->is_data_alloc)
    {
        ptr->size = image_size(ptr);
        ptr->pixels = xalloc(ptr->size);
        ptr->is_data_alloc = is_data_alloc;
    }
    return ptr;
}
image_t* imlib_image_create_fb_buff(int w, int h, pixformat_t pixfmt, uint32_t size, void *pixels, bool is_data_alloc)
{
    image_t *ptr = fb_alloc(sizeof(image_t), 0);
    ptr->w = w;
    ptr->h = h;
    ptr->pixfmt = pixfmt;
    ptr->size   = size;
    ptr->pixels = pixels;
    ptr->is_data_alloc = is_data_alloc;
    if(ptr->is_data_alloc)
    {
        ptr->size = image_size(ptr);
        ptr->pixels = fb_alloc(ptr->size, 0);
        ptr->is_data_alloc = is_data_alloc;
    }
    return ptr;
}
void imlib_image_destroy_fb_buff(image_t **obj)
{
    if (*obj)
    {
        if ((*obj)->is_data_alloc)
        {
            fb_free((*obj)->data);
            (*obj)->data = NULL;
        }
        fb_free(*obj);
        *obj = NULL;
    }
}
void imlib_image_destroy(image_t **obj)
{
    if (*obj)
    {
        if ((*obj)->is_data_alloc)
        {
            xfree((*obj)->data);
            (*obj)->data = NULL;
        }
        xfree(*obj);
        *obj = NULL;
    }
}
void image_copy(image_t *dst, image_t *src)
{
    memcpy(dst, src, sizeof(image_t));
}

size_t image_size(image_t *ptr)
{
    switch (ptr->pixfmt) {
        case PIXFORMAT_BINARY: {
            return IMAGE_BINARY_LINE_LEN_BYTES(ptr) * ptr->h;
        }
        case PIXFORMAT_GRAYSCALE:
        case PIXFORMAT_BAYER_ANY: { // re-use
            return IMAGE_GRAYSCALE_LINE_LEN_BYTES(ptr) * ptr->h;
        }
        case PIXFORMAT_RGB565:
        case PIXFORMAT_YUV_ANY: { // re-use
            return IMAGE_RGB565_LINE_LEN_BYTES(ptr) * ptr->h;
        }
        case PIXFORMAT_COMPRESSED_ANY: {
            return ptr->size;
        }
        case PIXFORMAT_RGB888: {
            return IMAGE_RGB888_LINE_LEN_BYTES(ptr) * ptr->h;
        }
        default: {
            return 0;
        }
    }
}

bool image_get_mask_pixel(image_t *ptr, int x, int y)
{
    if ((0 <= x) && (x < ptr->w) && (0 <= y) && (y < ptr->h)) {
        switch (ptr->pixfmt) {
            case PIXFORMAT_BINARY: {
                return IMAGE_GET_BINARY_PIXEL(ptr, x, y);
            }
            case PIXFORMAT_GRAYSCALE: {
                return COLOR_GRAYSCALE_TO_BINARY(IMAGE_GET_GRAYSCALE_PIXEL(ptr, x, y));
            }
            case PIXFORMAT_RGB565: {
                return COLOR_RGB565_TO_BINARY(IMAGE_GET_RGB565_PIXEL(ptr, x, y));
            }
            case PIXFORMAT_RGB888: {
                return COLOR_RGB888_TO_BINARY(IMAGE_GET_RGB888_PIXEL(ptr, x, y));
            }
            default: {
                return false;
            }
        }
    }

    return false;
}

// Gamma uncompress
extern const float xyz_table[256];

const int8_t kernel_gauss_3[3*3] = {
     1, 2, 1,
     2, 4, 2,
     1, 2, 1,
};

const int8_t kernel_gauss_5[5*5] = {
    1,  4,  6,  4, 1,
    4, 16, 24, 16, 4,
    6, 24, 36, 24, 6,
    4, 16, 24, 16, 4,
    1,  4,  6,  4, 1
};

const int kernel_laplacian_3[3*3] = {
     -1, -1, -1,
     -1,  8, -1,
     -1, -1, -1
};

const int kernel_high_pass_3[3*3] = {
    -1, -1, -1,
    -1, +8, -1,
    -1, -1, -1
};

// This function fills a grayscale image from an array of floating point numbers that are scaled
// between min and max. The image w*h must equal the floating point array w*h.
void imlib_fill_image_from_float(image_t *img, int w, int h, float *data, float min, float max,
                                 bool mirror, bool flip, bool dst_transpose, bool src_transpose)
{
    float tmp = min;
    min = (min < max) ? min : max;
    max = (max > tmp) ? max : tmp;

    float diff = 255.f / (max - min);
    int w_1 = w - 1;
    int h_1 = h - 1;

    if (!src_transpose) {
        for (int y = 0; y < h; y++) {
            int y_dst = flip ? (h_1 - y) : y;
            float *raw_row = data + (y * w);
            uint8_t *row_pointer = ((uint8_t *) img->data) + (y_dst * w);
            uint8_t *t_row_pointer = ((uint8_t *) img->data) + y_dst;

            for (int x = 0; x < w; x++) {
                int x_dst = mirror ? (w_1 - x) : x;
                float raw = raw_row[x];

                if (raw < min) {
                    raw = min;
                }

                if (raw > max) {
                    raw = max;
                }

                int pixel = fast_roundf((raw - min) * diff);
                pixel = __USAT(pixel, 8);

                if (!dst_transpose) {
                    row_pointer[x_dst] = pixel;
                } else {
                    t_row_pointer[x_dst * h] = pixel;
                }
            }
        }
    } else {
        for (int x = 0; x < w; x++) {
            int x_dst = mirror ? (w_1 - x) : x;
            float *raw_row = data + (x * h);
            uint8_t *t_row_pointer = ((uint8_t *) img->data) + (x_dst * h);
            uint8_t *row_pointer = ((uint8_t *) img->data) + x_dst;

            for (int y = 0; y < h; y++) {
                int y_dst = flip ? (h_1 - y) : y;
                float raw = raw_row[y];

                if (raw < min) {
                    raw = min;
                }

                if (raw > max) {
                    raw = max;
                }

                int pixel = fast_roundf((raw - min) * diff);
                pixel = __USAT(pixel, 8);

                if (!dst_transpose) {
                    row_pointer[y_dst * w] = pixel;
                } else {
                    t_row_pointer[y_dst] = pixel;
                }
            }
        }
    }
}

int8_t imlib_rgb565_to_l(uint16_t pixel)
{
    float r_lin = xyz_table[COLOR_RGB565_TO_R8(pixel)];
    float g_lin = xyz_table[COLOR_RGB565_TO_G8(pixel)];
    float b_lin = xyz_table[COLOR_RGB565_TO_B8(pixel)];

    float y = ((r_lin * 0.2126f) + (g_lin * 0.7152f) + (b_lin * 0.0722f)) * (1.0f / 100.000f);

    y = (y>0.008856f) ? fast_cbrtf(y) : ((y * 7.787037f) + 0.137931f);

    return IM_LIMIT(fast_floorf(116 * y) - 16, COLOR_L_MIN, COLOR_L_MAX);
}

int8_t imlib_rgb565_to_a(uint16_t pixel)
{
    float r_lin = xyz_table[COLOR_RGB565_TO_R8(pixel)];
    float g_lin = xyz_table[COLOR_RGB565_TO_G8(pixel)];
    float b_lin = xyz_table[COLOR_RGB565_TO_B8(pixel)];

    float x = ((r_lin * 0.4124f) + (g_lin * 0.3576f) + (b_lin * 0.1805f)) * (1.0f / 095.047f);
    float y = ((r_lin * 0.2126f) + (g_lin * 0.7152f) + (b_lin * 0.0722f)) * (1.0f / 100.000f);

    x = (x>0.008856f) ? fast_cbrtf(x) : ((x * 7.787037f) + 0.137931f);
    y = (y>0.008856f) ? fast_cbrtf(y) : ((y * 7.787037f) + 0.137931f);

    return IM_LIMIT(fast_floorf(500 * (x-y)), COLOR_A_MIN, COLOR_A_MAX);
}

int8_t imlib_rgb565_to_b(uint16_t pixel)
{
    float r_lin = xyz_table[COLOR_RGB565_TO_R8(pixel)];
    float g_lin = xyz_table[COLOR_RGB565_TO_G8(pixel)];
    float b_lin = xyz_table[COLOR_RGB565_TO_B8(pixel)];

    float y = ((r_lin * 0.2126f) + (g_lin * 0.7152f) + (b_lin * 0.0722f)) * (1.0f / 100.000f);
    float z = ((r_lin * 0.0193f) + (g_lin * 0.1192f) + (b_lin * 0.9505f)) * (1.0f / 108.883f);

    y = (y>0.008856f) ? fast_cbrtf(y) : ((y * 7.787037f) + 0.137931f);
    z = (z>0.008856f) ? fast_cbrtf(z) : ((z * 7.787037f) + 0.137931f);

    return IM_LIMIT(fast_floorf(200 * (y-z)), COLOR_B_MIN, COLOR_B_MAX);
}
int8_t imlib_rgb888_to_l(uint32_t pixel)
{
    float r_lin = xyz_table[COLOR_RGB888_TO_R8(pixel)];
    float g_lin = xyz_table[COLOR_RGB888_TO_G8(pixel)];
    float b_lin = xyz_table[COLOR_RGB888_TO_B8(pixel)];

    float y = ((r_lin * 0.2126f) + (g_lin * 0.7152f) + (b_lin * 0.0722f)) * (1.0f / 100.000f);

    y = (y>0.008856f) ? fast_cbrtf(y) : ((y * 7.787037f) + 0.137931f);

    return IM_LIMIT(fast_floorf(116 * y) - 16, COLOR_L_MIN, COLOR_L_MAX);
}

int8_t imlib_rgb888_to_a(uint32_t pixel)
{
    float r_lin = xyz_table[COLOR_RGB888_TO_R8(pixel)];
    float g_lin = xyz_table[COLOR_RGB888_TO_G8(pixel)];
    float b_lin = xyz_table[COLOR_RGB888_TO_B8(pixel)];

    float x = ((r_lin * 0.4124f) + (g_lin * 0.3576f) + (b_lin * 0.1805f)) * (1.0f / 095.047f);
    float y = ((r_lin * 0.2126f) + (g_lin * 0.7152f) + (b_lin * 0.0722f)) * (1.0f / 100.000f);

    x = (x>0.008856f) ? fast_cbrtf(x) : ((x * 7.787037f) + 0.137931f);
    y = (y>0.008856f) ? fast_cbrtf(y) : ((y * 7.787037f) + 0.137931f);

    return IM_LIMIT(fast_floorf(500 * (x-y)), COLOR_A_MIN, COLOR_A_MAX);
}

int8_t imlib_rgb888_to_b(uint32_t pixel)
{
    float r_lin = xyz_table[COLOR_RGB888_TO_R8(pixel)];
    float g_lin = xyz_table[COLOR_RGB888_TO_G8(pixel)];
    float b_lin = xyz_table[COLOR_RGB888_TO_B8(pixel)];

    float y = ((r_lin * 0.2126f) + (g_lin * 0.7152f) + (b_lin * 0.0722f)) * (1.0f / 100.000f);
    float z = ((r_lin * 0.0193f) + (g_lin * 0.1192f) + (b_lin * 0.9505f)) * (1.0f / 108.883f);

    y = (y>0.008856f) ? fast_cbrtf(y) : ((y * 7.787037f) + 0.137931f);
    z = (z>0.008856f) ? fast_cbrtf(z) : ((z * 7.787037f) + 0.137931f);

    return IM_LIMIT(fast_floorf(200 * (y-z)), COLOR_B_MIN, COLOR_B_MAX);
}
// https://en.wikipedia.org/wiki/Lab_color_space -> CIELAB-CIEXYZ conversions
// https://en.wikipedia.org/wiki/SRGB -> Specification of the transformation
uint16_t imlib_lab_to_rgb(uint8_t l, int8_t a, int8_t b)
{
    float x = ((l + 16) * 0.008621f) + (a * 0.002f);
    float y = ((l + 16) * 0.008621f);
    float z = ((l + 16) * 0.008621f) - (b * 0.005f);

    x = ((x > 0.206897f) ? (x*x*x) : ((0.128419f * x) - 0.017713f)) * 095.047f;
    y = ((y > 0.206897f) ? (y*y*y) : ((0.128419f * y) - 0.017713f)) * 100.000f;
    z = ((z > 0.206897f) ? (z*z*z) : ((0.128419f * z) - 0.017713f)) * 108.883f;

    float r_lin = ((x * +3.2406f) + (y * -1.5372f) + (z * -0.4986f)) / 100.0f;
    float g_lin = ((x * -0.9689f) + (y * +1.8758f) + (z * +0.0415f)) / 100.0f;
    float b_lin = ((x * +0.0557f) + (y * -0.2040f) + (z * +1.0570f)) / 100.0f;

    r_lin = (r_lin>0.0031308f) ? ((1.055f*powf(r_lin, 0.416666f))-0.055f) : (r_lin*12.92f);
    g_lin = (g_lin>0.0031308f) ? ((1.055f*powf(g_lin, 0.416666f))-0.055f) : (g_lin*12.92f);
    b_lin = (b_lin>0.0031308f) ? ((1.055f*powf(b_lin, 0.416666f))-0.055f) : (b_lin*12.92f);

    uint32_t red   = IM_MAX(IM_MIN(fast_floorf(r_lin * COLOR_R8_MAX), COLOR_R8_MAX), COLOR_R8_MIN);
    uint32_t green = IM_MAX(IM_MIN(fast_floorf(g_lin * COLOR_G8_MAX), COLOR_G8_MAX), COLOR_G8_MIN);
    uint32_t blue  = IM_MAX(IM_MIN(fast_floorf(b_lin * COLOR_B8_MAX), COLOR_B8_MAX), COLOR_B8_MIN);

    return COLOR_R8_G8_B8_TO_RGB565(red, green, blue);
}
uint32_t imlib_lab_to_rgb888(uint8_t l, int8_t a, int8_t b)
{
    float x = ((l + 16) * 0.008621f) + (a * 0.002f);
    float y = ((l + 16) * 0.008621f);
    float z = ((l + 16) * 0.008621f) - (b * 0.005f);

    x = ((x > 0.206897f) ? (x*x*x) : ((0.128419f * x) - 0.017713f)) * 095.047f;
    y = ((y > 0.206897f) ? (y*y*y) : ((0.128419f * y) - 0.017713f)) * 100.000f;
    z = ((z > 0.206897f) ? (z*z*z) : ((0.128419f * z) - 0.017713f)) * 108.883f;

    float r_lin = ((x * +3.2406f) + (y * -1.5372f) + (z * -0.4986f)) / 100.0f;
    float g_lin = ((x * -0.9689f) + (y * +1.8758f) + (z * +0.0415f)) / 100.0f;
    float b_lin = ((x * +0.0557f) + (y * -0.2040f) + (z * +1.0570f)) / 100.0f;

    r_lin = (r_lin>0.0031308f) ? ((1.055f*powf(r_lin, 0.416666f))-0.055f) : (r_lin*12.92f);
    g_lin = (g_lin>0.0031308f) ? ((1.055f*powf(g_lin, 0.416666f))-0.055f) : (g_lin*12.92f);
    b_lin = (b_lin>0.0031308f) ? ((1.055f*powf(b_lin, 0.416666f))-0.055f) : (b_lin*12.92f);

    uint32_t red   = IM_MAX(IM_MIN(fast_floorf(r_lin * COLOR_R8_MAX), COLOR_R8_MAX), COLOR_R8_MIN);
    uint32_t green = IM_MAX(IM_MIN(fast_floorf(g_lin * COLOR_G8_MAX), COLOR_G8_MAX), COLOR_G8_MIN);
    uint32_t blue  = IM_MAX(IM_MIN(fast_floorf(b_lin * COLOR_B8_MAX), COLOR_B8_MAX), COLOR_B8_MIN);

    return COLOR_R8_G8_B8_TO_RGB888(red, green, blue);
}
// https://en.wikipedia.org/wiki/YCbCr -> JPEG Conversion
uint16_t imlib_yuv_to_rgb(uint8_t y, int8_t u, int8_t v)
{
    uint32_t r = IM_MAX(IM_MIN(y + ((91881 * v) >> 16), COLOR_R8_MAX), COLOR_R8_MIN);
    uint32_t g = IM_MAX(IM_MIN(y - (((22554 * u) + (46802 * v)) >> 16), COLOR_G8_MAX), COLOR_G8_MIN);
    uint32_t b = IM_MAX(IM_MIN(y + ((116130 * u) >> 16), COLOR_B8_MAX), COLOR_B8_MIN);

    return COLOR_R8_G8_B8_TO_RGB565(r, g, b);
}
uint32_t imlib_yuv_to_rgb888(uint8_t y, int8_t u, int8_t v)
{
    uint32_t r = IM_MAX(IM_MIN(y + ((91881 * v) >> 16), COLOR_R8_MAX), COLOR_R8_MIN);
    uint32_t g = IM_MAX(IM_MIN(y - (((22554 * u) + (46802 * v)) >> 16), COLOR_G8_MAX), COLOR_G8_MIN);
    uint32_t b = IM_MAX(IM_MIN(y + ((116130 * u) >> 16), COLOR_B8_MAX), COLOR_B8_MIN);

    return COLOR_R8_G8_B8_TO_RGB888(r, g, b);
}
////////////////////////////////////////////////////////////////////////////////

#if defined(IMLIB_ENABLE_IMAGE_FILE_IO)
static save_image_format_t imblib_parse_extension(image_t *img, const char *path)
{
    size_t l = strlen(path);
    const char *p = path + l;
    if (l >= 5) {
               if (((p[-1] == 'g') || (p[-1] == 'G'))
               &&  ((p[-2] == 'e') || (p[-2] == 'E'))
               &&  ((p[-3] == 'p') || (p[-3] == 'P'))
               &&  ((p[-4] == 'j') || (p[-4] == 'J'))
               &&  ((p[-5] == '.') || (p[-5] == '.'))) {
                    // Will convert to JPG if not.
                    return FORMAT_JPG;
        }
    }
    if (l >= 4) {
               if (((p[-1] == 'g') || (p[-1] == 'G'))
               &&  ((p[-2] == 'p') || (p[-2] == 'P'))
               &&  ((p[-3] == 'j') || (p[-3] == 'J'))
               &&  ((p[-4] == '.') || (p[-4] == '.'))) {
                    // Will convert to JPG if not.
                    return FORMAT_JPG;
        } else if (((p[-1] == 'g') || (p[-1] == 'G'))
               &&  ((p[-2] == 'n') || (p[-2] == 'N'))
               &&  ((p[-3] == 'p') || (p[-3] == 'P'))
               &&  ((p[-4] == '.') || (p[-4] == '.'))) {
                    // Will convert to PNG if not.
                    return FORMAT_PNG;
        } else if (((p[-1] == 'p') || (p[-1] == 'P'))
               &&  ((p[-2] == 'm') || (p[-2] == 'M'))
               &&  ((p[-3] == 'b') || (p[-3] == 'B'))
               &&  ((p[-4] == '.') || (p[-4] == '.'))) {
                    if (IM_IS_JPEG(img) || IM_IS_BAYER(img)) {
                        ERR_PRINT("OSError:Image is not BMP!");
                        // mp_raise_msg(&mp_type_OSError, MP_ERROR_TEXT("Image is not BMP!"));
                    }
                    return FORMAT_BMP;
        } else if (((p[-1] == 'm') || (p[-1] == 'M'))
               &&  ((p[-2] == 'p') || (p[-2] == 'P'))
               &&  ((p[-3] == 'p') || (p[-3] == 'P'))
               &&  ((p[-4] == '.') || (p[-4] == '.'))) {
                    if (!IM_IS_RGB565(img)) {
                        ERR_PRINT("OSError:Image is not PPM!");
                        // mp_raise_msg(&mp_type_OSError, MP_ERROR_TEXT("Image is not PPM!"));
                    }
                    return FORMAT_PNM;
        } else if (((p[-1] == 'm') || (p[-1] == 'M'))
               &&  ((p[-2] == 'p') || (p[-2] == 'P'))
               &&  ((p[-3] == 'p') || (p[-3] == 'P'))
               &&  ((p[-4] == '.') || (p[-4] == '.'))) {
                    if (!IM_IS_RGB888(img)) {
                        ERR_PRINT("OSError:Image is not PPM!");
                        // mp_raise_msg(&mp_type_OSError, MP_ERROR_TEXT("Image is not PPM!"));
                    }
                    return FORMAT_PNM;
        } else if (((p[-1] == 'm') || (p[-1] == 'M'))
               &&  ((p[-2] == 'g') || (p[-2] == 'G'))
               &&  ((p[-3] == 'p') || (p[-3] == 'P'))
               &&  ((p[-4] == '.') || (p[-4] == '.'))) {
                    if (!IM_IS_GS(img)) {
                        ERR_PRINT("OSError:Image is not PGM!");
                        // mp_raise_msg(&mp_type_OSError, MP_ERROR_TEXT("Image is not PGM!"));
                    }
                    return FORMAT_PNM;
        } else if (((p[-1] == 'w') || (p[-1] == 'W'))
               &&  ((p[-2] == 'a') || (p[-2] == 'A'))
               &&  ((p[-3] == 'r') || (p[-3] == 'R'))
               &&  ((p[-4] == '.') || (p[-4] == '.'))) {
                    if (!IM_IS_BAYER(img)) {
                        ERR_PRINT("OSError:Image is not BAYER!");
                        // mp_raise_msg(&mp_type_OSError, MP_ERROR_TEXT("Image is not BAYER!"));
                    }
                    return FORMAT_RAW;
        }

    }
    return FORMAT_DONT_CARE;
}

bool imlib_read_geometry(FIL *fp, image_t *img, const char *path, img_read_settings_t *rs)
{
    file_read_open(fp, path);
    char magic[4];
    read_data(fp, &magic, 4);
    file_close(fp);

    bool vflipped = false;
    if ((magic[0]=='P')
    && ((magic[1]=='2') || (magic[1]=='3')
    ||  (magic[1]=='5') || (magic[1]=='6'))) { // PPM
        rs->format = FORMAT_PNM;
        file_read_open(fp, path);
        file_buffer_on(fp); // REMEMBER TO TURN THIS OFF LATER!
        ppm_read_geometry(fp, img, path, &rs->ppm_rs);
    } else if ((magic[0]=='B') && (magic[1]=='M')) { // BMP
        rs->format = FORMAT_BMP;
        file_read_open(fp, path);
        file_buffer_on(fp); // REMEMBER TO TURN THIS OFF LATER!
        vflipped = bmp_read_geometry(fp, img, path, &rs->bmp_rs);
    } else if ((magic[0]==0xFF) && (magic[1]==0xD8)) { // JPG
        rs->format = FORMAT_JPG;
        file_read_open(fp, path);
        // Do not use file_buffer_on() here.
        jpeg_read_geometry(fp, img, path, &rs->jpg_rs);
        file_buffer_on(fp); // REMEMBER TO TURN THIS OFF LATER!
    } else if ((magic[0]==0x89) && (magic[1]==0x50) && (magic[2]==0x4E) && (magic[3]==0x47)) { // PNG
        rs->format = FORMAT_PNG;
        file_read_open(fp, path);
        // Do not use file_buffer_on() here.
        png_read_geometry(fp, img, path, &rs->png_rs);
        file_buffer_on(fp); // REMEMBER TO TURN THIS OFF LATER!
    } else {
        ff_unsupported_format(NULL);
    }
    imblib_parse_extension(img, path); // Enforce extension!
    return vflipped;
}

static void imlib_read_pixels(FIL *fp, image_t *img, int n_lines, img_read_settings_t *rs)
{
    switch (rs->format) {
        case FORMAT_BMP:
            bmp_read_pixels(fp, img, n_lines, &rs->bmp_rs);
            break;
        case FORMAT_PNM:
            ppm_read_pixels(fp, img, n_lines, &rs->ppm_rs);
            break;
        default: // won't happen
            break;
    }
}
#endif  //IMLIB_ENABLE_IMAGE_FILE_IO

void imlib_image_operation(image_t *img, const char *path, image_t *other, int scalar, line_op_t op, void *data)
{
    if (path) {
        #if defined(IMLIB_ENABLE_IMAGE_FILE_IO)
        uint32_t size = fb_avail() / 2;
        void *alloc = fb_alloc(size, FB_ALLOC_NO_HINT); // We have to do this before the read.
        // This code reads a window of an image in at a time and then executes
        // the line operation on each line in that window before moving to the
        // next window. The vflipped part is here because BMP files can be saved
        // vertically flipped resulting in us reading the image backwards.
        FIL fp;
        image_t temp;
        img_read_settings_t rs;
        bool vflipped = imlib_read_geometry(&fp, &temp, path, &rs);
        if (!IM_EQUAL(img, &temp)) {
            // f_close(&fp);
            file_close(&fp);
            ERR_PRINT("OSError:Images not equal!");
            // mp_raise_msg(&mp_type_OSError, MP_ERROR_TEXT("Images not equal!"));
        }
        // When processing vertically flipped images the read function will fill
        // the window up from the bottom. The read function assumes that the
        // window is equal to an image in size. However, since this is not the
        // case we shrink the window size to how many lines we're buffering.
        temp.pixels = alloc;
        // Set the max buffer height to image height.
        temp.h = IM_MIN(img->h, (size / (temp.w * temp.bpp)));
        // This should never happen unless someone forgot to free.
        if ((!temp.pixels) || (!temp.h)) {
            ERR_PRINT("OSError:Not enough memory available!");
            // mp_raise_msg(&mp_type_MemoryError, MP_ERROR_TEXT("Not enough memory available!"));
        }
        for (int i=0; i<img->h; i+=temp.h) { // goes past end
            int lines = IM_MIN(temp.h, img->h-i);
            imlib_read_pixels(&fp, &temp, lines, &rs);
            for (int j=0; j<lines; j++) {
                if (!vflipped) {
                    op(img, i+j, temp.pixels+(temp.w*temp.bpp*j), data, false);
                } else {
                    op(img, (img->h-i-lines)+j, temp.pixels+(temp.w*temp.bpp*j), data, true);
                }
            }
        }
        file_buffer_off(&fp);
        file_close(&fp);
        fb_free(alloc);
        #else
        // mp_raise_msg(&mp_type_OSError, MP_ERROR_TEXT("Image I/O is not supported"));
        #endif
    } else if (other) {
        if (!IM_EQUAL(img, other)) {
            // mp_raise_msg(&mp_type_OSError, MP_ERROR_TEXT("Images not equal!"));
        }
        switch (img->pixfmt) {
            case PIXFORMAT_BINARY: {
                for (int i=0, ii=img->h; i<ii; i++) {
                    op(img, i, IMAGE_COMPUTE_BINARY_PIXEL_ROW_PTR(other, i), data, false);
                }
                break;
            }
            case PIXFORMAT_GRAYSCALE: {
                for (int i=0, ii=img->h; i<ii; i++) {
                    op(img, i, IMAGE_COMPUTE_GRAYSCALE_PIXEL_ROW_PTR(other, i), data, false);
                }
                break;
            }
            case PIXFORMAT_RGB565: {
                for (int i=0, ii=img->h; i<ii; i++) {
                    op(img, i, IMAGE_COMPUTE_RGB565_PIXEL_ROW_PTR(other, i), data, false);
                }
                break;
            }
            case PIXFORMAT_RGB888: {
                for (int i=0, ii=img->h; i<ii; i++) {
                    op(img, i, IMAGE_COMPUTE_RGB888_PIXEL_ROW_PTR(other, i), data, false);
                }
                break;
            }
            default: {
                break;
            }
        }
    } else {
        switch (img->pixfmt) {
            case PIXFORMAT_BINARY: {
                uint32_t *row_ptr = fb_alloc(IMAGE_BINARY_LINE_LEN_BYTES(img), FB_ALLOC_NO_HINT);

                for (int i=0, ii=img->w; i<ii; i++) {
                    IMAGE_PUT_BINARY_PIXEL_FAST(row_ptr, i, scalar);
                }

                for (int i=0, ii=img->h; i<ii; i++) {
                    op(img, i, row_ptr, data, false);
                }

                fb_free(row_ptr);
                break;
            }
            case PIXFORMAT_GRAYSCALE: {
                uint8_t *row_ptr = fb_alloc(IMAGE_GRAYSCALE_LINE_LEN_BYTES(img), FB_ALLOC_NO_HINT);

                for (int i=0, ii=img->w; i<ii; i++) {
                    IMAGE_PUT_GRAYSCALE_PIXEL_FAST(row_ptr, i, scalar);
                }

                for (int i=0, ii=img->h; i<ii; i++) {
                    op(img, i, row_ptr, data, false);
                }

                fb_free(row_ptr);
                break;
            }
            case PIXFORMAT_RGB565: {
                uint16_t *row_ptr = fb_alloc(IMAGE_RGB565_LINE_LEN_BYTES(img), FB_ALLOC_NO_HINT);

                for (int i=0, ii=img->w; i<ii; i++) {
                    IMAGE_PUT_RGB565_PIXEL_FAST(row_ptr, i, scalar);
                }

                for (int i=0, ii=img->h; i<ii; i++) {
                    op(img, i, row_ptr, data, false);
                }

                fb_free(row_ptr);
                break;
            }
            case PIXFORMAT_RGB888: {
                pixel24_t *row_ptr = fb_alloc(IMAGE_RGB888_LINE_LEN_BYTES(img), FB_ALLOC_NO_HINT);

                for (int i=0, ii=img->w; i<ii; i++) {
                    IMAGE_PUT_RGB888_PIXEL_FAST(row_ptr, i, scalar);
                }

                for (int i=0, ii=img->h; i<ii; i++) {
                    op(img, i, row_ptr, data, false);
                }

                fb_free(row_ptr);
                break;
            }
            default: {
                break;
            }
        }
    }
}

#if defined(IMLIB_ENABLE_IMAGE_FILE_IO)
void imlib_load_image(image_t *img, const char *path)
{
    FIL fp;
    file_read_open(&fp, path);
    char magic[4];
    read_data(&fp, &magic, 4);
    file_close(&fp);

    if ((magic[0]=='P')
    && ((magic[1]=='2') || (magic[1]=='3')
    ||  (magic[1]=='5') || (magic[1]=='6'))) { // PPM
        ppm_read(img, path);
    } else if ((magic[0]=='B') && (magic[1]=='M')) { // BMP
        bmp_read(img, path);
    } else if ((magic[0]==0xFF) && (magic[1]==0xD8)) { // JPEG
        jpeg_read(img, path);
    } else if ((magic[0]==0x89) && (magic[1]==0x50) && (magic[2]==0x4E) && (magic[3]==0x47)) { // PNG
        png_read(img, path);
    } else {
        ff_unsupported_format(NULL);
    }
    imblib_parse_extension(img, path); // Enforce extension!
}

void imlib_save_image(image_t *img, const char *path, rectangle_t *roi, int quality)
{
    switch (imblib_parse_extension(img, path)) {
        case FORMAT_BMP:
            bmp_write_subimg(img, path, roi);
            break;
        case FORMAT_PNM:
            ppm_write_subimg(img, path, roi);
            break;
        case FORMAT_RAW: {
            FIL fp;
            file_write_open(&fp, path);
            write_data(&fp, img->pixels, img->w * img->h);
            file_close(&fp);
            break;
        }
        case FORMAT_JPG:
            jpeg_write(img, path, quality);
            break;
        case FORMAT_PNG:
            png_write(img, path);
            break;
        case FORMAT_DONT_CARE:
            // Path doesn't have an extension.
            if (IM_IS_JPEG(img)) {
                char *new_path = strcat(strcpy(fb_alloc(strlen(path)+5, FB_ALLOC_NO_HINT), path), ".jpg");
                jpeg_write(img, new_path, quality);
                fb_free(NULL);
            } else if (img->pixfmt == PIXFORMAT_PNG) {
                char *new_path = strcat(strcpy(fb_alloc(strlen(path)+5, FB_ALLOC_NO_HINT), path), ".png");
                png_write(img, new_path);
                fb_free(NULL);
            } else if (IM_IS_BAYER(img)) {
                FIL fp;
                char *new_path = strcat(strcpy(fb_alloc(strlen(path)+5, FB_ALLOC_NO_HINT), path), ".raw");
                file_write_open(&fp, new_path);
                write_data(&fp, img->pixels, img->w * img->h);
                file_close(&fp);
                fb_free(NULL);
            } else { // RGB or GS, save as BMP.
                char *new_path = strcat(strcpy(fb_alloc(strlen(path)+5, FB_ALLOC_NO_HINT), path), ".bmp");
                bmp_write_subimg(img, new_path, roi);
                fb_free(NULL);
            }
            break;
    }
}
#endif //IMLIB_ENABLE_IMAGE_FILE_IO

////////////////////////////////////////////////////////////////////////////////

void imlib_zero(image_t *img, image_t *mask, bool invert)
{
    switch (img->pixfmt) {
        case PIXFORMAT_BINARY: {
            for (int y = 0, yy = img->h; y < yy; y++) {
                uint32_t *row_ptr = IMAGE_COMPUTE_BINARY_PIXEL_ROW_PTR(img, y);
                for (int x = 0, xx = img->w; x < xx; x++) {
                    if (image_get_mask_pixel(mask, x, y) ^ invert) {
                        IMAGE_PUT_BINARY_PIXEL_FAST(row_ptr, x, 0);
                    }
                }
            }
            break;
        }
        case PIXFORMAT_GRAYSCALE: {
            for (int y = 0, yy = img->h; y < yy; y++) {
                uint8_t *row_ptr = IMAGE_COMPUTE_GRAYSCALE_PIXEL_ROW_PTR(img, y);
                for (int x = 0, xx = img->w; x < xx; x++) {
                    if (image_get_mask_pixel(mask, x, y) ^ invert) {
                        IMAGE_PUT_GRAYSCALE_PIXEL_FAST(row_ptr, x, 0);
                    }
                }
            }
            break;
        }
        case PIXFORMAT_RGB565: {
            for (int y = 0, yy = img->h; y < yy; y++) {
                uint16_t *row_ptr = IMAGE_COMPUTE_RGB565_PIXEL_ROW_PTR(img, y);
                for (int x = 0, xx = img->w; x < xx; x++) {
                    if (image_get_mask_pixel(mask, x, y) ^ invert) {
                        IMAGE_PUT_RGB565_PIXEL_FAST(row_ptr, x, 0);
                    }
                }
            }
            break;
        }
        case PIXFORMAT_RGB888: {
            for (int y = 0, yy = img->h; y < yy; y++) {
                pixel24_t *row_ptr = IMAGE_COMPUTE_RGB888_PIXEL_ROW_PTR(img, y);
                for (int x = 0, xx = img->w; x < xx; x++) {
                    if (image_get_mask_pixel(mask, x, y) ^ invert) {
                        IMAGE_PUT_RGB888_PIXEL_FAST(row_ptr, x, 0);
                    }
                }
            }
            break;
        }
        default: {
            break;
        }
    }
}

#ifdef IMLIB_ENABLE_LENS_CORR
// A simple algorithm for correcting lens distortion.
// See http://www.tannerhelland.com/4743/simple-algorithm-correcting-lens-distortion/
void imlib_lens_corr(image_t *img, float strength, float zoom, float x_corr, float y_corr)
{
    int w = img->w;
    int h = img->h;
    int halfWidth = w / 2;
    int halfHeight = h / 2;
    float maximum_diameter = fast_sqrtf((w * w) + (h * h));
    float lens_corr_diameter = strength / maximum_diameter;
    zoom = 1 / zoom;

    // Convert percentage offset to pixels from center of image
    int x_off = w * x_corr;
    int y_off = h * y_corr;

    // Create a tmp copy of the image to pull pixels from.
    size_t size = image_size(img);
    void *data = fb_alloc(size, FB_ALLOC_NO_HINT);
    memcpy(data, img->data, size);
    memset(img->data, 0, size);

    int maximum_radius = fast_ceilf(maximum_diameter / 2) + 1; // +1 inclusive of final value
    float *precalculated_table = fb_alloc(maximum_radius * sizeof(float), FB_ALLOC_NO_HINT);

    for(int i=0; i < maximum_radius; i++) {
        float r = lens_corr_diameter * i;
        precalculated_table[i] = (fast_atanf(r) / r) * zoom;
    }

    int down_adj = halfHeight + y_off;
    int up_adj = h - 1 - halfHeight + y_off;
    int right_adj = halfWidth + x_off;
    int left_adj = w - 1 - halfWidth + x_off;

    switch (img->pixfmt) {
        case PIXFORMAT_BINARY: {
            uint32_t *tmp = (uint32_t *) data;

            for (int y = 0; y < halfHeight; y++) {
                uint32_t *row_ptr = IMAGE_COMPUTE_BINARY_PIXEL_ROW_PTR(img, y);
                uint32_t *row_ptr2 = IMAGE_COMPUTE_BINARY_PIXEL_ROW_PTR(img, h-1-y);
                int newY = y - halfHeight;
                int newY2 = newY * newY;

                for (int x = 0; x < halfWidth; x++) {
                    int newX = x - halfWidth;
                    int newX2 = newX * newX;
                    float precalculated = precalculated_table[(int)fast_sqrtf(newX2 + newY2)];
                    int sourceY = fast_roundf(precalculated * newY); // rounding is necessary
                    int sourceX = fast_roundf(precalculated * newX); // rounding is necessary
                    int sourceY_down = down_adj + sourceY;
                    int sourceY_up = up_adj - sourceY;
                    int sourceX_right = right_adj + sourceX;
                    int sourceX_left = left_adj - sourceX;

                    // plot the 4 symmetrical pixels
                    // top 2 pixels
                    if (sourceY_down >= 0 && sourceY_down < h) {
                        uint32_t *ptr = tmp + (((w + UINT32_T_MASK) >> UINT32_T_SHIFT) * sourceY_down);

                        if (sourceX_right >= 0 && sourceX_right < w) {
                            uint8_t pixel = IMAGE_GET_BINARY_PIXEL_FAST(ptr, sourceX_right);
                            IMAGE_PUT_BINARY_PIXEL_FAST(row_ptr, x, pixel);
                        }

                        if (sourceX_left >= 0 && sourceX_left < w) {
                            uint8_t pixel = IMAGE_GET_BINARY_PIXEL_FAST(ptr, sourceX_left);
                            IMAGE_PUT_BINARY_PIXEL_FAST(row_ptr, w - 1 - x, pixel);
                        }
                    }

                    // bottom 2 pixels
                    if (sourceY_up >= 0 && sourceY_up < h) {
                        uint32_t *ptr = tmp + (((w + UINT32_T_MASK) >> UINT32_T_SHIFT) * sourceY_up);

                        if (sourceX_right >= 0 && sourceX_right < w) {
                            uint8_t pixel = IMAGE_GET_BINARY_PIXEL_FAST(ptr, sourceX_right);
                            IMAGE_PUT_BINARY_PIXEL_FAST(row_ptr2, x, pixel);
                        }

                        if (sourceX_left >= 0 && sourceX_left < w) {
                            uint8_t pixel = IMAGE_GET_BINARY_PIXEL_FAST(ptr, sourceX_left);
                            IMAGE_PUT_BINARY_PIXEL_FAST(row_ptr2, w - 1 - x, pixel);
                        }
                    }
                }
            }
            break;
        }
        case PIXFORMAT_GRAYSCALE: {
            uint8_t *tmp = (uint8_t *) data;

            for (int y = 0; y < halfHeight; y++) {
                uint8_t *row_ptr = IMAGE_COMPUTE_GRAYSCALE_PIXEL_ROW_PTR(img, y);
                uint8_t *row_ptr2 = IMAGE_COMPUTE_GRAYSCALE_PIXEL_ROW_PTR(img, h-1-y);
                int newY = y - halfHeight;
                int newY2 = newY * newY;

                for (int x = 0; x < halfWidth; x++) {
                    int newX = x - halfWidth;
                    int newX2 = newX * newX;
                    float precalculated = precalculated_table[(int)fast_sqrtf(newX2 + newY2)];
                    int sourceY = fast_roundf(precalculated * newY); // rounding is necessary
                    int sourceX = fast_roundf(precalculated * newX); // rounding is necessary
                    int sourceY_down = down_adj + sourceY;
                    int sourceY_up = up_adj - sourceY;
                    int sourceX_right = right_adj + sourceX;
                    int sourceX_left = left_adj - sourceX;

                    // plot the 4 symmetrical pixels
                    // top 2 pixels
                    if (sourceY_down >= 0 && sourceY_down < h) {
                        uint8_t *ptr = tmp + (w * sourceY_down);

                        if (sourceX_right >= 0 && sourceX_right < w) {
                            row_ptr[x] = ptr[sourceX_right];
                        }

                        if (sourceX_left >= 0 && sourceX_left < w) {
                            row_ptr[w - 1 - x] = ptr[sourceX_left];
                        }
                    }

                    // bottom 2 pixels
                    if (sourceY_up >= 0 && sourceY_up < h) {
                        uint8_t *ptr = tmp + (w * sourceY_up);

                        if (sourceX_right >= 0 && sourceX_right < w) {
                            row_ptr2[x] = ptr[sourceX_right];
                        }

                        if (sourceX_left >= 0 && sourceX_left < w) {
                            row_ptr2[w - 1 - x] = ptr[sourceX_left];
                        }
                    }
                }
            }
            break;
        }
        case PIXFORMAT_RGB565: {
            uint16_t *tmp = (uint16_t *) data;

            for (int y = 0; y < halfHeight; y++) {
                uint16_t *row_ptr = IMAGE_COMPUTE_RGB565_PIXEL_ROW_PTR(img, y);
                uint16_t *row_ptr2 = IMAGE_COMPUTE_RGB565_PIXEL_ROW_PTR(img, h-1-y);
                int newY = y - halfHeight;
                int newY2 = newY * newY;

                for (int x = 0; x < halfWidth; x++) {
                    int newX = x - halfWidth;
                    int newX2 = newX * newX;
                    float precalculated = precalculated_table[(int)fast_sqrtf(newX2 + newY2)];
                    int sourceY = fast_roundf(precalculated * newY); // rounding is necessary
                    int sourceX = fast_roundf(precalculated * newX); // rounding is necessary
                    int sourceY_down = down_adj + sourceY;
                    int sourceY_up = up_adj - sourceY;
                    int sourceX_right = right_adj + sourceX;
                    int sourceX_left = left_adj - sourceX;

                    // plot the 4 symmetrical pixels
                    // top 2 pixels
                    if (sourceY_down >= 0 && sourceY_down < h) {
                        uint16_t *ptr = tmp + (w * sourceY_down);

                        if (sourceX_right >= 0 && sourceX_right < w) {
                            row_ptr[x] = ptr[sourceX_right];
                        }

                        if (sourceX_left >= 0 && sourceX_left < w) {
                            row_ptr[w - 1 - x] = ptr[sourceX_left];
                        }
                    }

                    // bottom 2 pixels
                    if (sourceY_up >= 0 && sourceY_up < h) {
                        uint16_t *ptr = tmp + (w * sourceY_up);

                        if (sourceX_right >= 0 && sourceX_right < w) {
                            row_ptr2[x] = ptr[sourceX_right];
                        }

                        if (sourceX_left >= 0 && sourceX_left < w) {
                            row_ptr2[w - 1 - x] = ptr[sourceX_left];
                        }
                    }
                }
            }
            break;
        }
        case PIXFORMAT_RGB888: {
            pixel24_t *tmp = (pixel24_t *) data;

            for (int y = 0; y < halfHeight; y++) {
                pixel24_t *row_ptr = IMAGE_COMPUTE_RGB888_PIXEL_ROW_PTR(img, y);
                pixel24_t *row_ptr2 = IMAGE_COMPUTE_RGB888_PIXEL_ROW_PTR(img, h-1-y);
                int newY = y - halfHeight;
                int newY2 = newY * newY;

                for (int x = 0; x < halfWidth; x++) {
                    int newX = x - halfWidth;
                    int newX2 = newX * newX;
                    float precalculated = precalculated_table[(int)fast_sqrtf(newX2 + newY2)];
                    int sourceY = fast_roundf(precalculated * newY); // rounding is necessary
                    int sourceX = fast_roundf(precalculated * newX); // rounding is necessary
                    int sourceY_down = down_adj + sourceY;
                    int sourceY_up = up_adj - sourceY;
                    int sourceX_right = right_adj + sourceX;
                    int sourceX_left = left_adj - sourceX;

                    // plot the 4 symmetrical pixels
                    // top 2 pixels
                    if (sourceY_down >= 0 && sourceY_down < h) {
                        pixel24_t *ptr = tmp + (w * sourceY_down);

                        if (sourceX_right >= 0 && sourceX_right < w) {
                            row_ptr[x] = ptr[sourceX_right];
                        }

                        if (sourceX_left >= 0 && sourceX_left < w) {
                            row_ptr[w - 1 - x] = ptr[sourceX_left];
                        }
                    }

                    // bottom 2 pixels
                    if (sourceY_up >= 0 && sourceY_up < h) {
                        pixel24_t *ptr = tmp + (w * sourceY_up);

                        if (sourceX_right >= 0 && sourceX_right < w) {
                            row_ptr2[x] = ptr[sourceX_right];
                        }

                        if (sourceX_left >= 0 && sourceX_left < w) {
                            row_ptr2[w - 1 - x] = ptr[sourceX_left];
                        }
                    }
                }
            }
            break;
        }
        default: {
            break;
        }
    }
    fb_free(precalculated_table); // precalculated_table
    fb_free(data); // data
}
#endif //IMLIB_ENABLE_LENS_CORR

////////////////////////////////////////////////////////////////////////////////

int imlib_image_mean(image_t *src, int *r_mean, int *g_mean, int *b_mean)
{
    int r_s = 0;
    int g_s = 0;
    int b_s = 0;
    int n = src->w * src->h;

    switch (src->pixfmt) {
        case PIXFORMAT_BINARY: {
            // Can't run this on a binary image.
            break;
        }
        case PIXFORMAT_GRAYSCALE: {
            for (int i=0; i<n; i++) {
                r_s += src->pixels[i];
            }
            *r_mean = r_s/n;
            *g_mean = r_s/n;
            *b_mean = r_s/n;
            break;
        }
        case PIXFORMAT_RGB565: {
            for (int i=0; i<n; i++) {
                uint16_t p = ((uint16_t*)src->pixels)[i];
                r_s += COLOR_RGB565_TO_R8(p);
                g_s += COLOR_RGB565_TO_G8(p);
                b_s += COLOR_RGB565_TO_B8(p);
            }
            *r_mean = r_s/n;
            *g_mean = g_s/n;
            *b_mean = b_s/n;
            break;
        }
        case PIXFORMAT_RGB888: {
            for (int i=0; i<n; i++) {
                pixel24_t p = ((pixel24_t*)src->pixels)[i];
                r_s += COLOR_RGB888_TO_R8(pixel24232(p));
                g_s += COLOR_RGB888_TO_G8(pixel24232(p));
                b_s += COLOR_RGB888_TO_B8(pixel24232(p));
            }
            *r_mean = r_s/n;
            *g_mean = g_s/n;
            *b_mean = b_s/n;
            break;
        }
        default: {
            break;
        }
    }

    return 0;
}

// One pass standard deviation.
int imlib_image_std(image_t *src)
{
    int w=src->w;
    int h=src->h;
    int n=w*h;
    uint8_t *data=src->pixels;

    uint32_t s=0, sq=0;
    for (int i=0; i<n; i+=2) {
        s += data[i+0]+data[i+1];
        uint32_t tmp = __PKHBT(data[i+0], data[i+1], 16);
        sq = __SMLAD(tmp, tmp, sq);
    }

    if (n%2) {
        s += data[n-1];
        sq += data[n-1]*data[n-1];
    }

    /* mean */
    int m = s/n;

    /* variance */
    uint32_t v = sq/n-(m*m);

    /* std */
    return fast_sqrtf(v);
}

void imlib_sepconv3(image_t *img, const int8_t *krn, const float m, const int b)
{
    int ksize = 3;
    // TODO: Support RGB
    int *buffer = fb_alloc(img->w * sizeof(*buffer) * 2, FB_ALLOC_NO_HINT);

    // NOTE: This doesn't deal with borders right now. Adding if
    // statements in the inner loop will slow it down significantly.
    for (int y=0; y<img->h-ksize; y++) {
        for (int x=0; x<img->w; x++) {
            int acc=0;
            //if (IM_X_INSIDE(img, x+k) && IM_Y_INSIDE(img, y+j))
            acc = __SMLAD(krn[0], IM_GET_GS_PIXEL(img, x, y + 0), acc);
            acc = __SMLAD(krn[1], IM_GET_GS_PIXEL(img, x, y + 1), acc);
            acc = __SMLAD(krn[2], IM_GET_GS_PIXEL(img, x, y + 2), acc);
            buffer[((y%2)*img->w) + x] = acc;
        }
        if (y > 0) {
            // flush buffer
            for (int x=0; x<img->w-ksize; x++) {
                int acc = 0;
                acc = __SMLAD(krn[0], buffer[((y-1)%2) * img->w + x + 0], acc);
                acc = __SMLAD(krn[1], buffer[((y-1)%2) * img->w + x + 1], acc);
                acc = __SMLAD(krn[2], buffer[((y-1)%2) * img->w + x + 2], acc);
                acc = (acc * m) + b; // scale, offset, and clamp
                acc = IM_MAX(IM_MIN(acc, IM_MAX_GS), 0);
                IM_SET_GS_PIXEL(img, (x+1), (y), acc);
            }
        }
    }
    fb_free(buffer);
}




// 下面的函数是 maixpy 的一些函数 有一些定制函数


typedef struct xylf
{
    int16_t x, y, l, r;
}
xylf_t;


size_t __imlib_flood_fill_int(image_t *out, image_t *img, int x, int y,
                          int seed_threshold, int floating_threshold,
                          flood_fill_call_back_t cb, void *data)
{
    lifo_t lifo;
    size_t lifo_len;
    lifo_alloc_all(&lifo, &lifo_len, sizeof(xylf_t));
	size_t count = 0;

    for(int seed_pixel = IMAGE_GET_GRAYSCALE_PIXEL(img, x, y);;) {
        int left = x, right = x;
        uint8_t *row = IMAGE_COMPUTE_GRAYSCALE_PIXEL_ROW_PTR(img, y);
        uint32_t *out_row = IMAGE_COMPUTE_BINARY_PIXEL_ROW_PTR(out, y);

        while ((left > 0)
        && (!IMAGE_GET_BINARY_PIXEL_FAST(out_row, left - 1))
        && COLOR_BOUND_GRAYSCALE(IMAGE_GET_GRAYSCALE_PIXEL_FAST(row, left - 1), seed_pixel, seed_threshold)
        && COLOR_BOUND_GRAYSCALE(IMAGE_GET_GRAYSCALE_PIXEL_FAST(row, left - 1),
                                    IMAGE_GET_GRAYSCALE_PIXEL_FAST(row, left), floating_threshold)) {
            left--;
        }

        while ((right < (img->w - 1))
        && (!IMAGE_GET_BINARY_PIXEL_FAST(out_row, right + 1))
        && COLOR_BOUND_GRAYSCALE(IMAGE_GET_GRAYSCALE_PIXEL_FAST(row, right + 1), seed_pixel, seed_threshold)
        && COLOR_BOUND_GRAYSCALE(IMAGE_GET_GRAYSCALE_PIXEL_FAST(row, right + 1),
                                    IMAGE_GET_GRAYSCALE_PIXEL_FAST(row, right), floating_threshold)) {
            right++;
        }

        for (int i = left; i <= right; i++) {
            IMAGE_SET_BINARY_PIXEL_FAST(out_row, i);
        }
        count += (right-left+1);

        bool break_out = false;
        for(;;) {
            if (lifo_size(&lifo) < lifo_len) {
                uint8_t *old_row = IMAGE_COMPUTE_GRAYSCALE_PIXEL_ROW_PTR(img, y);

                if (y > 0) {
                    row = IMAGE_COMPUTE_GRAYSCALE_PIXEL_ROW_PTR(img, y - 1);
                    out_row = IMAGE_COMPUTE_BINARY_PIXEL_ROW_PTR(out, y - 1);

                    bool recurse = false;
                    for (int i = left; i <= right; i++) {
                        if ((!IMAGE_GET_BINARY_PIXEL_FAST(out_row, i))
                        && COLOR_BOUND_GRAYSCALE(IMAGE_GET_GRAYSCALE_PIXEL_FAST(row, i), seed_pixel, seed_threshold)
                        && COLOR_BOUND_GRAYSCALE(IMAGE_GET_GRAYSCALE_PIXEL_FAST(row, i),
                                                    IMAGE_GET_GRAYSCALE_PIXEL_FAST(old_row, i), floating_threshold)) {
                            xylf_t context;
                            context.x = x;
                            context.y = y;
                            context.l = left;
                            context.r = right;
                            lifo_enqueue(&lifo, &context);
                            x = i;
                            y = y - 1;
                            recurse = true;
                            break;
                        }
                    }
                    if (recurse) {
                        break;
                    }
                }

                if (y < (img->h - 1)) {
                    row = IMAGE_COMPUTE_GRAYSCALE_PIXEL_ROW_PTR(img, y + 1);
                    out_row = IMAGE_COMPUTE_BINARY_PIXEL_ROW_PTR(out, y + 1);

                    bool recurse = false;
                    for (int i = left; i <= right; i++) {
                        if ((!IMAGE_GET_BINARY_PIXEL_FAST(out_row, i))
                        && COLOR_BOUND_GRAYSCALE(IMAGE_GET_GRAYSCALE_PIXEL_FAST(row, i), seed_pixel, seed_threshold)
                        && COLOR_BOUND_GRAYSCALE(IMAGE_GET_GRAYSCALE_PIXEL_FAST(row, i),
                                                    IMAGE_GET_GRAYSCALE_PIXEL_FAST(old_row, i), floating_threshold)) {
                            xylf_t context;
                            context.x = x;
                            context.y = y;
                            context.l = left;
                            context.r = right;
                            lifo_enqueue(&lifo, &context);
                            x = i;
                            y = y + 1;
                            recurse = true;
                            break;
                        }
                    }
                    if (recurse) {
                        break;
                    }
                }
            }

            if (cb) cb(img, y, left, right, data);

            if (!lifo_size(&lifo)) {
                break_out = true;
                break;
            }

            xylf_t context;
            lifo_dequeue(&lifo, &context);
            x = context.x;
            y = context.y;
            left = context.l;
            right = context.r;
        }

        if (break_out) {
            break;
        }
    }


    lifo_free(&lifo);
	return count;
}



// #ifdef IMLIB_ENABLE_FLOOD_FILL

size_t __imlib_flood_fill(image_t *img, int x, int y,
                      float seed_threshold, float floating_threshold,
                      int c, bool invert, bool clear_background, image_t *mask)
{
	size_t count = 0;
    if ((0 <= x) && (x < img->w) && (0 <= y) && (y < img->h)) {
        image_t out;
        out.w = img->w;
        out.h = img->h;
        out.pixfmt = PIXFORMAT_BINARY;
        out.data = fb_alloc0(image_size(&out), FB_ALLOC_NO_HINT);

        if (mask) {
            for (int y = 0, yy = out.h; y < yy; y++) {
                uint32_t *row_ptr = IMAGE_COMPUTE_BINARY_PIXEL_ROW_PTR(&out, y);
                for (int x = 0, xx = out.w; x < xx; x++) {
                    if (image_get_mask_pixel(mask, x, y)) IMAGE_SET_BINARY_PIXEL_FAST(row_ptr, x);
                }
            }
        }

        int color_seed_threshold = 0;
        int color_floating_threshold = 0;

        switch(img->pixfmt) {
        //     case PIXFORMAT_BINARY: {
        //         color_seed_threshold = fast_roundf(seed_threshold * COLOR_BINARY_MAX);
        //         color_floating_threshold = fast_roundf(floating_threshold * COLOR_BINARY_MAX);
        //         break;
        //     }
            case PIXFORMAT_GRAYSCALE: {
                color_seed_threshold = fast_roundf(seed_threshold * COLOR_GRAYSCALE_MAX);
                color_floating_threshold = fast_roundf(floating_threshold * COLOR_GRAYSCALE_MAX);
                break;
            }
        //     case PIXFORMAT_RGB565: {
        //         color_seed_threshold = COLOR_R5_G6_B5_TO_RGB565(fast_roundf(seed_threshold * COLOR_R5_MAX),
        //                                                         fast_roundf(seed_threshold * COLOR_G6_MAX),
        //                                                         fast_roundf(seed_threshold * COLOR_B5_MAX));
        //         color_floating_threshold = COLOR_R5_G6_B5_TO_RGB565(fast_roundf(floating_threshold * COLOR_R5_MAX),
        //                                                             fast_roundf(floating_threshold * COLOR_G6_MAX),
        //                                                             fast_roundf(floating_threshold * COLOR_B5_MAX));
        //         break;
        //     }
            default: {
                break;
            }
        }

        count = __imlib_flood_fill_int(&out, img, x, y, color_seed_threshold, color_floating_threshold, NULL, NULL);

        switch(img->bpp) {
        //     case PIXFORMAT_BINARY: {
        //         for (int y = 0, yy = out.h; y < yy; y++) {
        //             uint32_t *row_ptr = IMAGE_COMPUTE_BINARY_PIXEL_ROW_PTR(img, y);
        //             uint32_t *out_row_ptr = IMAGE_COMPUTE_BINARY_PIXEL_ROW_PTR(&out, y);
        //             for (int x = 0, xx = out.w; x < xx; x++) {
        //                 if (IMAGE_GET_BINARY_PIXEL_FAST(out_row_ptr, x) ^ invert) {
        //                     IMAGE_PUT_BINARY_PIXEL_FAST(row_ptr, x, c);
        //                 } else if (clear_background) {
        //                     IMAGE_PUT_BINARY_PIXEL_FAST(row_ptr, x, 0);
        //                 }
        //             }
        //         }
        //         break;
        //     }
            case PIXFORMAT_GRAYSCALE: {
                for (int y = 0, yy = out.h; y < yy; y++) {
                    uint8_t *row_ptr = IMAGE_COMPUTE_GRAYSCALE_PIXEL_ROW_PTR(img, y);
                    uint32_t *out_row_ptr = IMAGE_COMPUTE_BINARY_PIXEL_ROW_PTR(&out, y);
                    for (int x = 0, xx = out.w; x < xx; x++) {
                        if (IMAGE_GET_BINARY_PIXEL_FAST(out_row_ptr, x) ^ invert) {
                            IMAGE_PUT_GRAYSCALE_PIXEL_FAST(row_ptr, x, c);
                        } else if (clear_background) {
                            IMAGE_PUT_GRAYSCALE_PIXEL_FAST(row_ptr, x, 0);
                        }
                    }
                }
                break;
            }
        //     case PIXFORMAT_RGB565: {
        //         for (int y = 0, yy = out.h; y < yy; y++) {
        //             uint16_t *row_ptr = IMAGE_COMPUTE_RGB565_PIXEL_ROW_PTR(img, y);
        //             uint32_t *out_row_ptr = IMAGE_COMPUTE_BINARY_PIXEL_ROW_PTR(&out, y);
        //             for (int x = 0, xx = out.w; x < xx; x++) {
        //                 if (IMAGE_GET_BINARY_PIXEL_FAST(out_row_ptr, x) ^ invert) {
        //                     IMAGE_PUT_RGB565_PIXEL_FAST(row_ptr, x, c);
        //                 } else if (clear_background) {
        //                     IMAGE_PUT_RGB565_PIXEL_FAST(row_ptr, x, 0);
        //                 }
        //             }
        //         }
        //         break;
        //     }
            default: {
                break;
            }
        }

        fb_free(out.data);
    }
	return count;
}
// #endif // IMLIB_ENABLE_FLOOD_FILL



// #ifndef OMV_MINIMUM
//输入灰度图像，找出面积最大的连通域
#define MAX_DOMAIN_CNT 254

static void _get_hv_pixel(image_t* img, uint16_t* h0, uint16_t* h1, \
						uint16_t* h2, uint16_t* v0, uint16_t* v1, uint16_t* v2){
	uint8_t* data = img->pixels;
	uint16_t w = img->w;
	uint16_t h = img->h;

	uint16_t minx=w;
	uint16_t miny=h;
	uint16_t maxx=0;
	uint16_t maxy=0;
	uint16_t _h1=0; uint16_t _h2=0; uint16_t _v1=0; uint16_t _v2=0;

	for (int y = 0, yy = img->h; y < yy; y++) {
		//uint32_t *row_ptr = IMAGE_COMPUTE_BINARY_PIXEL_ROW_PTR(img, y);
		uint32_t *img_row_ptr = IMAGE_COMPUTE_BINARY_PIXEL_ROW_PTR(img, y);
		for (int x = 0, xx = img->w; x < xx; x++) {
			if (IMAGE_GET_BINARY_PIXEL_FAST(img_row_ptr, x) ) {
				if(y<miny) miny=y; else if(y>maxy) maxy=y;
				if(x<minx) minx=x; else if(x>maxx) maxx=x;
				if(y==0) _h1+=1;   else if(y==h-1) _h2+=1;
				if(x==0) _v1+=1;   else if(x==w-1) _v2+=1;
			}
		}
	}

	*h0 = maxx-minx+1;
	*v0 = maxy-miny+1;
	*h1 = _h1; *h2 = _h2;
	*v1 = _v1; *v2 = _v2;

	return;
}



#define EDGE_GATE 0.7
void imlib_find_domain(image_t* img, image_t** dst, float edge_gate)
{
	if(img->pixfmt != PIXFORMAT_GRAYSCALE)
		imlib_printf(0, "only support grayscale pic");

	uint16_t w = img->w;
	uint16_t h = img->h;
	uint8_t* pic = img->pixels;

	uint16_t domian_cnt[MAX_DOMAIN_CNT];
	uint8_t p_x[MAX_DOMAIN_CNT];
	uint8_t p_y[MAX_DOMAIN_CNT];
	for(int i=0; i < MAX_DOMAIN_CNT; i++) {
		domian_cnt[i] = 0;
	}

	uint8_t color_idx = 0;

	for(int y=0; y<h; y++) {
		if(color_idx>=MAX_DOMAIN_CNT)
			break;
		for(int x=0; x<w; x++){
			int oft = x+y*w;
			if(pic[oft] > color_idx){
				domian_cnt[color_idx] = __imlib_flood_fill(img, x, y, 0, 0,color_idx+1, 0, 0, NULL);
				p_x[color_idx] = x;
				p_y[color_idx] = y;
				//printf("(%d, %d) -> %d, count=%d\r\n",x,y,color_idx+1,domian_cnt[color_idx]);
				color_idx += 1;

			}
		}
	}

	int maxcnt = 0;
	int maxidx = -1;
	int max2cnt = 0;
	int max2idx = -1;
	for(int i=0; i< MAX_DOMAIN_CNT; i++){
		if(domian_cnt[i]>maxcnt) {
			max2cnt= maxcnt;
			max2idx= maxidx;
			maxcnt = domian_cnt[i];
			maxidx = i;
		} else if(domian_cnt[i]>max2cnt) {
			max2cnt = domian_cnt[i];
			max2idx = i;
		}
	}
	//printf("max domain idx=%d, cnt=%d\r\n",maxidx, maxcnt);
	if(maxcnt == 0) { //没有连通域
        *dst = NULL;
		return ;
	} else if (max2cnt<w+h) { //只有一块连通域, 或者第二块连通域面积过小
        image_t *out = imlib_image_create(img->w, img->h, PIXFORMAT_BINARY, NULL, NULL, true);

		__imlib_flood_fill_int(out, img, p_x[maxidx], p_y[maxidx], 0, 0, NULL, NULL);
        *dst = out;
		return ;
	} else { //有超过两块较大的连通域
		image_t out0, out1;
		out0.w = out1.w = img->w;
		out0.h = out1.h = img->h;
		out0.pixfmt = out1.pixfmt = PIXFORMAT_BINARY;
		out0.data = fb_alloc0(image_size(&out0), FB_ALLOC_NO_HINT);
		out1.data = fb_alloc0(image_size(&out1), FB_ALLOC_NO_HINT);
		__imlib_flood_fill_int(&out0, img, p_x[maxidx], p_y[maxidx], 0, 0, NULL, NULL);
		__imlib_flood_fill_int(&out1, img, p_x[max2idx], p_y[max2idx], 0, 0, NULL, NULL);
		uint16_t r0_h0,r0_h1,r0_h2; //水平方向像素范围，上边界像素数，下边界像素数
		uint16_t r0_v0,r0_v1,r0_v2; //
		uint16_t r1_h0,r1_h1,r1_h2; //水平方向像素范围，上边界像素数，下边界像素数
		uint16_t r1_v0,r1_v1,r1_v2; //
		_get_hv_pixel(&out0, &r0_h0,&r0_h1,&r0_h2, &r0_v0,&r0_v1,&r0_v2);
		_get_hv_pixel(&out1, &r1_h0,&r1_h1,&r1_h2, &r1_v0,&r1_v1,&r1_v2);
		uint16_t r0_h, r0_v, r1_h, r1_v;
		r0_h = r0_h1>r0_h2 ? r0_h1 : r0_h2;
		r1_h = r1_h1>r1_h2 ? r1_h1 : r1_h2;
		r0_v = r0_v1>r0_v2 ? r0_v1 : r0_v2;
		r1_v = r1_v1>r1_v2 ? r1_v1 : r1_v2;
		float r0_hrate, r0_vrate, r1_hrate, r1_vrate;
		r0_hrate = 1.0*r0_h/r0_h0;	r0_vrate = 1.0*r0_v/r0_v0;
		r1_hrate = 1.0*r1_h/r1_h0;	r1_vrate = 1.0*r1_v/r1_v0;
		// printf("r0cnt=%04d; h0=%03d, h1=%03d, h2=%03d; v0=%03d, v1=%03d, v2=%03d; hrate=%.3f, vrate=%.3f\r\n",
			//  maxcnt, r0_h0, r0_h1, r0_h2, r0_v0, r0_v1, r0_v2, r0_hrate, r0_vrate );
		// printf("r1cnt=%04d; h0=%03d, h1=%03d, h2=%03d; v0=%03d, v1=%03d, v2=%03d; hrate=%.3f, vrate=%.3f\r\n",
			//  max2cnt, r1_h0, r1_h1, r1_h2, r1_v0, r1_v1, r1_v2, r1_hrate, r1_vrate );
		int r_flag = -1;
		if(r0_hrate < edge_gate || r0_vrate < edge_gate) { //r0是线
			r_flag = 0; //使用r0
		} else if(r1_hrate < edge_gate || r1_vrate < edge_gate) { //r0不是线,r1是线
			r_flag = 1;
		} else { //两个都不是线，选面积大的
			r_flag = 0;
		}
		// printf("rflag=%d\r\n\r\n", r_flag);

        image_t *out = imlib_image_create(w, h, PIXFORMAT_BINARY, NULL, NULL, true);
        // image_copy(out, r_flag ? &out1 : &out0);
        memcpy(out->data, r_flag?out1.data:out0.data, image_size(&out0));
		// uint8_t* data = xalloc(image_size(&out0));
		// mp_obj_t image = py_image(w, h, PIXFORMAT_BINARY, data);
		// memcpy(((py_image_obj_t *)image)->_cobj.data, r_flag?out1.data:out0.data, image_size(&out0));
		fb_free(NULL);
		fb_free(NULL);
        *dst = out;
		return ;
	}
}