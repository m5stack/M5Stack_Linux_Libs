#include "u8g2.h"
#include <stdio.h>
#include <unistd.h>
#include "m5_sh1107/m5_sh1107.h"

// #include "linux_i2c.h"

#include <string.h>

#include "au8x8_d_null_cb.h"

u8g2_t u8g2;

void drawScreenInfo()
{
  u8g2_ClearBuffer(&u8g2); // 清空缓冲区

  // 获取屏幕信息
  int screenWidth = u8g2_GetDisplayWidth(&u8g2);
  int screenHeight = u8g2_GetDisplayHeight(&u8g2);
  int fontHeight = u8g2_GetAscent(&u8g2) - u8g2_GetDescent(&u8g2);
  int colorDepth = u8g2.draw_color;
  // 在屏幕上打印屏幕信息
  printf("screenWidth: %d \n", screenWidth);
  printf("screenHeight: %d \n", screenHeight);
  printf("fontHeight: %d \n", fontHeight);
  printf("colorDepth: %d \n", colorDepth);
}

static uint8_t buf[1024];

#define IMAGE_GET_BINARY_PIXEL(image, x, y)                \
  ({                                                       \
    uint8_t *_image = (image);                             \
    int _x = (x);                                          \
    int _y = (y);                                          \
    (_image[(((int)(_y / 8)) * 64 + _x)] >> (_y % 8)) & 1; \
  })

int get_display_buff_pixel(int x, int y)
{
  // int y_group_num = y / 8;
  // int y_iteam_num = y % 8;
  // int byte_num = y_group_num * 64 + x;
  // int pixel = (buf[byte_num] >> y_iteam_num) & 0x01;
  int pixel = (buf[(((int)(y / 8)) * 64 + x)] >> (y % 8)) & 0x01;
  // if(y < 64)
  // printf("x :%d, y :%d, y_group_num: %d, y_iteam_num: %d, byte_num:%d\n", x, y, y_group_num, y_iteam_num, byte_num);
  return pixel;
}

void display_show()
{
  uint8_t buff[8];
  for (int p = 0; p < 64; p++)
  {
    memset(buff, 0, 8);
    for (int i = 0; i < 8; i++)
    {
      for (int j = 0; j < 8; j++)
      {
        // if (get_display_buff_pixel(63 - p, i * 8 + j))
        if (IMAGE_GET_BINARY_PIXEL(buf, 63 - p, i * 8 + j))
        {
          buff[i] |= 0x01 << j;
        }
      }
    }
    m5_sh1107_dev_set_img(0, p, buff, 8);
  }
  for (int p = 0; p < 64; p++)
  {
    memset(buff, 0, 8);
    for (int i = 0; i < 8; i++)
    {
      for (int j = 0; j < 8; j++)
      {
        // if (get_display_buff_pixel(63 - p, 64 + i * 8 + j))
        if (IMAGE_GET_BINARY_PIXEL(buf, 63 - p, 64 + i * 8 + j))
        {
          buff[i] |= 0x01 << j;
        }
      }
    }
    m5_sh1107_dev_set_img(8, p, buff, 8);
  }
}

class sh1107dev
{
public:
  sh1107dev()
  {
    m5_sh1107_dev_init(1);
  }
  ~sh1107dev()
  {
    m5_sh1107_dev_deinit();
  }
};

// extern const uint8_t u8g2_msyh[] U8G2_FONT_SECTION("u8g2_msyh");
#include "u8g2_msyh_font.h"

int main(void)
{
  sh1107dev _dev;

  // 设置全缓冲
  u8g2_SetupDisplay(&u8g2, au8x8_d_null_cb, au8x8_cad_empty, au8x8_byte_empty, au8x8_dummy_cb);
  // u8g2_SetupBuffer(&u8g2, buf, 16, u8g2_ll_hvline_vertical_top_lsb, U8G2_R0);
  u8g2_SetupBuffer(&u8g2, buf, 16, u8g2_ll_hvline_vertical_top_lsb, U8G2_R1);

  u8g2_InitDisplay(&u8g2);     // 初始化显示屏
  u8g2_SetPowerSave(&u8g2, 0); // 关闭屏幕节能模式
  u8g2_ClearBuffer(&u8g2);
  drawScreenInfo();
  display_show();
  while (1)
  {
    printf("u8g2_ClearBuffer \n");
    u8g2_ClearBuffer(&u8g2); // 清空缓冲区

    // 画一条线
    printf("u8g2_DrawLine \n");
    u8g2_DrawLine(&u8g2, 0, 0, 6, 6);

    u8g2_DrawLine(&u8g2, 10, 10, 20, 20);

    u8g2_DrawLine(&u8g2, 30, 30, 40, 40);

    u8g2_DrawLine(&u8g2, 50, 50, 60, 60);

    u8g2_DrawLine(&u8g2, 60, 50, 50, 60);

    u8g2_DrawLine(&u8g2, 32, 50, 32, 80);
    u8g2_DrawLine(&u8g2, 20, 50, 20, 128);

    u8g2_DrawFrame(&u8g2, 50, 10, 10, 10);

    u8g2_DrawBox(&u8g2, 70, 10, 10, 10);

    u8g2_DrawRFrame(&u8g2, 85, 10, 20, 20, 4);

    u8g2_DrawTriangle(&u8g2, 20, 5, 27, 50, 5, 32);
    printf("u8g2_SendBuffer \n");
    display_show();

    sleep(2);
    u8g2_ClearBuffer(&u8g2);
    u8g2_SetFont(&u8g2, au8g2_font_ncenB08_tr);
    // u8g2_SetFont(&u8g2, u8g2_msyh);
    u8g2_DrawStr(&u8g2, 0, 10, "Hello World!");
    u8g2_DrawStr(&u8g2, 0, 20, "This is bro cainiao!");
    u8g2_DrawStr(&u8g2, 0, 30, "Welcome to U8G2!");

    u8g2_SetFont(&u8g2, au8g2_font_unifont_t_symbols);
    u8g2_DrawStrX2(&u8g2, 5, 55, "anihao");

    // u8g2_SendBuffer(&u8g2); // 将缓冲区内容发送到显示屏

    u8g2_SetFont(&u8g2, au8g2_font_open_iconic_weather_6x_t);
    u8g2_DrawGlyph(&u8g2, 0, 48, 0x0045);

    display_show();

    sleep(2);
    u8g2_ClearBuffer(&u8g2);
    u8g2_SetFont(&u8g2, u8g2_font_unifont_t_chinese1);

    u8g2_SetFontDirection(&u8g2, 0);

    u8g2_DrawUTF8X2(&u8g2, 20, 40, "中文");

    display_show();
    sleep(2);
  }
  return 0;
}

// 程序使用注意，关于在字体库中u8g2的字体库相当大，编译出来大概有11m,所以尽量使用缩减字体。可以将字体复制出来单独定义，以减少程序的体积
