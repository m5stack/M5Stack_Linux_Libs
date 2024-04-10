#ifndef _FBTOOLS_H_
#define _FBTOOLS_H_

#include <linux/fb.h>
#include <stdio.h>
#if __cplusplus
extern "C"
{
#endif
/* a framebuffer device structure */
typedef struct fbdev{
int fb;
unsigned long fb_mem_offset;
unsigned long fb_mem;
struct fb_fix_screeninfo fb_fix;
struct fb_var_screeninfo fb_var;
char dev[20];
} FBDEV, *PFBDEV;

/* open & init a frame buffer */
/* to use this function,
you must set FBDEV.dev="/dev/fb0"
or "/dev/fbX" */
/* it's your frame buffer. */
int fb_open(PFBDEV pFbdev);

/*close a frame buffer*/
int fb_close(PFBDEV pFbdev);

/*get display depth*/
int get_display_depth(PFBDEV pFbdev);

/*draw rgb888 or rgb565 to framebuffer.color=24/16*/
void fb_draw_img(PFBDEV pFbdev, int w, int h, int color, void *buf);

void fb_put_pixel(PFBDEV pFbdev, int x, int y, unsigned int color);

/*full screen clear */
void fb_memset(void *addr, int c, size_t len);
#if __cplusplus
}
#endif
#endif
