#ifndef __IMLIB_IO__
#define __IMLIB_IO__
#ifdef __cplusplus
extern "C"
{
#endif


int imlib_printf(int level, char *fmt, ...);
void imlib_set_print_out_level(int le);



#define DEBUG_PRINT(fmt, ...) do{\
		imlib_printf(4,fmt"  [DEBUG:%s:%d] [%s]\n", ##__VA_ARGS__, __FILE__, __LINE__, __FUNCTION__);\
}while(0);

#define LOG_PRINT(fmt, ...) do{\
		imlib_printf(3,fmt"  [LOG] [%s]\n", ##__VA_ARGS__, __FUNCTION__);\
}while(0);

#define INFO_PRINT(fmt, ...) do{\
		imlib_printf(2,fmt"  [INFO]\n", ##__VA_ARGS__);\
}while(0);

#define WORN_PRINT(fmt, ...) do{\
		imlib_printf(1,fmt"  [WORN:%s:%d] [%s]\n", ##__VA_ARGS__, __FILE__, __LINE__, __FUNCTION__);\
}while(0);


#define ERR_PRINT(fmt, ...) do{\
		imlib_printf(0, fmt"  [ERR:%s:%d] [%s]\n", ##__VA_ARGS__, __FILE__, __LINE__, __FUNCTION__);\
}while(0);


void imlib_printf_image_info(void *img);


#define IMLIB_2_MAIX(img) \
({\
	const libmaix_image_mode_t __tmp_mode[4] = {LIBMAIX_IMAGE_MODE_GRAY, LIBMAIX_IMAGE_MODE_GRAY, LIBMAIX_IMAGE_MODE_RGB565, LIBMAIX_IMAGE_MODE_RGB888};\
	libmaix_image_create(img->w, img->h, __tmp_mode[img->bpp], LIBMAIX_IMAGE_LAYOUT_HWC, img->data, false);\
})


#define MAIX_2_IMLIB(img) \
({\
	const pixformat_t __tmp_mode[5] = {PIXFORMAT_GRAYSCALE, PIXFORMAT_GRAYSCALE, PIXFORMAT_GRAYSCALE, PIXFORMAT_RGB888, PIXFORMAT_RGB565};\
	imlib_image_create(img->width, img->height, __tmp_mode[img->mode], img->width * img->height * (__tmp_mode[img->mode] & 0xff), img->data, false);\
})

#ifdef __cplusplus
}
#endif
#endif // __IMLIB_IO__




