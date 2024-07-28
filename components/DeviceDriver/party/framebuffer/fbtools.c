#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <unistd.h>
#include <string.h>
#include <sys/ioctl.h>
#include <sys/mman.h>
#define PAGE_SHIFT  12
#define PAGE_MASK       (~((1 << PAGE_SHIFT) - 1)) 

#include "fbtools.h"

#define TRUE 1
#define FALSE 0
#define MAX (x, y)((x) > (y) ? (x) : (y))
#define MIN (x, y)((x) < (y) ? (x) : (y))

/* open & init a frame buffer */
int fb_open(PFBDEV pFbdev)
{
    pFbdev->fb = open(pFbdev-> dev, O_RDWR);
    if (pFbdev->fb < 0)
    {
        printf("Error opening %s: %m. Check kernel config/n", pFbdev->dev);
        return FALSE;
    }
    if (-1 == ioctl(pFbdev->fb, FBIOGET_VSCREENINFO, &(pFbdev->fb_var)))
    {
        printf("ioctl FBIOGET_VSCREENINFO/n");
        return FALSE;
    }
    if (-1 == ioctl(pFbdev->fb, FBIOGET_FSCREENINFO, &(pFbdev->fb_fix)))
    {
        printf("ioctl FBIOGET_FSCREENINFO/n");
        return FALSE;
    }

    /*map physics address to virtual address */
    pFbdev->fb_mem_offset = (unsigned long)(pFbdev->fb_fix.smem_start) & (~PAGE_MASK);
    pFbdev->fb_mem = (unsigned long int)mmap(NULL, pFbdev->fb_fix.smem_len + pFbdev->fb_mem_offset,
                                                PROT_READ | PROT_WRITE, MAP_SHARED, pFbdev->fb, 0);
    if (-1L == (long)pFbdev->fb_mem)
    {
        printf("mmap error! mem:%ld offset:%ld/n", pFbdev->fb_mem,
               pFbdev->fb_mem_offset);
        return FALSE;
    }

    return TRUE;
}

/* close frame buffer */
int fb_close(PFBDEV pFbdev)
{
    munmap(pFbdev->fb, pFbdev->fb_mem);
    close(pFbdev->fb);
    pFbdev->fb = -1;
    return 0;
}

void fb_draw_img(PFBDEV pFbdev, int w, int h, int color, void *buf)
{
    if(NULL == pFbdev || NULL == buf)
    {
        return;
    }
    unsigned char *buf_8 = (unsigned char *)buf;

    unsigned char *pen_8 = pFbdev->fb_mem;
    unsigned short *pen_16;
    unsigned int *pen_32;

    unsigned int red, green, blue;

    pen_16 = (unsigned short *)pen_8;
    pen_32 = (unsigned int *)pen_8;

    switch (pFbdev->fb_var.bits_per_pixel)
    {
    case 16:
        {
            switch (color)
            {
            case 24:
                {
                    for (int i = 0; i < w * h * 3; i += 3)
                    {
                        unsigned int t_color;
                        t_color = ((buf_8[i] >> 3) << 11) | ((buf_8[i+1] >> 2) << 5) | (buf_8[i+2] >> 3);
                        *pen_16++ = t_color;
                    }
                }
                break;
            
            default:
                break;
            }
        }
        break;
    
    default:
        break;
    }
}

/* get display depth */
int get_display_depth(PFBDEV pFbdev)
{
    if (pFbdev->fb <= 0)
    {
        printf("fb device not open, open it first/n");
        return FALSE;
    }
    return pFbdev->fb_var.bits_per_pixel;
}

/* full screen clear */
void fb_memset(void *addr, int c, size_t len)
{
    memset(addr, c, len);
}

/* full screen color*/
void fb_put_pixel(PFBDEV pFbdev, int x, int y, unsigned int color)
{
    unsigned char *pen_8 = pFbdev->fb_mem + y * pFbdev->fb_var.xres * pFbdev->fb_var.bits_per_pixel / 8 + x * pFbdev->fb_var.bits_per_pixel / 8;
    unsigned short *pen_16;
    unsigned int *pen_32;

    unsigned int red, green, blue;

    pen_16 = (unsigned short *)pen_8;
    pen_32 = (unsigned int *)pen_8;

    switch (pFbdev->fb_var.bits_per_pixel)
    {
    case 8:
    {
        *pen_8 = color;
        break;
    }
    case 16:
    {
        /* 565 */
        red = (color >> 16) & 0xff;
        green = (color >> 8) & 0xff;
        blue = (color >> 0) & 0xff;
        color = ((red >> 3) << 11) | ((green >> 2) << 5) | (blue >> 3);
        *pen_16 = color;
        break;
    }
    case 24:
    {
        *pen_32 = color;
        break;
    }
    case 32:
    {
        *pen_32 = color;
        break;
    }
    default:
    {
        printf("can't surport %dbpp\n", pFbdev->fb_var.bits_per_pixel);
        break;
    }
    }
}








/* use by test */
// #define DEBUG
#ifdef FB_TOOLS_DEBUG
main()
{
    FBDEV fbdev;
    memset(&fbdev, 0, sizeof(FBDEV));
    strcpy(fbdev.dev, "/dev/fb0");
    if (fb_open(&fbdev) == FALSE)
    {
        printf("open frame buffer error/n");
        return;
    }

    fb_memset(fbdev.fb_mem + fbdev.fb_mem_offset, 0, fbdev.fb_fix.smem_len);

    fb_close(&fbdev);
}
#endif