/*
 * c-periphery
 * https://github.com/vsergeev/c-periphery
 * License: MIT
 */

#ifndef _FRAMEBUFFER_H
#define _FRAMEBUFFER_H

#ifdef __cplusplus
extern "C" {
#endif

#include <linux/fb.h>
#include <stdio.h>
#include <stdbool.h>

enum fb_error_code {
    FB_ERROR_ARG       = -1, /* Invalid arguments */
    FB_ERROR_OPEN      = -2, /* Opening FBx */
    FB_ERROR_QUERY     = -3, /* Querying FBx attributes */
    FB_ERROR_IO        = -4, /* Reading/writing FBx brightness */
    FB_ERROR_CLOSE     = -5, /* Closing FBx */
};

/* a framebuffer device structure */
struct fb_handle
{
    int fb;
    unsigned long fb_mem_offset;
    unsigned long fb_mem;
    struct fb_fix_screeninfo fb_fix;
    struct fb_var_screeninfo fb_var;
    int w;
    int h;
    int pixel_bits;
    char dev[20];
    struct
    {
        int c_errno;
        char errmsg[96];
    } error;
};

typedef struct fb_handle fb_t;

/* Primary Functions */
fb_t *fb_new(void);
int fb_open(fb_t *fb, const char *name);
int fb_put_pixel(fb_t *fb, int x, int y, int clolr);
int fb_put_img(fb_t *fb, void *buf);
int fb_memset(fb_t *fb, int clolr);
int fb_close(fb_t *fb);
void fb_free(fb_t *fb);

/* Error Handling */
int fb_errno(fb_t *fb);
const char *fb_errmsg(fb_t *fb);

#ifdef __cplusplus
}
#endif

#endif

