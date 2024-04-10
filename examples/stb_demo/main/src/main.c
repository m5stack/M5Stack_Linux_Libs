#include <stdio.h>
#define STB_IMAGE_IMPLEMENTATION
#define STBI_NO_STDIO
#define STBI_ONLY_JPEG
#define STBI_NO_LINEAR
#define STBI_ASSERT(x) do{if(!(x)){printf("error!\n");}}while (0)


#include "stb_image.h"

#include "logo.h"

// 将 RGB888 数据转换为 RGB565 数据，直接在原始缓冲区上覆盖
void rgb888_to_rgb565_inplace(uint8_t* rgb888, int width, int height) {
    for (int i = 0; i < width * height; ++i) {
        uint8_t r = rgb888[i * 3];
        uint8_t g = rgb888[i * 3 + 1];
        uint8_t b = rgb888[i * 3 + 2];

        // 将 8 位的 R、G、B 值转换为 5 位 R、6 位 G、5 位 B
        uint16_t r565 = (r >> 3) & 0x1F;
        uint16_t g565 = (g >> 2) & 0x3F;
        uint16_t b565 = (b >> 3) & 0x1F;

        // 将 R、G、B 组合为 RGB565 格式
        uint16_t rgb565_pixel = (r565 << 11) | (g565 << 5) | b565;

        // 将 RGB565 数据写回到原始缓冲区
        *((uint16_t*)&rgb888[i * 2]) = rgb565_pixel;
    }
}



int main(int argc, char *argv[])
{
    printf("hello world!\n");
    int w,h, c;
    // stbi_us *img_data = stbi_load_16_from_memory(_home_nihao_poto_logo_jpg, _home_nihao_poto_logo_jpg_len, &w, &h, &c, 0);
    stbi_uc *img_data = stbi_load_from_memory(_home_nihao_poto_logo_jpg, _home_nihao_poto_logo_jpg_len, &w, &h, &c, 0);
    
    printf("image:w%d,h:%d,c:%d\n", w,h,c);
    rgb888_to_rgb565_inplace(img_data, w, h);
    stbi_image_free(img_data);
    return 0;
}