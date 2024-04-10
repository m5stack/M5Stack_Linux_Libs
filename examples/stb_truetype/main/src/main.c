#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#define STB_IMAGE_WRITE_IMPLEMENTATION
#include "stb_image_write.h" /* http://nothings.org/stb/stb_image_write.h */

#define STB_TRUETYPE_IMPLEMENTATION
#include "stb_truetype.h" /* http://nothings.org/stb/stb_truetype.h */



// #c---  
int enc_get_utf8_size(char pInput)
{
   unsigned char c = pInput;
   switch (c)
   {
   case 0x20 ... 0x7f:
        return 1;
   case 0xC0 ... 0xDF:
        return 2;
   case 0xE0 ... 0xEF:
        return 3;
   case 0xF0 ... 0xF7:
        return 4;
   case 0xF8 ... 0xFB:
        return 5;
   case 0xFC ... 0xFD:
        return 6;
   default:
       return -1;
   }
   // 0xxxxxxx 返回0
   // 10xxxxxx 不存在
   // 110xxxxx 返回2
   // 1110xxxx 返回3
   // 11110xxx 返回4
   // 111110xx 返回5
    // 1111110x 返回6
    // if(c< 0x80) return 1;
    // if(c>=0x80 && c<0xC0) return -1;
    // if(c>=0xC0 && c<0xE0) return 2;
    // if(c>=0xE0 && c<0xF0) return 3;
    // if(c>=0xF0 && c<0xF8) return 4;
    // if(c>=0xF8 && c<0xFC) return 5;
    // if(c>=0xFC) return 6;
}
/***************************************************************************** 
 * 将一个字符的UTF8编码转换成Unicode(UCS-2和UCS-4)编码. 
 * 
 * 参数: 
 *    pInput      指向输入缓冲区, 以UTF-8编码 
 *    Unic        指向输出缓冲区, 其保存的数据即是Unicode编码值, 
 *                类型为unsigned long . 
 * 
 * 返回值: 
 *    成功则返回该字符的UTF8编码所占用的字节数; 失败则返回0. 
 * 
 * 注意: 
 *     1. UTF8没有字节序问题, 但是Unicode有字节序要求; 
 *        字节序分为大端(Big Endian)和小端(Little Endian)两种; 
 *        在Intel处理器中采用小端法表示, 在此采用小端法表示. (低地址存低位) 
 ****************************************************************************/  
int enc_utf8_to_unicode_one(unsigned char* pInput, unsigned long *Unic)  
{  
    assert(pInput != NULL && Unic != NULL);  

    // b1 表示UTF-8编码的pInput中的高字节, b2 表示次高字节, ...  
    char b1, b2, b3, b4, b5, b6;  

    *Unic = 0x0; // 把 *Unic 初始化为全零  
    int utfbytes = enc_get_utf8_size(*pInput);  
    unsigned char *pOutput = (unsigned char *) Unic;  

    switch ( utfbytes )  
    {  
        case 1:  
            *Unic     = *pInput;
            break;  
        case 2:  
            b1 = *pInput;  
            b2 = *(pInput + 1);  
            if ( (b2 & 0xE0) != 0x80 )  
                return 0;  
            *pOutput     = (b1 << 6) + (b2 & 0x3F);  
            *(pOutput+1) = (b1 >> 2) & 0x07;  
            break;  
        case 3:  
            b1 = *pInput;  
            b2 = *(pInput + 1);  
            b3 = *(pInput + 2);  
            if ( ((b2 & 0xC0) != 0x80) || ((b3 & 0xC0) != 0x80) )  
                return 0;  
            *pOutput     = (b2 << 6) + (b3 & 0x3F);  
            *(pOutput+1) = (b1 << 4) + ((b2 >> 2) & 0x0F);  
            break;  
        case 4:  
            b1 = *pInput;  
            b2 = *(pInput + 1);  
            b3 = *(pInput + 2);  
            b4 = *(pInput + 3);  
            if ( ((b2 & 0xC0) != 0x80) || ((b3 & 0xC0) != 0x80)  
                    || ((b4 & 0xC0) != 0x80) )  
                return 0;  
            *pOutput     = (b3 << 6) + (b4 & 0x3F);  
            *(pOutput+1) = (b2 << 4) + ((b3 >> 2) & 0x0F);  
            *(pOutput+2) = ((b1 << 2) & 0x1C)  + ((b2 >> 4) & 0x03);  
            break;  
        case 5:  
            b1 = *pInput;  
            b2 = *(pInput + 1);  
            b3 = *(pInput + 2);  
            b4 = *(pInput + 3);  
            b5 = *(pInput + 4);  
            if ( ((b2 & 0xC0) != 0x80) || ((b3 & 0xC0) != 0x80)  
                    || ((b4 & 0xC0) != 0x80) || ((b5 & 0xC0) != 0x80) )  
                return 0;  
            *pOutput     = (b4 << 6) + (b5 & 0x3F);  
            *(pOutput+1) = (b3 << 4) + ((b4 >> 2) & 0x0F);  
            *(pOutput+2) = (b2 << 2) + ((b3 >> 4) & 0x03);  
            *(pOutput+3) = (b1 << 6);  
            break;  
        case 6:  
            b1 = *pInput;  
            b2 = *(pInput + 1);  
            b3 = *(pInput + 2);  
            b4 = *(pInput + 3);  
            b5 = *(pInput + 4);  
            b6 = *(pInput + 5);  
            if ( ((b2 & 0xC0) != 0x80) || ((b3 & 0xC0) != 0x80)  
                    || ((b4 & 0xC0) != 0x80) || ((b5 & 0xC0) != 0x80)  
                    || ((b6 & 0xC0) != 0x80) )  
                return 0;  
            *pOutput     = (b5 << 6) + (b6 & 0x3F);  
            *(pOutput+1) = (b5 << 4) + ((b6 >> 2) & 0x0F);  
            *(pOutput+2) = (b3 << 2) + ((b4 >> 4) & 0x03);  
            *(pOutput+3) = ((b1 << 6) & 0x40) + (b2 & 0x3F);  
            break;  
        default:  
            return 0;  
            break;  
    }  
    return utfbytes;  
}  


// #c---end 
int main(int argc, const char *argv[])
{
    /* 加载字体（.ttf）文件 */
    long int size = 0;
    unsigned char *fontBuffer = NULL;

    FILE *fontFile = fopen("/home/nihao/work/lin/tmp/tmppppp.ttf", "rb");
    if (fontFile == NULL)
    {
        printf("Can not open font file!\n");
        return 0;
    }
    fseek(fontFile, 0, SEEK_END); /* 设置文件指针到文件尾，基于文件尾偏移0字节 */
    size = ftell(fontFile);       /* 获取文件大小（文件尾 - 文件头  单位：字节） */
    fseek(fontFile, 0, SEEK_SET); /* 重新设置文件指针到文件头 */

    fontBuffer = calloc(size, sizeof(unsigned char));
    fread(fontBuffer, size, 1, fontFile);
    fclose(fontFile);

    /* 初始化字体 */
    stbtt_fontinfo info;
    if (!stbtt_InitFont(&info, fontBuffer, 0))
    {
        printf("stb init font failed\n");
    }
    /* 创建位图 */
    int bitmap_w = 512; /* 位图的宽 */
    int bitmap_h = 128; /* 位图的高 */
    unsigned char *bitmap = calloc(bitmap_w * bitmap_h, sizeof(unsigned char));

    /* "STB"的 unicode 编码 */
    // char word[20] = {0x53, 0x54, 0x42};
    char *nihao_ts = "你好呀!你";
    uint32_t word_un[50] = {0};
    // enc_utf8_to_unicode_one(nihao_ts, word_un);
    char *tmp_c = nihao_ts;
    uint32_t *tmp_u = word_un;
    int mkle = 0;
    while (*tmp_c != '\0')
    {
        enc_utf8_to_unicode_one(tmp_c, tmp_u);
        tmp_c += enc_get_utf8_size(*tmp_c);
        tmp_u ++;
        mkle ++;
        
    }
    /* 计算字体缩放 */
    float pixels = 60.0;                                    /* 字体大小（字号） */
    float scale = stbtt_ScaleForPixelHeight(&info, pixels); /* scale = pixels / (ascent - descent) */

    /** 
     * 获取垂直方向上的度量 
     * ascent：字体从基线到顶部的高度；
     * descent：基线到底部的高度，通常为负值；
     * lineGap：两个字体之间的间距；
     * 行间距为：ascent - descent + lineGap。
    */
    int ascent = 0;
    int descent = 0;
    int lineGap = 0;
    stbtt_GetFontVMetrics(&info, &ascent, &descent, &lineGap);

    /* 根据缩放调整字高 */
    ascent = roundf(ascent * scale);
    descent = roundf(descent * scale);

    int x = 0; /*位图的x*/
// #define unical_nu  0x9f7f
    /* 循环加载word中每个字符 */
    for (int i = 0; i < mkle; ++i)
    {
        /** 
          * 获取水平方向上的度量
          * advanceWidth：字宽；
          * leftSideBearing：左侧位置；
        */
        int advanceWidth = 0;
        int leftSideBearing = 0;
        stbtt_GetCodepointHMetrics(&info, word_un[i], &advanceWidth, &leftSideBearing);

        /* 获取字符的边框（边界） */
        int c_x1, c_y1, c_x2, c_y2;
        stbtt_GetCodepointBitmapBox(&info, word_un[i], scale, scale, &c_x1, &c_y1, &c_x2, &c_y2);

        /* 计算位图的y (不同字符的高度不同） */
        int y = ascent + c_y1;

        /* 渲染字符 */
        int byteOffset = x + roundf(leftSideBearing * scale) + (y * bitmap_w);
        stbtt_MakeCodepointBitmap(&info, bitmap + byteOffset, c_x2 - c_x1, c_y2 - c_y1, bitmap_w, scale, scale, word_un[i]);

        /* 调整x */
        x += roundf(advanceWidth * scale);

        /* 调整字距 */
        int kern;
        kern = stbtt_GetCodepointKernAdvance(&info, word_un[i], word_un[i]);
        x += roundf(kern * scale);
    }

    /* 将位图数据保存到1通道的png图像中 */
    stbi_write_png("/home/nihao/work/lin/nihao/STB.png", bitmap_w, bitmap_h, 1, bitmap, bitmap_w);

    free(fontBuffer);
    free(bitmap);

    return 0;
}
