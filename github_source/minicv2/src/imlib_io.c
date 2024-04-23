#include "imlib_io.h"
#include <stdio.h>
#include <stdlib.h>
#include <stdarg.h>
#include "imlib_config.h"

#ifdef IMLIB_ENABLE_PRINT

static int _out_level = 1;

int imlib_printf(int level, char *fmt, ...)
{
    static char sprint_buf[1024];

    va_list args;
    int n = 0;
    if (level > _out_level) return n;
    va_start(args, fmt);
    n = vsprintf(sprint_buf, fmt, args);
    va_end(args);
    switch (level)
    {
    case 0:
        fprintf(stderr, "%s", sprint_buf);
        abort();
        break;
    case 1:
        fprintf(stdout, "%s", sprint_buf);
        break;
    case 2:
        fprintf(stdout, "%s", sprint_buf);
        break;
    case 3:
        fprintf(stdout, "%s", sprint_buf);
        break;
    case 4:
        fprintf(stdout, "%s", sprint_buf);
        break;
    case 5:
        fprintf(stdout, "%s", sprint_buf);
        break;
    default:
        break;
    }
    return n;
}

void imlib_set_print_out_level(int le)
{
    _out_level = le;
}

#else

int imlib_printf(int level, char *fmt, ...)
{

}

void imlib_set_print_out_level(int le)
{

}

#endif

#include "imlib.h"

void imlib_printf_image_info(void *img)
{
    image_t *imlib_img = (image_t *)img;
    switch (imlib_img->pixfmt)
    {
    case PIXFORMAT_GRAYSCALE:
        printf("imlib image info:\n\t\twidth:%d height:%d mode:PIXFORMAT_GRAYSCALE size:%d pixels:%p is_data_alloc:%d \n", imlib_img->w, imlib_img->h, imlib_img->size, imlib_img->pixels, imlib_img->is_data_alloc);
        break;
    case PIXFORMAT_RGB565:
        printf("imlib image info:\n\t\twidth:%d height:%d mode:PIXFORMAT_RGB565 size:%d pixels:%p is_data_alloc:%d \n", imlib_img->w, imlib_img->h, imlib_img->size, imlib_img->pixels, imlib_img->is_data_alloc);
        break;
    case PIXFORMAT_RGB888:
        printf("imlib image info:\n\t\twidth:%d height:%d mode:PIXFORMAT_RGB888 size:%d pixels:%p is_data_alloc:%d \n", imlib_img->w, imlib_img->h, imlib_img->size, imlib_img->pixels, imlib_img->is_data_alloc);
        break;
    default:
        printf("image is not info\n");
        break;
    }
}


