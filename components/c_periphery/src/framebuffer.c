#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <unistd.h>
#include <string.h>
#include <sys/ioctl.h>
#include <sys/mman.h>

#include <stddef.h>
#include <stdbool.h>
#include <stdarg.h>
#include <unistd.h>
#include <fcntl.h>
#include <errno.h>
#include <stdint.h>

#define PAGE_SHIFT 12
#define PAGE_MASK (~((1 << PAGE_SHIFT) - 1))

#include "framebuffer.h"

#define TRUE 1
#define FALSE 0
#define MAX (x, y)((x) > (y) ? (x) : (y))
#define MIN (x, y)((x) < (y) ? (x) : (y))



static int _fb_error(fb_t *fb, int code, int c_errno, const char *fmt, ...)
{
    va_list ap;

    fb->error.c_errno = c_errno;

    va_start(ap, fmt);
    vsnprintf(fb->error.errmsg, sizeof(fb->error.errmsg), fmt, ap);
    va_end(ap);

    /* Tack on strerror() and errno */
    if (c_errno)
    {
        char buf[64];
        strerror_r(c_errno, buf, sizeof(buf));
        snprintf(fb->error.errmsg + strlen(fb->error.errmsg), sizeof(fb->error.errmsg) - strlen(fb->error.errmsg), ": %s [errno %d]", buf, c_errno);
    }

    return code;
}

fb_t *fb_new(void)
{
    fb_t *fb = calloc(1, sizeof(fb_t));
    if (fb == NULL)
        return NULL;

    return fb;
}
int fb_open(fb_t *fb, const char *name)
{
    fb->fb = open(name, O_RDWR);
    if (fb->fb < 0)
    {
        return _fb_error(fb, FB_ERROR_OPEN, errno, "Error opening %s: %m. Check kernel config/n", fb->dev);
    }
    if (-1 == ioctl(fb->fb, FBIOGET_VSCREENINFO, &(fb->fb_var)))
    {
        return _fb_error(fb, FB_ERROR_OPEN, errno, "ioctl FBIOGET_VSCREENINFO/n");
    }
    if (-1 == ioctl(fb->fb, FBIOGET_FSCREENINFO, &(fb->fb_fix)))
    {
        return _fb_error(fb, FB_ERROR_OPEN, errno, "ioctl FBIOGET_FSCREENINFO/n");
    }

    /*map physics address to virtual address */
    fb->fb_mem_offset = (unsigned long)(fb->fb_fix.smem_start) & (~PAGE_MASK);
    fb->fb_mem = (unsigned long int)mmap(NULL, fb->fb_fix.smem_len + fb->fb_mem_offset,
                                         PROT_READ | PROT_WRITE, MAP_SHARED, fb->fb, 0);
    if (-1L == (long)fb->fb_mem)
    {
        return _fb_error(fb, FB_ERROR_OPEN, errno, "mmap error! mem:%ld offset:%ld/n", fb->fb_mem, fb->fb_mem_offset);
    }

    fb->w = fb->fb_var.xres;
    fb->h = fb->fb_var.yres;
    fb->pixel_bits = fb->fb_var.bits_per_pixel;
    return 1;
}

typedef struct pixel_s
{
    uint8_t red;
    uint8_t green;
    uint8_t blue;
} pixel24_t;

#define pixel32224(_u32_t)                   \
    ({                                       \
        __typeof__(_u32_t) __u32_t = _u32_t; \
        (*((pixel24_t *)&__u32_t));          \
    })

int fb_put_pixel(fb_t *fb, int x, int y, int clolr)
{
    switch (fb->pixel_bits)
    {
    case 16:
    {
        uint16_t *pixel16 = (uint16_t *)fb->fb_mem;
        pixel16[y * fb->w + x] = (clolr & 0xffff);
    }
    break;
    case 24:
    {
        pixel24_t *pixel24 = (pixel24_t *)fb->fb_mem;
        pixel24[y * fb->w + x] = pixel32224(clolr);
    }
    break;
    default:
        break;
    }
    return 0;
}

int fb_put_img(fb_t *fb, void *buf)
{
    memcpy((void *)fb->fb_mem, buf, fb->w * fb->h * fb->pixel_bits / 8);
    return 0;
}

int fb_memset(fb_t *fb, int clolr)
{
    if (clolr)
    {
        switch (fb->pixel_bits)
        {
        case 16:
        {
            uint16_t *pixel16 = (uint16_t *)fb->fb_mem;
            for (int i = 0; i < fb->w * fb->h; i++)
            {
                pixel16[i] = (clolr & 0xffff);
            }
        }
        break;
        case 24:
        {
            pixel24_t *pixel24 = (pixel24_t *)fb->fb_mem;
            for (int i = 0; i < fb->w * fb->h; i++)
            {
                pixel24[i] = pixel32224(clolr);
            }
        }
        break;
        default:
            break;
        }
    }
    else
    {
        memset((void *)fb->fb_mem, 0, fb->w * fb->h * fb->pixel_bits / 8);
    }
    return 0;
}

int fb_close(fb_t *fb)
{
    close(fb->fb);
    fb->fb = -1;
    return 0;
}

void fb_free(fb_t *fb)
{
    free(fb);
}

int fb_errno(fb_t *fb)
{
    return fb->error.c_errno;
}

const char *fb_errmsg(fb_t *fb)
{
    return fb->error.errmsg;
}
