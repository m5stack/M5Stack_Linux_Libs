// 引入所需的头文件
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ft2build.h>
#include FT_FREETYPE_H
#include FT_BITMAP_H

int main(int argc, char *argv[])
{
    FT_Library library;  // FreeType 库对象
    FT_Face face;        // 字体对象
    FT_GlyphSlot slot;   // 字形槽对象，用于加载和渲染字符
    FT_Error error;      // 错误码

    // 初始化 FreeType 库
    error = FT_Init_FreeType(&library);
    if (error)
    {
        printf("Error initializing FreeType library\n");
        return 1;
    }

    // 加载字体文件并创建字体对象
    error = FT_New_Face(library, "fangsong_GB2312.ttf", 0, &face);
    if (error)
    {
        printf("Error loading font\n");
        FT_Done_FreeType(library);  // 释放 FreeType 库资源
        return 1;
    }

    // 设置字符大小
    error = FT_Set_Char_Size(face, 0, 16 * 64, 300, 300);
    if (error)
    {
        printf("Error setting character size\n");
        FT_Done_Face(face);        // 释放字体对象
        FT_Done_FreeType(library);  // 释放 FreeType 库资源
        return 1;
    }

    slot = face->glyph;  // 获取字形槽对象

    const wchar_t text[] = {'h','e','l','l','o',' ','w','o','r','l','d'};
    int num_chars = sizeof(text) / sizeof(text[0]);

    for (int n = 0; n < num_chars; n++)
    {
        // 加载字符的字形数据
        error = FT_Load_Char(face, text[n], FT_LOAD_RENDER);
        if (error)
        {
            continue; // 忽略错误并继续循环
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
                printf("%c", pixel > 128 ? '#' : '-');
            }
            printf("\n");
        }

        // 释放字符的字形位图数据
        FT_Bitmap_Done(library, &face->glyph->bitmap);
    }

    FT_Done_Face(face);        // 释放字体对象
    FT_Done_FreeType(library);  // 释放 FreeType 库资源

    return 0;
}
