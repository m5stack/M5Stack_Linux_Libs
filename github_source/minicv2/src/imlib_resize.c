// #include <stdint.h>
// // *********************************************************************
// // * 函数名称: BspDouToFix
// // * 功能描述: 将指定的浮点数 转化为 定点数
// // * 算法描述: 无
// // * 输入参数:  ucType 0表示无符号 1表示有符号
// // *           ucInteger 表示整数占几个bit
// // *           ucdecimal 表示小数占几个bit
// // *           dbDou 为待转化的浮点数
// // * 输出参数: 无
// // * 返 回 值: 转化后的定点数
// // *********************************************************************

// #define VOID void
// #define UCHAR int
// #define DOUBLE double
// #define UINT64 uint64_t
 
// VOID BspDouToFix(UCHAR ucType, UCHAR ucInteger, UCHAR ucdecimal, DOUBLE dbDou, UINT64 *pllfix)
// {
//     UINT64 lltemp = 0;
//     DOUBLE dbtemp = 0;
     
//     dbtemp = dbDou;
//     if(dbtemp < 0) /* 有符号正数 或者 无符号数 */
//     {
//         lltemp = (UINT64)(-dbDou*(1<<ucdecimal));
//         *pllfix = (UINT64)((UINT64)(1)<<(ucType + ucInteger + ucdecimal)) -    lltemp;    
//     }
//     else if(dbtemp > 0)   /* 有符号负数 */
//         {
//             *pllfix = (UINT64)(dbDou * (1<<ucdecimal));
//         }
//         else
//         {
//             *pllfix = 0;
//         }
// }

#include "imlib.h"


float F_BiCubic(float x){
	float w = abs(x);
	if(w < 1){
		return (1 - 2 * (w * w)  +  (w * w * w));
	}
	else if(w < 2){
		return (4 - 8 * w + 5 * (w * w) - (w * w * w));
	}
	else{
		return 0;
	}
}
/**
 * @brief 
 * 
 * @param src 
 * @param dst 
 * @param hist 
 * 双线型插值算法
 * https://blog.csdn.net/qq_37577735/article/details/80041586
 */
void imlib_image_resize(image_t *dst, image_t *src, int hist)
{
	#define GET_SITE_PIXEL_FAST(index_n, scale) \
	({ \
		__typeof__ (index_n) _index_n = (index_n); \
		__typeof__ (scale) _scale = (scale); \
		fast_roundf(_index_n * _scale);\
	})

	int dst_x_index = 0, dst_y_index = 0;
	int src_x_index = 0, src_y_index = 0;
	float h_scale = 1.f * src->h / dst->h;
	float w_scale = 1.f * src->w / dst->w;

	if (hist & IMAGE_HINT_AREA)	//临近插值
	{
		switch (src->pixfmt)
		{
		case PIXFORMAT_BINARY:{
			uint32_t *dst32, *src32;
			for (dst_y_index = 0; dst_y_index < dst->h; dst_y_index ++)
			{
				dst32 = IMAGE_COMPUTE_BINARY_PIXEL_ROW_PTR(dst, dst_y_index);
				src_y_index = GET_SITE_PIXEL_FAST(dst_y_index, h_scale);
				src32 = IMAGE_COMPUTE_BINARY_PIXEL_ROW_PTR(src, src_y_index);
				for (dst_x_index = 0; dst_x_index < dst->w; dst_x_index ++)
				{
					src_x_index = GET_SITE_PIXEL_FAST(dst_x_index, w_scale);
					IMAGE_PUT_BINARY_PIXEL_FAST(dst32, dst_x_index, IMAGE_GET_BINARY_PIXEL_FAST(src32, src_x_index));
				}
			}
			break;
		}
		case PIXFORMAT_GRAYSCALE:{
			uint8_t *dst8, *src8;
			for (dst_y_index = 0; dst_y_index < dst->h; dst_y_index ++)
			{
				dst8 = IMAGE_COMPUTE_GRAYSCALE_PIXEL_ROW_PTR(dst, dst_y_index);
				src_y_index = GET_SITE_PIXEL_FAST(dst_y_index, h_scale);
				src8 = IMAGE_COMPUTE_GRAYSCALE_PIXEL_ROW_PTR(src, src_y_index);
				for (dst_x_index = 0; dst_x_index < dst->w; dst_x_index ++)
				{
					int src_x_index = GET_SITE_PIXEL_FAST(dst_x_index, w_scale);
					IMAGE_PUT_GRAYSCALE_PIXEL_FAST(dst8, dst_x_index, IMAGE_GET_GRAYSCALE_PIXEL_FAST(src8, src_x_index));
				}
			}
			break;
		}
		case PIXFORMAT_RGB565:{
			uint16_t *dst16, *src16;
			for (dst_y_index = 0; dst_y_index < dst->h; dst_y_index ++)
			{
				dst16 = IMAGE_COMPUTE_RGB565_PIXEL_ROW_PTR(dst, dst_y_index);
				src_y_index = GET_SITE_PIXEL_FAST(dst_y_index, h_scale);
				src16 = IMAGE_COMPUTE_RGB565_PIXEL_ROW_PTR(src, src_y_index);
				for (dst_x_index = 0; dst_x_index < dst->w; dst_x_index ++)
				{
					src_x_index = GET_SITE_PIXEL_FAST(dst_x_index, w_scale);
					IMAGE_PUT_RGB565_PIXEL_FAST(dst16, dst_x_index, IMAGE_GET_RGB565_PIXEL_FAST(src16, src_x_index));
				}
			}
			break;
		}
		case PIXFORMAT_RGB888:{
			pixel24_t *dst24, *src24;
			for (dst_y_index = 0; dst_y_index < dst->h; dst_y_index ++)
			{
				dst24 = IMAGE_COMPUTE_RGB888_PIXEL_ROW_PTR(dst, dst_y_index);
				src_y_index = GET_SITE_PIXEL_FAST(dst_y_index, h_scale);
				src24 = IMAGE_COMPUTE_RGB888_PIXEL_ROW_PTR(src, src_y_index);
				for (dst_x_index = 0; dst_x_index < dst->w; dst_x_index ++)
				{
					src_x_index = GET_SITE_PIXEL_FAST(dst_x_index, w_scale);
					IMAGE_PUT_RGB888_PIXEL_FAST_(dst24, dst_x_index, IMAGE_GET_RGB888_PIXEL_FAST_(src24, src_x_index));
				}
			}
			break;
		}
		default:
		{
			return;
		}
		}
	}else if(hist & IMAGE_HINT_BILINEAR){ //双线性缩放
        // +---+---+
        // | C | x |
        // +---+---+
        // | x | x |
        // +---+---+
		switch (src->pixfmt){
			case PIXFORMAT_BINARY:{
				//For binary images, linear interpolation does not make much sense
				int src_x_index, src_y_index;
				uint32_t *dst32, *src32;
				for (dst_y_index = 0; dst_y_index < dst->h; dst_y_index ++){
					dst32 = IMAGE_COMPUTE_BINARY_PIXEL_ROW_PTR(dst, dst_y_index);
					src_y_index = GET_SITE_PIXEL_FAST(dst_y_index, h_scale);
					src32 = IMAGE_COMPUTE_BINARY_PIXEL_ROW_PTR(src, src_y_index);
					for (dst_x_index = 0; dst_x_index < dst->w; dst_x_index ++){
						src_x_index = GET_SITE_PIXEL_FAST(dst_x_index, w_scale);
						IMAGE_PUT_BINARY_PIXEL_FAST(dst32, dst_x_index, IMAGE_GET_BINARY_PIXEL_FAST(src32, src_x_index));
					}
				}
				break;
			}
			case PIXFORMAT_GRAYSCALE:{
				uint8_t *src_row_ptr_0, *src_row_ptr_1;
				int w_limit = src->w - 2;
				int h_limit = src->h - 2;
				for (dst_y_index = 0; dst_y_index < dst->h; ++ dst_y_index){
					uint8_t *dst_row_ptr = IMAGE_COMPUTE_GRAYSCALE_PIXEL_ROW_PTR(dst, dst_y_index);
					float f_src_y_index = (float)((dst_y_index + 0.5) * h_scale - 0.5);
					src_y_index = fast_floorf(f_src_y_index);
					f_src_y_index = f_src_y_index - src_y_index;
					if(src_y_index <= 0){
						src_row_ptr_0 = src_row_ptr_1 = IMAGE_COMPUTE_GRAYSCALE_PIXEL_ROW_PTR(src, 0);
					}
					else if(src_y_index >= h_limit){
						src_row_ptr_0 = src_row_ptr_1 = IMAGE_COMPUTE_GRAYSCALE_PIXEL_ROW_PTR(src, src_y_index);
					}
					else{
                        int src_y_index_p_1 = src_y_index + 1;
                        src_row_ptr_0 = IMAGE_COMPUTE_GRAYSCALE_PIXEL_ROW_PTR(src, src_y_index);
                        src_row_ptr_1 = IMAGE_COMPUTE_GRAYSCALE_PIXEL_ROW_PTR(src, src_y_index_p_1);
					}
					short cbufy[2];									
					cbufy[0] = IM_MAX(IM_MIN((1.f - f_src_y_index) * 2048, 2048), 0);	
					cbufy[1] = 2048 - cbufy[0];								
					for (dst_x_index = 0; dst_x_index < dst->w; ++ dst_x_index){
						float f_src_x_index = (float)((dst_x_index + 0.5) * w_scale - 0.5);
						src_x_index = fast_floorf(f_src_x_index);
						f_src_x_index = f_src_x_index - src_x_index;
						short cbufx[2];									
						cbufx[0] = IM_MAX(IM_MIN((1.f - f_src_x_index) * 2048, 2048), 0);	
						cbufx[1] = 2048 - cbufx[0];	
						int pixel_00, pixel_10, pixel_01, pixel_11;
						if (src_x_index < 0) {
							pixel_00 = pixel_10 = src_row_ptr_0[0];
							pixel_01 = pixel_11 = src_row_ptr_1[0];
						} else if (src_x_index >= w_limit) {
							pixel_00 = pixel_10 = src_row_ptr_0[src_x_index];
							pixel_01 = pixel_11 = src_row_ptr_1[src_x_index];
						} else { // get 4 neighboring pixels
							int src_x_index_p_1 = src_x_index + 1;
							pixel_00 = src_row_ptr_0[src_x_index]; pixel_10 = src_row_ptr_0[src_x_index_p_1];
							pixel_01 = src_row_ptr_1[src_x_index]; pixel_11 = src_row_ptr_1[src_x_index_p_1];
						}
						int pixel = (pixel_00 * cbufx[0] * cbufy[0] +
									 pixel_01 * cbufx[0] * cbufy[1] + 
									 pixel_10 * cbufx[1] * cbufy[0] + 
									 pixel_11 * cbufx[1] * cbufy[1]) >> 22;
						IMAGE_PUT_GRAYSCALE_PIXEL_FAST(dst_row_ptr, dst_x_index, pixel);
					}
				}
				break;
			}
			case PIXFORMAT_RGB565:{
				uint16_t *src_row_ptr_0, *src_row_ptr_1;
				int w_limit = src->w - 2;
				int h_limit = src->h - 2;
				for (dst_y_index = 0; dst_y_index < dst->h; ++ dst_y_index){
					uint16_t *dst_row_ptr = IMAGE_COMPUTE_RGB565_PIXEL_ROW_PTR(dst, dst_y_index);
					float f_src_y_index = (float)((dst_y_index + 0.5) * h_scale - 0.5);
					src_y_index = fast_floorf(f_src_y_index);
					f_src_y_index = f_src_y_index - src_y_index;
					if(src_y_index <= 0){
						src_row_ptr_0 = src_row_ptr_1 = IMAGE_COMPUTE_RGB565_PIXEL_ROW_PTR(src, 0);
					}
					else if(src_y_index >= h_limit){
						src_row_ptr_0 = src_row_ptr_1 = IMAGE_COMPUTE_RGB565_PIXEL_ROW_PTR(src, src_y_index);
					}
					else{
                        int src_y_index_p_1 = src_y_index + 1;
                        src_row_ptr_0 = IMAGE_COMPUTE_RGB565_PIXEL_ROW_PTR(src, src_y_index);
                        src_row_ptr_1 = IMAGE_COMPUTE_RGB565_PIXEL_ROW_PTR(src, src_y_index_p_1);
					}
					short cbufy[2];									
					cbufy[0] = IM_MAX(IM_MIN((1.f - f_src_y_index) * 2048, 2048), 0);	
					cbufy[1] = 2048 - cbufy[0];	
					for (dst_x_index = 0; dst_x_index < dst->w; ++ dst_x_index){
						float f_src_x_index = (float)((dst_x_index + 0.5) * w_scale - 0.5);
						src_x_index = fast_floorf(f_src_x_index);
						f_src_x_index = f_src_x_index - src_x_index;
						short cbufx[2];									
						cbufx[0] = IM_MAX(IM_MIN((1.f - f_src_x_index) * 2048, 2048), 0);	
						cbufx[1] = 2048 - cbufx[0];	

						uint16_t pixel_00, pixel_10, pixel_01, pixel_11;
						
						if (src_x_index <= 0) {
							pixel_00 = pixel_10 = src_row_ptr_0[0];
							pixel_01 = pixel_11 = src_row_ptr_1[0];
						} else if (src_x_index >= w_limit) {
							pixel_00 = pixel_10 = src_row_ptr_0[src_x_index];
							pixel_01 = pixel_11 = src_row_ptr_1[src_x_index];
						} else { // get 4 neighboring pixels
							int src_x_index_p_1 = src_x_index + 1;
							pixel_00 = src_row_ptr_0[src_x_index]; pixel_10 = src_row_ptr_0[src_x_index_p_1];
							pixel_01 = src_row_ptr_1[src_x_index]; pixel_11 = src_row_ptr_1[src_x_index_p_1];
						}
						int pixel_r = (COLOR_RGB565_TO_R8(pixel_00) * cbufx[0] * cbufy[0] +
									   COLOR_RGB565_TO_R8(pixel_01) * cbufx[0] * cbufy[1] + 
									   COLOR_RGB565_TO_R8(pixel_10) * cbufx[1] * cbufy[0] + 
									   COLOR_RGB565_TO_R8(pixel_11) * cbufx[1] * cbufy[1]) >> 22;
						int pixel_g = (COLOR_RGB565_TO_G8(pixel_00) * cbufx[0] * cbufy[0] +
									   COLOR_RGB565_TO_G8(pixel_01) * cbufx[0] * cbufy[1] + 
									   COLOR_RGB565_TO_G8(pixel_10) * cbufx[1] * cbufy[0] + 
									   COLOR_RGB565_TO_G8(pixel_11) * cbufx[1] * cbufy[1]) >> 22;
						int pixel_b = (COLOR_RGB565_TO_B8(pixel_00) * cbufx[0] * cbufy[0] +
									   COLOR_RGB565_TO_B8(pixel_01) * cbufx[0] * cbufy[1] + 
									   COLOR_RGB565_TO_B8(pixel_10) * cbufx[1] * cbufy[0] + 
									   COLOR_RGB565_TO_B8(pixel_11) * cbufx[1] * cbufy[1]) >> 22;
						IMAGE_PUT_RGB565_PIXEL_FAST(dst_row_ptr, dst_x_index, COLOR_R8_G8_B8_TO_RGB565(pixel_r, pixel_g, pixel_b));
					}
				}
				break;
			}
			case PIXFORMAT_RGB888:{
				pixel24_t *src_row_ptr_0, *src_row_ptr_1;
				int w_limit = src->w - 2;
				int h_limit = src->h - 2;
				for (dst_y_index = 0; dst_y_index < dst->h; ++ dst_y_index){
					pixel24_t *dst_row_ptr = IMAGE_COMPUTE_RGB888_PIXEL_ROW_PTR(dst, dst_y_index);
					float f_src_y_index = (float)((dst_y_index + 0.5) * h_scale - 0.5);
					src_y_index = fast_floorf(f_src_y_index);
					f_src_y_index = f_src_y_index - src_y_index;
					if(src_y_index <= 0){
						src_row_ptr_0 = src_row_ptr_1 = IMAGE_COMPUTE_RGB888_PIXEL_ROW_PTR(src, 0);
					}
					else if(src_y_index >= h_limit){
						src_row_ptr_0 = src_row_ptr_1 = IMAGE_COMPUTE_RGB888_PIXEL_ROW_PTR(src, src_y_index);
					}
					else{
                        int src_y_index_p_1 = src_y_index + 1;
                        src_row_ptr_0 = IMAGE_COMPUTE_RGB888_PIXEL_ROW_PTR(src, src_y_index);
                        src_row_ptr_1 = IMAGE_COMPUTE_RGB888_PIXEL_ROW_PTR(src, src_y_index_p_1);
					}
					short cbufy[2];									
					cbufy[0] = IM_MAX(IM_MIN((1.f - f_src_y_index) * 2048, 2048), 0);	
					cbufy[1] = 2048 - cbufy[0];								
					for (dst_x_index = 0; dst_x_index < dst->w; ++ dst_x_index){
						float f_src_x_index = (float)((dst_x_index + 0.5) * w_scale - 0.5);
						src_x_index = fast_floorf(f_src_x_index);
						f_src_x_index = f_src_x_index - src_x_index;
						short cbufx[2];									
						cbufx[0] = IM_MAX(IM_MIN((1.f - f_src_x_index) * 2048, 2048), 0);	
						cbufx[1] = 2048 - cbufx[0];	
						pixel24_t pixel_00, pixel_10, pixel_01, pixel_11;
						if (src_x_index <= 0) {
							pixel_00 = pixel_10 = src_row_ptr_0[0];
							pixel_01 = pixel_11 = src_row_ptr_1[0];
						} else if (src_x_index >= w_limit) {
							pixel_00 = pixel_10 = src_row_ptr_0[src_x_index];
							pixel_01 = pixel_11 = src_row_ptr_1[src_x_index];
						} else { // get 4 neighboring pixels
							int src_x_index_p_1 = src_x_index + 1;
							pixel_00 = src_row_ptr_0[src_x_index]; pixel_10 = src_row_ptr_0[src_x_index_p_1];
							pixel_01 = src_row_ptr_1[src_x_index]; pixel_11 = src_row_ptr_1[src_x_index_p_1];
						}
						int pixel_r = (pixel_00.red * cbufx[0] * cbufy[0] +
									   pixel_01.red * cbufx[0] * cbufy[1] + 
									   pixel_10.red * cbufx[1] * cbufy[0] + 
									   pixel_11.red * cbufx[1] * cbufy[1]) >> 22;
						int pixel_g = (pixel_00.green * cbufx[0] * cbufy[0] +
									   pixel_01.green * cbufx[0] * cbufy[1] + 
									   pixel_10.green * cbufx[1] * cbufy[0] + 
									   pixel_11.green * cbufx[1] * cbufy[1]) >> 22;
						int pixel_b = (pixel_00.blue * cbufx[0] * cbufy[0] +
									   pixel_01.blue * cbufx[0] * cbufy[1] + 
									   pixel_10.blue * cbufx[1] * cbufy[0] + 
									   pixel_11.blue * cbufx[1] * cbufy[1]) >> 22;
						IMAGE_PUT_RGB888_PIXEL_FAST(dst_row_ptr, dst_x_index, COLOR_R8_G8_B8_TO_RGB888(pixel_r, pixel_g, pixel_b));
					}
				}
				break;
			}
			default:{
				return;
			}
		}

	}else if(hist & IMAGE_HINT_BICUBIC){  //三线性缩放
        // +---+---+---+---+
        // | x | x | x | x |
        // +---+---+---+---+
        // | x | C | x | x |
        // +---+---+---+---+
        // | x | x | x | x |
        // +---+---+---+---+
        // | x | x | x | x |
        // +---+---+---+---+
		//目前忽略这个实现
		switch (src->pixfmt)
		{
			case PIXFORMAT_BINARY:{
				int src_x_index, src_y_index;
				uint32_t *dst32, *src32;
				for (dst_y_index = 0; dst_y_index < dst->h; dst_y_index ++)
				{
					dst32 = IMAGE_COMPUTE_BINARY_PIXEL_ROW_PTR(dst, dst_y_index);
					src_y_index = GET_SITE_PIXEL_FAST(dst_y_index, h_scale);
					src32 = IMAGE_COMPUTE_BINARY_PIXEL_ROW_PTR(src, src_y_index);
					for (dst_x_index = 0; dst_x_index < dst->w; dst_x_index ++)
					{
						src_x_index = GET_SITE_PIXEL_FAST(dst_x_index, w_scale);
						IMAGE_PUT_BINARY_PIXEL_FAST(dst32, dst_x_index, IMAGE_GET_BINARY_PIXEL_FAST(src32, src_x_index));
					}
				}
				break;
			}
			case PIXFORMAT_GRAYSCALE:{


				break;
			}
			case PIXFORMAT_RGB565:{


				break;
			}
			case PIXFORMAT_RGB888:{


				break;
			}
		
		default:
			break;
		}
	}

#undef GET_SITE_PIXEL_FAST
}

/*
//https://www.cnblogs.com/GoldBeetle/archive/2018/09/17/9662871.html
int is_in_array(short x, short y, short height, short width)
{
    if (x >= 0 && x < width && y >= 0 && y < height)
        return 1;
    else
        return 0;
}

void bilinera_interpolation(short** in_array, short height, short width, 
                            short** out_array, short out_height, short out_width)
{
    double h_times = (double)out_height / (double)height,
           w_times = (double)out_width / (double)width;
    short  x1, y1, x2, y2, f11, f12, f21, f22;
    double x, y;

    for (int i = 0; i < out_height; i++){
        for (int j = 0; j < out_width; j++){
            x = j / w_times;
            y = i / h_times;
            x1 = (short)(x - 1);
            x2 = (short)(x + 1);
            y1 = (short)(y + 1);
            y2 = (short)(y - 1);
            f11 = is_in_array(x1, y1, height, width) ? in_array[y1][x1] : 0;
            f12 = is_in_array(x1, y2, height, width) ? in_array[y2][x1] : 0;
            f21 = is_in_array(x2, y1, height, width) ? in_array[y1][x2] : 0;
            f22 = is_in_array(x2, y2, height, width) ? in_array[y2][x2] : 0;
            out_array[i][j] = (short)(((f11 * (x2 - x) * (y2 - y)) +
                                       (f21 * (x - x1) * (y2 - y)) +
                                       (f12 * (x2 - x) * (y - y1)) +
                                       (f22 * (x - x1) * (y - y1))) / ((x2 - x1) * (y2 - y1)));
        }
    }
}





void TruncToInt32 (int &ival, float fval)
{
    undefined ival = *(int *)&fval;

// 提取尾数
// 注意实际的尾数前面还有一个被省略掉的1
int mantissa = (ival & 0x07fffff) | 0x800000;

// 提取指数
// 以23分界，指数大于23则左移，否则右移
// 由于指数用偏移表示，所以23+127=150
int exponent = 150 - ((ival >> 23) & 0xff);

if (exponent < 0)
ival = (mantissa << -exponent);
else
ival = (mantissa >> exponent);

// 如果小于0，则将结果取反
if ((*(int *)&fval) & 0x80000000)
ival = -ival;
}


#include<opencv2\imgproc\imgproc.hpp>
#include<opencv2\highgui\highgui.hpp>
#include<opencv2\core\core.hpp>
#include<iostream>
#include<cmath>
using namespace std;
using namespace cv;
float BiCubicPloy(float x);
//BiCubic基函数
 
float BiCubicPloy(float x)
{
	float abs_x = abs(x);//取x的绝对值
	float a = -0.5;
	if (abs_x <= 1.0)
	{
		return (a + 2)*pow(abs_x, 3) - (a + 3)*pow(abs_x, 2) + 1;
	}
	else if (abs_x < 2.0)
	{
		return a*pow(abs_x, 3) - 5 * a*pow(abs_x, 2) + 8 * a*abs_x - 4 * a;
	}
	else
		return 0.0;
}
 
Mat BiCubicInter(Mat &srcImage, double kx, double ky)
{
	//获取输出图像的分辨率
	int nRows = cvRound(srcImage.rows*kx);
	int nCols = cvRound(srcImage.cols*ky);
	Mat resultImage(nRows, nCols, srcImage.type());
	for (int i = 0; i < nRows; i++)
	{
		for (int j = 0; j < nCols; j++)
		{
			//获取目标图像(i,j)在原图中的坐标
			int xm = i / kx;
			int ym = j / ky;
 
			//取出映射到原图的整数部分
			int xi = (int)xm;
			int yi = (int)ym;
 
			//取出映射到原图中的点的四周的16个点的坐标
			int x0 = xi - 1;
			int y0 = yi - 1;
			int x1 = xi;
			int y1 = yi ;
			int x2 = xi + 1;
			int y2 = yi + 1;
			int x3 = xi + 2;
			int y3 = yi + 2;
			if ((x0 >= 0) && (x3 < srcImage.rows) && (y0 >= 0) && (y3 < srcImage.cols))
			{
				//求出行和列所对应的系数
				float dist_x0 = BiCubicPloy(xm - x0);
				float dist_x1 = BiCubicPloy(xm - x1);
				float dist_x2 = BiCubicPloy(xm - x2);
				float dist_x3 = BiCubicPloy(xm - x3);
				float dist_y0 = BiCubicPloy(ym - y0);
				float dist_y1 = BiCubicPloy(ym - y1);
				float dist_y2 = BiCubicPloy(ym - y2);
				float dist_y3 = BiCubicPloy(ym - y3);
 
				//k_i_j=k_i*k_j
				float dist_x0y0 = dist_x0 * dist_y0;
				float dist_x0y1 = dist_x0 * dist_y1;
				float dist_x0y2 = dist_x0 * dist_y2;
				float dist_x0y3 = dist_x0 * dist_y3;
				float dist_x1y0 = dist_x1 * dist_y0;
				float dist_x1y1 = dist_x1 * dist_y1;
				float dist_x1y2 = dist_x1 * dist_y2;
				float dist_x1y3 = dist_x1 * dist_y3;
				float dist_x2y0 = dist_x2 * dist_y0;
				float dist_x2y1 = dist_x2 * dist_y1;
				float dist_x2y2 = dist_x2 * dist_y2;
				float dist_x2y3 = dist_x2 * dist_y3;
				float dist_x3y0 = dist_x3 * dist_y0;
				float dist_x3y1 = dist_x3 * dist_y1;
				float dist_x3y2 = dist_x3 * dist_y2;
				float dist_x3y3 = dist_x3 * dist_y3;
 
				resultImage.at<Vec3b>(i, j) = (srcImage.at<Vec3b>(x0, y0)*dist_x0y0 +
												srcImage.at<Vec3b>(x0, y1)*dist_x0y1 +
												srcImage.at<Vec3b>(x0, y2)*dist_x0y2 +
												srcImage.at<Vec3b>(x0, y3)*dist_x0y3 +
												srcImage.at<Vec3b>(x1, y0)*dist_x1y0 +
												srcImage.at<Vec3b>(x1, y1)*dist_x1y1 +
												srcImage.at<Vec3b>(x1, y2)*dist_x1y2 +
												srcImage.at<Vec3b>(x1, y3)*dist_x1y3 +
												srcImage.at<Vec3b>(x2, y0)*dist_x2y0 +
												srcImage.at<Vec3b>(x2, y1)*dist_x2y1 +
												srcImage.at<Vec3b>(x2, y2)*dist_x2y2 +
												srcImage.at<Vec3b>(x2, y3)*dist_x2y3 +
												srcImage.at<Vec3b>(x3, y0)*dist_x3y0 +
												srcImage.at<Vec3b>(x3, y1)*dist_x3y1 +
												srcImage.at<Vec3b>(x3, y2)*dist_x3y2 +
												srcImage.at<Vec3b>(x3, y3)*dist_x3y3  );
			}
		}
	}
	return resultImage;
}
int main()
{
	Mat srcImage = imread("lakeWater.jpg");
	if (!srcImage.data)
	{
		printf("image could not load...\n");
		return -1;
	}
	imshow("srcImage", srcImage);
	Mat resultImage = BiCubicInter(srcImage, 0.5, 0.5);
	imshow("resultImage", resultImage);
	waitKey(0);
	return 0;
}




typedef unsigned char  BYTE;
typedef unsigned short WORD;
typedef unsigned long  DWORD;
typedef long LONG;

typedef struct tagBITMAPFILEHEADER{
	WORD   bfType;          //must be 0x4D42
	WORD   bfSizeLow;       //the size of the whole bitmap file,low 16bits.
    WORD   bfSizeHigh;      //the size of the whole bitmap file,high 16bits.
	WORD   bfReserved1;     //reserved
	WORD   bfReserved2;     //reserved
	WORD   bfOffBitsLow;    //the sum bits of BITMAPFILEHEADER,BITMAPINFOHEADER and RGBQUAD;the index byte of the image data,low 16bits.
    WORD   bfOffBitsHigh;   //the sum bits of BITMAPFILEHEADER,BITMAPINFOHEADER and RGBQUAD;the index byte of the image data,high 16bits.
}BITMAPFILEHEADER;

typedef struct tagBITMAPINFOHEADER {
    DWORD      biSize;          //the size of this struct.it is 40 bytes.
    LONG       biWidth;         //the width of image data. the unit is pixel.
    LONG       biHeight;        //the height of image data. the unit is pixel.
    WORD       biPlanes;        //must be 1.
    WORD       biBitCount;      //the bit count of each pixel.usually be 1,4,8,or 24.
    DWORD      biCompression;   //is this image compressed.0 indicates no compression.
    DWORD      biSizeImage;     //the size of image data, the unit is byte.
    LONG       biXPelsPerMeter; //resolution in X direction.
    LONG       biYPelsPerMeter; //resolution in Y direction.
    DWORD      biClrUsed;       //the number of colors.0 represents the default value.(2^).
    DWORD      biClrImportant;  //the number of important colors.0 means all important colors.
} BITMAPINFOHEADER;

#include <stdio.h>
#include <stdlib.h>
#include <malloc.h>
#include <string.h>

#include "bitmapfile.h"

#define DST_WIDTH 3840   //width of the destination image
#define DST_HEIGHT 2160  //height of the destination image

#define WIDTHBYTE(bits) ((bits+31)/32*4)

void NearestInterpolation(double dSrcX,double dSrcY,int iDesX,int iDesY,int iSrcWidth,int iSrcHeight,BYTE* pSrcMemData,BYTE* pDesMemData,int iSrcMemWidth,int iDesMemWidth);

void main()
{
	BITMAPFILEHEADER SrcFileHead, DesFileHead;
	BITMAPINFOHEADER SrcInfoHead, DesInfoHead;

	FILE* pSrcFile=0;
	FILE* pDesFile=0;

	//open source image file
	errno_t SrcFileOpenError;
	SrcFileOpenError = fopen_s(&pSrcFile,"../SrcPic.bmp","rb");
	if (SrcFileOpenError != 0)
	{
		printf("fail to open source image file");
		return;
	}

	//read bit map file header
	fread(&SrcFileHead,sizeof(BITMAPFILEHEADER),1,pSrcFile);
	if (SrcFileHead.bfType != 0x4D42)
	{
		printf("source file is not bmp file");
		return;
	}
	//read bit map information header
	fread(&SrcInfoHead,sizeof(BITMAPINFOHEADER),1,pSrcFile);

	//allocate memory space for source image
	int SrcWidth = SrcInfoHead.biWidth;
	int SrcHeight = SrcInfoHead.biHeight;
	//the actual number of bytes per row of the source image
	int SrcWidthMemByte = WIDTHBYTE(SrcWidth * SrcInfoHead.biBitCount);
	int SrcTotalMemByte = SrcHeight * SrcWidthMemByte;
	BYTE* pSrcData = (BYTE*)malloc(SrcTotalMemByte);
	memset(pSrcData,0,SrcTotalMemByte);
	fread(pSrcData, 1, SrcTotalMemByte, pSrcFile);

	//allocate memory space for destination image
	int DesWidth = DST_WIDTH;
	int DesHeight = DST_HEIGHT;
	//the actual number of bytes per row of the destination image
	int DesWidthMemByte = WIDTHBYTE(DesWidth* SrcInfoHead.biBitCount);
	int DesTotalMemByte = DesHeight * DesWidthMemByte;
	BYTE* pDesData = (BYTE*)malloc(DesTotalMemByte);
	memset(pDesData,0,DesTotalMemByte);

	//width and height ratio of source image and destination image
	double dFactorWidth = (double)SrcWidth / DesWidth;
	double dFactorHeight = (double)SrcHeight / DesHeight;

	//interpolation
	for(int y = 0; y < DesHeight; y++)
		for (int x = 0; x < DesWidth; x++)
		{
			double dSrcImageX = x * dFactorWidth;
			double dSrcImageY = y * dFactorHeight;
			//nearest interpolation
			NearestInterpolation(dSrcImageX, dSrcImageY, x, y, SrcWidth, SrcHeight, pSrcData, pDesData, SrcWidthMemByte, DesWidthMemByte);
		}

	//destination image bit map file header
	DesFileHead = SrcFileHead;
	DWORD bfSize = (SrcFileHead.bfOffBitsHigh << 16) + SrcFileHead.bfOffBitsLow + DesTotalMemByte;
	DesFileHead.bfSizeLow = bfSize & 0xFFFF;
	DesFileHead.bfSizeHigh = bfSize >> 16;
	//destination image bit map information header
	DesInfoHead = SrcInfoHead;
	DesInfoHead.biWidth = DesWidth;
	DesInfoHead.biHeight = DesHeight;
	DesInfoHead.biSizeImage = DesTotalMemByte;
	//open destination image file
	errno_t DesFileOpenError;
	DesFileOpenError = fopen_s(&pDesFile, "../DesPic.bmp", "wb");
	if (DesFileOpenError != 0)
	{
		printf("fail to open destination image file");
		return;
	}
	//write destination image into bitmapfile
	fwrite(&DesFileHead,1,sizeof(BITMAPFILEHEADER), pDesFile);
	fwrite(&DesInfoHead,1,sizeof(BITMAPINFOHEADER), pDesFile);
	fwrite(pDesData,1, DesTotalMemByte, pDesFile);

	fclose(pSrcFile);
	fclose(pDesFile);

	printf("finish image scale\n");
	return;
}

void NearestInterpolation(double dSrcX,double dSrcY,int iDesX,int iDesY,int iSrcWidth,int iSrcHeight,BYTE* pSrcMemData,BYTE* pDesMemData,int iSrcMemWidth,int iDesMemWidth)
{
	//source image pixel position
	int iSrcX = dSrcX + 0.5;
	int iSrcY = dSrcY + 0.5;
	//image boundary processing
	if (iSrcX > (iSrcWidth-1))
	{
		iSrcX = iSrcWidth - 1;
	}
	if (iSrcY > (iSrcHeight - 1))
	{
		iSrcY = iSrcHeight - 1;
	}
	int iSrcPixelPoint = iSrcY * iSrcMemWidth + iSrcX * 3;

	//destination image
	int iDesPixelPoint = iDesY * iDesMemWidth + iDesX * 3;
	pDesMemData[iDesPixelPoint] = pSrcMemData[iSrcPixelPoint];
	pDesMemData[iDesPixelPoint+1] = pSrcMemData[iSrcPixelPoint+1];
	pDesMemData[iDesPixelPoint+2] = pSrcMemData[iSrcPixelPoint+2];

	return;
}
*/