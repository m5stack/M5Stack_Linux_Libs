#include <linux/fb.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/mman.h>
#include <sys/ioctl.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <fcntl.h>

#include <stdlib.h>
#include <string.h>
#include <ft2build.h>
#include FT_FREETYPE_H
#include FT_BITMAP_H

#include <stdio.h>
#include <stdint.h>

struct fb_fix_screeninfo finfo;
struct fb_var_screeninfo vinfo;

int main(int argc, char *argv[])
{
    int fbdev = open("/dev/fb1", O_RDWR);
    if (fbdev < 0)
    {
        perror("Error: cannot open framebuffer device");
        exit(-1);
    }

    // fb_fix_screeninfo 通过 fbdev 获取屏幕固定的相关信息
    if (ioctl(fbdev, FBIOGET_FSCREENINFO, &finfo) == -1)
    {
        perror("Error reading fixed information");
        close(fbdev);
        exit(-2);
    }
    // fb_var_screeninfo 通过 fbdev 获取可变的信息，可以调用参数为`FBIOPUT_VSCREENINFO`的重新进行设置
    if (ioctl(fbdev, FBIOGET_VSCREENINFO, &vinfo) == -1)
    {
        perror("Error reading variable information");
        close(fbdev);
        exit(-3);
    }

    // 计算显存的总大小
    long int screensize = vinfo.yres_virtual * finfo.line_length;

    // 将显示内存映射到用户空间内
    char *fbp = (char *)mmap(NULL, screensize, PROT_READ | PROT_WRITE, MAP_SHARED, fbdev, 0);
    if ((size_t)fbp == -1)
    {
        perror("Error: failed to map framebuffer device to memory");
        exit(-4);
    }

    // 计算当前缓冲区偏移
    vinfo.yoffset = vinfo.yoffset + vinfo.yres >= vinfo.yres_virtual ? 0 : vinfo.yoffset + vinfo.yres;
    uint16_t *fb_mem = (uint16_t *)(fbp + finfo.line_length * vinfo.yoffset);

    FT_Library library;
    FT_Face face;
    FT_GlyphSlot slot;
    FT_Error error;

    error = FT_Init_FreeType(&library);
    if (error)
    {
        printf("Error initializing FreeType library\n");
        return 1;
    }

    error = FT_New_Face(library, "fangsong_GB2312.ttf", 0, &face);
    if (error)
    {
        printf("Error loading font\n");
        FT_Done_FreeType(library);
        return 1;
    }

    error = FT_Set_Char_Size(face, 0, 16 * 64 * 3, 320, 240);
    if (error)
    {
        printf("Error setting character size\n");
        FT_Done_Face(face);
        FT_Done_FreeType(library);
        return 1;
    }

    slot = face->glyph;

    // 哈
    const FT_ULong text = 0x54c8;

    error = FT_Load_Char(face, text, FT_LOAD_RENDER);
    if (error)
    {
        goto error_end; // Ignore errors and continue.
    }
    int height = slot->bitmap.rows;
    int width = slot->bitmap.width;
    int bitlen = slot->bitmap.pitch;
    for (int row = 0; row < height; ++row)
    {
        for (int col = 0; col < width; ++col)
        {
            int index = row * bitlen + col;

            // 对于灰度位图，值越高表示越亮
            unsigned char pixel = slot->bitmap.buffer[index];
            fb_mem[row * vinfo.xres + col] = pixel > 128 ? 0xffff : 0;
        }
    }

    FT_Bitmap_Done(library, &face->glyph->bitmap);
error_end:

    FT_Done_Face(face);
    FT_Done_FreeType(library);

    // 如果使用了多缓冲，切换显示界面
    if (vinfo.yres_virtual != vinfo.yres)
    {
        if (ioctl(fbdev, FBIOPAN_DISPLAY, &vinfo) == -1)
        {
            perror("Error display frame");
        }
    }

    // 解除内存映射
    munmap(fbp, screensize);
    // 关闭设备
    close(fbdev);

    return 0;
}
