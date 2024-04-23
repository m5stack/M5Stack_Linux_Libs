#include "imlib.h"


void imlib_pixfmt_to(image_t *dst, image_t *src, rectangle_t *roi_i)
{
    rectangle_t *roi, tmp_roi;
    if(roi_i == NULL){
        roi = &tmp_roi;
        roi->x = 0;
        roi->y = 0;
        roi->w = dst->w;
        roi->h = dst->h;
    }else{
        roi = roi_i;
    }
    switch (src->pixfmt)
    {
    case PIXFORMAT_BINARY:
        switch (dst->pixfmt)
        {
        case PIXFORMAT_BINARY:
        {
            for(int dst_y_index = 0, roi_y_index = roi->y; dst_y_index < dst->h; ++ dst_y_index, ++ roi_y_index){
                uint32_t *src_row_ptr = IMAGE_COMPUTE_BINARY_PIXEL_ROW_PTR(src, roi_y_index);
                uint32_t *dst_row_ptr = IMAGE_COMPUTE_BINARY_PIXEL_ROW_PTR(dst, dst_y_index);
                for(int dst_x_index = 0, roi_x_index = roi->x; dst_x_index < dst->w; ++ dst_x_index, ++ roi_x_index){
                    int pixel = IMAGE_GET_BINARY_PIXEL_FAST(src_row_ptr, roi_x_index);
                    IMAGE_PUT_BINARY_PIXEL_FAST(dst_row_ptr, dst_x_index, pixel);
                }
            }
            break;
        }
        case PIXFORMAT_GRAYSCALE:
            for(int dst_y_index = 0, roi_y_index = roi->y; dst_y_index < dst->h; ++ dst_y_index, ++ roi_y_index){
                uint32_t *src_row_ptr = IMAGE_COMPUTE_BINARY_PIXEL_ROW_PTR(src, roi_y_index);
                uint8_t *dst_row_ptr = IMAGE_COMPUTE_GRAYSCALE_PIXEL_ROW_PTR(dst, dst_y_index);
                for(int dst_x_index = 0, roi_x_index = roi->x; dst_x_index < dst->w; ++ dst_x_index, ++ roi_x_index){
                    int pixel = COLOR_BINARY_TO_GRAYSCALE(IMAGE_GET_BINARY_PIXEL_FAST(src_row_ptr, roi_x_index));
                    IMAGE_PUT_GRAYSCALE_PIXEL_FAST(dst_row_ptr, dst_x_index, pixel);
                }
            }
            break;
        case PIXFORMAT_RGB565:
            for(int dst_y_index = 0, roi_y_index = roi->y; dst_y_index < dst->h; ++ dst_y_index, ++ roi_y_index){
                uint32_t *src_row_ptr = IMAGE_COMPUTE_BINARY_PIXEL_ROW_PTR(src, roi_y_index);
                uint16_t *dst_row_ptr = IMAGE_COMPUTE_RGB565_PIXEL_ROW_PTR(dst, dst_y_index);
                for(int dst_x_index = 0, roi_x_index = roi->x; dst_x_index < dst->w; ++ dst_x_index, ++ roi_x_index){
                    int pixel = COLOR_BINARY_TO_RGB565(IMAGE_GET_BINARY_PIXEL_FAST(src_row_ptr, roi_x_index));
                    IMAGE_PUT_RGB565_PIXEL_FAST(dst_row_ptr, dst_x_index, pixel);
                }
            }
            break;
        case PIXFORMAT_RGB888:
            for(int dst_y_index = 0, roi_y_index = roi->y; dst_y_index < dst->h; ++ dst_y_index, ++ roi_y_index){
                uint32_t *src_row_ptr = IMAGE_COMPUTE_BINARY_PIXEL_ROW_PTR(src, roi_y_index);
                pixel24_t *dst_row_ptr = IMAGE_COMPUTE_RGB888_PIXEL_ROW_PTR(dst, dst_y_index);
                for(int dst_x_index = 0, roi_x_index = roi->x; dst_x_index < dst->w; ++ dst_x_index, ++ roi_x_index){
                    int pixel = COLOR_BINARY_TO_RGB888(IMAGE_GET_BINARY_PIXEL_FAST(src_row_ptr, roi_x_index));
                    IMAGE_PUT_RGB888_PIXEL_FAST(dst_row_ptr, dst_x_index, pixel);
                }
            }
            break;
        default:
            break;
        }
        break;
    case PIXFORMAT_GRAYSCALE:
        switch (dst->pixfmt)
        {
        case PIXFORMAT_BINARY:
            for(int dst_y_index = 0, roi_y_index = roi->y; dst_y_index < dst->h; ++ dst_y_index, ++ roi_y_index){
                uint8_t *src_row_ptr = IMAGE_COMPUTE_GRAYSCALE_PIXEL_ROW_PTR(src, roi_y_index);
                uint32_t *dst_row_ptr = IMAGE_COMPUTE_BINARY_PIXEL_ROW_PTR(dst, dst_y_index);
                for(int dst_x_index = 0, roi_x_index = roi->x; dst_x_index < dst->w; ++ dst_x_index, ++ roi_x_index){
                    int pixel = COLOR_GRAYSCALE_TO_BINARY(IMAGE_GET_GRAYSCALE_PIXEL_FAST(src_row_ptr, roi_x_index));
                    IMAGE_PUT_BINARY_PIXEL_FAST(dst_row_ptr, dst_x_index, pixel);
                }
            }
            break;
        case PIXFORMAT_GRAYSCALE:
            for(int dst_y_index = 0, roi_y_index = roi->y; dst_y_index < dst->h; ++ dst_y_index, ++ roi_y_index){
                uint8_t *src_row_ptr = IMAGE_COMPUTE_GRAYSCALE_PIXEL_ROW_PTR(src, roi_y_index);
                uint8_t *dst_row_ptr = IMAGE_COMPUTE_GRAYSCALE_PIXEL_ROW_PTR(dst, dst_y_index);
                for(int dst_x_index = 0, roi_x_index = roi->x; dst_x_index < dst->w; ++ dst_x_index, ++ roi_x_index){
                    int pixel = IMAGE_GET_GRAYSCALE_PIXEL_FAST(src_row_ptr, roi_x_index);
                    IMAGE_PUT_GRAYSCALE_PIXEL_FAST(dst_row_ptr, dst_x_index, pixel);
                }
            }
            break;
        case PIXFORMAT_RGB565:
            for(int dst_y_index = 0, roi_y_index = roi->y; dst_y_index < dst->h; ++ dst_y_index, ++ roi_y_index){
                uint8_t *src_row_ptr = IMAGE_COMPUTE_GRAYSCALE_PIXEL_ROW_PTR(src, roi_y_index);
                uint16_t *dst_row_ptr = IMAGE_COMPUTE_RGB565_PIXEL_ROW_PTR(dst, dst_y_index);
                for(int dst_x_index = 0, roi_x_index = roi->x; dst_x_index < dst->w; ++ dst_x_index, ++ roi_x_index){
                    int pixel = COLOR_GRAYSCALE_TO_RGB565(IMAGE_GET_GRAYSCALE_PIXEL_FAST(src_row_ptr, roi_x_index));
                    IMAGE_PUT_RGB565_PIXEL_FAST(dst_row_ptr, dst_x_index, pixel);
                }
            }
            break;
        case PIXFORMAT_RGB888:
            for(int dst_y_index = 0, roi_y_index = roi->y; dst_y_index < dst->h; ++ dst_y_index, ++ roi_y_index){
                uint8_t *src_row_ptr = IMAGE_COMPUTE_GRAYSCALE_PIXEL_ROW_PTR(src, roi_y_index);
                pixel24_t *dst_row_ptr = IMAGE_COMPUTE_RGB888_PIXEL_ROW_PTR(dst, dst_y_index);
                for(int dst_x_index = 0, roi_x_index = roi->x; dst_x_index < dst->w; ++ dst_x_index, ++ roi_x_index){
                    int pixel = COLOR_GRAYSCALE_TO_RGB888(IMAGE_GET_GRAYSCALE_PIXEL_FAST(src_row_ptr, roi_x_index));
                    IMAGE_PUT_RGB888_PIXEL_FAST(dst_row_ptr, dst_x_index, pixel);
                }
            }
            break;
        default:
            break;
        }
        break;
    case PIXFORMAT_RGB565:
        switch (dst->pixfmt)
        {
        case PIXFORMAT_BINARY:
            for(int dst_y_index = 0, roi_y_index = roi->y; dst_y_index < dst->h; ++ dst_y_index, ++ roi_y_index){
                uint16_t *src_row_ptr = IMAGE_COMPUTE_RGB565_PIXEL_ROW_PTR(src, roi_y_index);
                uint32_t *dst_row_ptr = IMAGE_COMPUTE_BINARY_PIXEL_ROW_PTR(dst, dst_y_index);
                for(int dst_x_index = 0, roi_x_index = roi->x; dst_x_index < dst->w; ++ dst_x_index, ++ roi_x_index){
                    int pixel = COLOR_RGB565_TO_BINARY(IMAGE_GET_RGB565_PIXEL_FAST(src_row_ptr, roi_x_index));
                    IMAGE_PUT_BINARY_PIXEL_FAST(dst_row_ptr, dst_x_index, pixel);
                }
            }
            break;
        case PIXFORMAT_GRAYSCALE:
            for(int dst_y_index = 0, roi_y_index = roi->y; dst_y_index < dst->h; ++ dst_y_index, ++ roi_y_index){
                uint16_t *src_row_ptr = IMAGE_COMPUTE_RGB565_PIXEL_ROW_PTR(src, roi_y_index);
                uint8_t *dst_row_ptr = IMAGE_COMPUTE_GRAYSCALE_PIXEL_ROW_PTR(dst, dst_y_index);
                for(int dst_x_index = 0, roi_x_index = roi->x; dst_x_index < dst->w; ++ dst_x_index, ++ roi_x_index){
                    int pixel = COLOR_RGB565_TO_GRAYSCALE(IMAGE_GET_RGB565_PIXEL_FAST(src_row_ptr, roi_x_index));
                    IMAGE_PUT_GRAYSCALE_PIXEL_FAST(dst_row_ptr, dst_x_index, pixel);
                }
            }
            break;
        case PIXFORMAT_RGB565:
            for(int dst_y_index = 0, roi_y_index = roi->y; dst_y_index < dst->h; ++ dst_y_index, ++ roi_y_index){
                uint16_t *src_row_ptr = IMAGE_COMPUTE_RGB565_PIXEL_ROW_PTR(src, roi_y_index);
                uint16_t *dst_row_ptr = IMAGE_COMPUTE_RGB565_PIXEL_ROW_PTR(dst, dst_y_index);
                for(int dst_x_index = 0, roi_x_index = roi->x; dst_x_index < dst->w; ++ dst_x_index, ++ roi_x_index){
                    int pixel = IMAGE_GET_RGB565_PIXEL_FAST(src_row_ptr, roi_x_index);
                    IMAGE_PUT_RGB565_PIXEL_FAST(dst_row_ptr, dst_x_index, pixel);
                }
            }
            break;
        case PIXFORMAT_RGB888:
            for(int dst_y_index = 0, roi_y_index = roi->y; dst_y_index < dst->h; ++ dst_y_index, ++ roi_y_index){
                uint16_t *src_row_ptr = IMAGE_COMPUTE_RGB565_PIXEL_ROW_PTR(src, roi_y_index);
                pixel24_t *dst_row_ptr = IMAGE_COMPUTE_RGB888_PIXEL_ROW_PTR(dst, dst_y_index);
                for(int dst_x_index = 0, roi_x_index = roi->x; dst_x_index < dst->w; ++ dst_x_index, ++ roi_x_index){
                    int pixel = IMAGE_GET_RGB565_PIXEL_FAST(src_row_ptr, roi_x_index);
                    pixel24_t pixel_out = {.red = COLOR_RGB565_TO_R8(pixel), .green = COLOR_RGB565_TO_G8(pixel), .blue = COLOR_RGB565_TO_B8(pixel) };
                    IMAGE_PUT_RGB888_PIXEL_FAST_(dst_row_ptr, dst_x_index, pixel_out);
                }
            }
            break;
        default:
            break;
        }
        break;
    case PIXFORMAT_RGB888:
        switch (dst->pixfmt)
        {
        case PIXFORMAT_BINARY:
            for(int dst_y_index = 0, roi_y_index = roi->y; dst_y_index < dst->h; ++ dst_y_index, ++ roi_y_index){
                pixel24_t *src_row_ptr = IMAGE_COMPUTE_RGB888_PIXEL_ROW_PTR(src, roi_y_index);
                uint32_t *dst_row_ptr = IMAGE_COMPUTE_BINARY_PIXEL_ROW_PTR(dst, dst_y_index);
                for(int dst_x_index = 0, roi_x_index = roi->x; dst_x_index < dst->w; ++ dst_x_index, ++ roi_x_index){
                    int pixel = COLOR_RGB888_TO_BINARY(IMAGE_GET_RGB888_PIXEL_FAST(src_row_ptr, roi_x_index));
                    IMAGE_PUT_BINARY_PIXEL_FAST(dst_row_ptr, dst_x_index, pixel);
                }
            }
            break;
        case PIXFORMAT_GRAYSCALE:
            for(int dst_y_index = 0, roi_y_index = roi->y; dst_y_index < dst->h; ++ dst_y_index, ++ roi_y_index){
                pixel24_t *src_row_ptr = IMAGE_COMPUTE_RGB888_PIXEL_ROW_PTR(src, roi_y_index);
                uint8_t *dst_row_ptr = IMAGE_COMPUTE_GRAYSCALE_PIXEL_ROW_PTR(dst, dst_y_index);
                for(int dst_x_index = 0, roi_x_index = roi->x; dst_x_index < dst->w; ++ dst_x_index, ++ roi_x_index){
                    pixel24_t s_pixel = IMAGE_GET_RGB888_PIXEL_FAST_(src_row_ptr, roi_x_index);
                    int pixel = COLOR_RGB888_TO_Y_(s_pixel.red, s_pixel.green, s_pixel.blue);
                    IMAGE_PUT_GRAYSCALE_PIXEL_FAST(dst_row_ptr, dst_x_index, pixel);
                }
            }
            break;
        case PIXFORMAT_RGB565:
            for(int dst_y_index = 0, roi_y_index = roi->y; dst_y_index < dst->h; ++ dst_y_index, ++ roi_y_index){
                pixel24_t *src_row_ptr = IMAGE_COMPUTE_RGB888_PIXEL_ROW_PTR(src, roi_y_index);
                uint16_t *dst_row_ptr = IMAGE_COMPUTE_RGB565_PIXEL_ROW_PTR(dst, dst_y_index);
                for(int dst_x_index = 0, roi_x_index = roi->x; dst_x_index < dst->w; ++ dst_x_index, ++ roi_x_index){
                    pixel24_t src_pixel = IMAGE_GET_RGB888_PIXEL_FAST_(src_row_ptr, roi_x_index);
                    int pixel = COLOR_R8_G8_B8_TO_RGB565(src_pixel.red, src_pixel.green, src_pixel.blue);
                    IMAGE_PUT_RGB565_PIXEL_FAST(dst_row_ptr, dst_x_index, pixel);
                }
            }
            break;
        case PIXFORMAT_RGB888:
            for(int dst_y_index = 0, roi_y_index = roi->y; dst_y_index < dst->h; ++ dst_y_index, ++ roi_y_index){
                pixel24_t *src_row_ptr = IMAGE_COMPUTE_RGB888_PIXEL_ROW_PTR(src, roi_y_index);
                pixel24_t *dst_row_ptr = IMAGE_COMPUTE_RGB888_PIXEL_ROW_PTR(dst, dst_y_index);
                for(int dst_x_index = 0, roi_x_index = roi->x; dst_x_index < dst->w; ++ dst_x_index, ++ roi_x_index){
                    pixel24_t pixel = IMAGE_GET_RGB888_PIXEL_FAST_(src_row_ptr, roi_x_index);
                    IMAGE_PUT_RGB888_PIXEL_FAST_(dst_row_ptr, dst_x_index, pixel);
                }
            }
            break;
        default:
            break;
        }
        break;
    default:
        break;
    }
}