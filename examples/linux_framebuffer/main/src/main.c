#define main_define 3
#if main_define == 1
#include <sys/mman.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include <linux/fb.h>
#include <fcntl.h>
#include <stdio.h>
#include <string.h>
#include <sys/ioctl.h>
#include <stdlib.h>
static int fd_fb;
static struct fb_var_screeninfo var; /* Current var */
static struct fb_fix_screeninfo finfo;
static int screen_size;
static unsigned char *fb_base;
static unsigned int line_width;
static unsigned int pixel_width;

/**********************************************************************
 * 函数名称： lcd_put_pixel
 * 功能描述： 在LCD指定位置上输出指定颜色（描点）
 * 输入参数： x坐标，y坐标，颜色
 * 输出参数： 无
 * 返 回 值： 会
 * 修改日期        版本号     修改人	      修改内容
 * -----------------------------------------------
 * 2020/05/12	     V1.0	  zh(angenao)	      创建
 ***********************************************************************/
void lcd_put_pixel(int x, int y, unsigned int color)
{
    unsigned char *pen_8 = fb_base + y * line_width + x * pixel_width;
    unsigned short *pen_16;
    unsigned int *pen_32;

    unsigned int red, green, blue;

    pen_16 = (unsigned short *)pen_8;
    pen_32 = (unsigned int *)pen_8;

    switch (var.bits_per_pixel)
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
        printf("can't surport %dbpp\n", var.bits_per_pixel);
        break;
    }
    }
}

int main(int argc, char **argv)
{
    int i, j;

    fd_fb = open("/dev/fb0", O_RDWR);
    if (fd_fb < 0)
    {
        printf("can't open /dev/fb0\n");
        return -1;
    }
    if (ioctl(fd_fb, FBIOGET_VSCREENINFO, &var))
    {
        printf("can't get var\n");
        return -1;
    }
    if (ioctl(fd_fb, FBIOGET_FSCREENINFO, &finfo))
    {
        printf("Error reading fixed information\n");
        exit(-1);
    }
    // 显示结构体信息
    printf("The mem is :%d\n", finfo.smem_len);
    printf("The line_length is :%d\n", finfo.line_length);
    printf("The xres is :%d\n", var.xres);
    printf("The yres is :%d\n", var.yres);
    printf("bits_per_pixel is :%d\n", var.bits_per_pixel);
    line_width = var.xres * var.bits_per_pixel / 8;

    pixel_width = var.bits_per_pixel / 8;
    // 计算显存大小
    screen_size = var.xres * var.yres * var.bits_per_pixel / 8;

    /*这就是把fp所指的文件中从开始到screensize大小的内容给映射出来，得到一个指向这块空间的指针*/
    fb_base = (unsigned char *)mmap(NULL, screen_size, PROT_READ | PROT_WRITE, MAP_SHARED, fd_fb, 0);
    if (fb_base == (unsigned char *)-1)
    {
        printf("can't mmap\n");
        return -1;
    }

    /* 清屏: 全部设为白色 */
    memset(fb_base, 0xff, screen_size);

    for (i = 0; i < var.xres / 2; i++)
        for (j = 0; j < var.yres / 2; j++)
            lcd_put_pixel(i, j, 0xFF0000); // 设为红色

    for (i = var.xres / 2; i < var.xres; i++)
        for (j = var.yres / 2; j < var.yres; j++)
            lcd_put_pixel(i, j, 0xFF0000); // 设为红色

    munmap(fb_base, screen_size); /*解除映射*/
    close(fd_fb);                 /*关闭文件*/

    return 0;
}

#elif main_define == 2

#include <unistd.h>
#include <stdio.h>
#include <fcntl.h>
#include <linux/fb.h>
#include <sys/mman.h>
inline static unsigned short int make16color(unsigned char r, unsigned char g, unsigned char b)
{
    return (
        (((r >> 3) & 31) << 11) |
        (((g >> 2) & 63) << 5) |
        ((b >> 3) & 31));
}
int main(int argc, char **argv)
{
    int fbfd = 0;
    struct fb_var_screeninfo vinfo;
    struct fb_fix_screeninfo finfo;
    long int screensize = 0;
    char *fbp = 0;
    int x = 0, y = 0;
    int guage_height = 20, step = 10;
    long int location = 0;
    // Open the file for reading and writing
    fbfd = open("/dev/fb0", O_RDWR);
    if (!fbfd)
    {
        printf("Error: cannot open framebuffer device.\n");
        exit(1);
    }
    printf("The framebuffer device was opened successfully.\n");
    // Get fixed screen information
    if (ioctl(fbfd, FBIOGET_FSCREENINFO, &finfo))
    {
        printf("Error reading fixed information.\n");
        exit(2);
    }
    // Get variable screen information
    if (ioctl(fbfd, FBIOGET_VSCREENINFO, &vinfo))
    {
        printf("Error reading variable information.\n");
        exit(3);
    }
    printf("sizeof(unsigned short) = %d\n", sizeof(unsigned short));
    printf("%dx%d, %dbpp\n", vinfo.xres, vinfo.yres, vinfo.bits_per_pixel);
    printf("xoffset:%d, yoffset:%d, line_length: %d\n", vinfo.xoffset, vinfo.yoffset, finfo.line_length);
    // Figure out the size of the screen in bytes
    screensize = vinfo.xres * vinfo.yres * vinfo.bits_per_pixel / 8;
    ;
    // Map the device to memory
    fbp = (char *)mmap(0, screensize, PROT_READ | PROT_WRITE, MAP_SHARED,
                       fbfd, 0);
    if ((int)fbp == -1)
    {
        printf("Error: failed to map framebuffer device to memory.\n");
        exit(4);
    }
    printf("The framebuffer device was mapped to memory successfully.\n");
    // set to black color first
    memset(fbp, 0, screensize);
    // draw rectangle
    y = (vinfo.yres - guage_height) / 2 - 2; // Where we are going to put the pixel
    for (x = step - 2; x < vinfo.xres - step + 2; x++)
    {
        location = (x + vinfo.xoffset) * (vinfo.bits_per_pixel / 8) +
                   (y + vinfo.yoffset) * finfo.line_length;
        *((unsigned short int *)(fbp + location)) = 255;
    }
    y = (vinfo.yres + guage_height) / 2 + 2; // Where we are going to put the pixel
    for (x = step - 2; x < vinfo.xres - step + 2; x++)
    {
        location = (x + vinfo.xoffset) * (vinfo.bits_per_pixel / 8) +
                   (y + vinfo.yoffset) * finfo.line_length;
        *((unsigned short int *)(fbp + location)) = 255;
    }
    x = step - 2;
    for (y = (vinfo.yres - guage_height) / 2 - 2; y < (vinfo.yres + guage_height) / 2 + 2; y++)
    {
        location = (x + vinfo.xoffset) * (vinfo.bits_per_pixel / 8) +
                   (y + vinfo.yoffset) * finfo.line_length;
        *((unsigned short int *)(fbp + location)) = 255;
    }
    x = vinfo.xres - step + 2;
    for (y = (vinfo.yres - guage_height) / 2 - 2; y < (vinfo.yres + guage_height) / 2 + 2; y++)
    {
        location = (x + vinfo.xoffset) * (vinfo.bits_per_pixel / 8) +
                   (y + vinfo.yoffset) * finfo.line_length;
        *((unsigned short int *)(fbp + location)) = 255;
    }
    // Figure out where in memory to put the pixel
    for (x = step; x < vinfo.xres - step; x++)
    {
        for (y = (vinfo.yres - guage_height) / 2; y < (vinfo.yres + guage_height) / 2; y++)
        {
            location = (x + vinfo.xoffset) * (vinfo.bits_per_pixel / 8) +
                       (y + vinfo.yoffset) * finfo.line_length;
            if (vinfo.bits_per_pixel == 32)
            {
                *(fbp + location) = 100;                     // Some blue
                *(fbp + location + 1) = 15 + (x - 100) / 2;  // A little green
                *(fbp + location + 2) = 200 - (y - 100) / 5; // A lot of red
                *(fbp + location + 3) = 0;                   // No transparency
            }
            else
            { // assume 16bpp
                unsigned char b = 255 * x / (vinfo.xres - step);
                unsigned char g = 255; // (x - 100)/6 A little green
                unsigned char r = 255; // A lot of red
                unsigned short int t = make16color(255, 0, 0);
                *((unsigned short int *)(fbp + location)) = t;
            }
        }
        // printf("x = %d, temp = %d\n", x, temp);
        // sleep to see it
        usleep(200);
    }
    // clean framebuffer
    munmap(fbp, screensize);
    close(fbfd);
    return 0;
}
#elif main_define == 3

#include <stdio.h>
#include <string.h>
#include "framebuffer/fbtools.h"
int main(int argc, char **argv)
{
    FBDEV fbdev;
    memset(&fbdev, 0, sizeof(FBDEV));
    strcpy(fbdev.dev, "/dev/fb0");
    if (fb_open(&fbdev) == 0)
    {
        printf("open frame buffer error/n");
        return -1;
    }
    printf("The mem is :%d\n", fbdev.fb_fix.smem_len);
    printf("The line_length is :%d\n", fbdev.fb_fix.line_length);
    printf("The xres is :%d\n", fbdev.fb_var.xres);
    printf("The yres is :%d\n", fbdev.fb_var.yres);
    printf("bits_per_pixel is :%d\n", fbdev.fb_var.bits_per_pixel);
    fb_memset((void*)(fbdev.fb_mem + fbdev.fb_mem_offset), 0, fbdev.fb_fix.smem_len);

    fb_close(&fbdev);
    return 0;
}

#endif