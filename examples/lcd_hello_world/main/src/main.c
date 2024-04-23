#include <stdio.h>
#include <string.h>
#include "framebuffer/fbtools.h"
#include "imlib.h"

int main(int argc, char **argv)
{
    FBDEV fbdev;
    memset(&fbdev, 0, sizeof(FBDEV));
    strcpy(fbdev.dev, "/dev/fb1");
    if (fb_open(&fbdev) == 0)
    {
        printf("open frame buffer error/n");
        return -1;
    }
    int w = fbdev.fb_var.xres;
    int h = fbdev.fb_var.yres;
    int color = fbdev.fb_var.bits_per_pixel;

    image_t *img = imlib_image_create(w, h, PIXFORMAT_RGB565, w * h * 2, (void*)fbdev.fb_mem, 0);
    fb_memset((void*)(fbdev.fb_mem + fbdev.fb_mem_offset), 0, fbdev.fb_fix.smem_len);

    char strs[100];
    sprintf(strs, "hello_world!");
    imlib_draw_string(img, 75, 110, strs,     COLOR_R8_G8_B8_TO_RGB565(0xff, 0xff, 0xff), 2, 0, 0, 1, 0, 0, 0, 0, 0, 0);

    imlib_image_destroy(&img);
    fb_close(&fbdev);
    return 0;
}
