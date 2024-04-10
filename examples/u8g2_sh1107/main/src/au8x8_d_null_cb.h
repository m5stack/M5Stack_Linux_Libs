/*

  u8x8_d_bitmap.c

  a bitmap device

*/

#include "stdlib.h" /* malloc */
#include "stdint.h" /* uint16_t */
#include "string.h" /* memcpy */
#include "stdio.h"  /* FILE */
#include "u8g2.h"   /* because of u8g2_Setup... */

/*
 * This example uses the Bitmap device to draw to an in-memory buffer
 * and writes the result to a TGA file. This can for example be used to
 * make screenshots of an application normally running on an Arduino.
 */

// 用于定义显示信息
static const u8x8_display_info_t au8x8_null_display_info =
    {
        /* 芯片使能电平。1 表示高电平有效，0 表示低电平有效 = */ 0,
        /* 芯片失能电平。0 表示高电平有效，1 表示低电平有效 = */ 1,

        /* 芯片使能后的等待时间（纳秒），用于一些非常慢的显示设备 = */ 0,
        /* 芯片失能前的等待时间（纳秒），用于一些非常慢的显示设备 = */ 0,
        /* reset_pulse_width_ms = */ 0,
        /* post_reset_wait_ms = */ 0,
        /* sda_setup_time_ns = */ 0,
        /* sck_pulse_width_ns = */ 0,   /* half of cycle time (100ns according to datasheet), AVR: below 70: 8 MHz, >= 70 --> 4MHz clock */
        /* sck_clock_hz = */ 4000000UL, /* since Arduino 1.6.0, the SPI bus speed in Hz. Should be  1000000000/sck_pulse_width_ns */
        /* spi_mode = */ 0,             /* active high, rising edge */
        /* i2c_bus_clock_100kHz = */ 4,
        /* data_setup_time_ns = */ 0,
        /* write_pulse_width_ns = */ 0,
        /* tile_width = */ 8, /* 8x8 */
        /* tile_hight = */ 16,
        /* default_x_offset = */ 0,
        /* flipmode_x_offset = */ 0,
        /* pixel_width = */ 64,
        /* pixel_height = */ 128};
// 用于屏幕操作，设置屏幕信息，刷新屏幕等
uint8_t au8x8_d_null_cb(u8x8_t *u8x8, uint8_t msg, U8X8_UNUSED uint8_t arg_int, U8X8_UNUSED void *arg_ptr)
{
  uint8_t x, c;
  uint8_t *ptr;

  // printf("au8x8_d_null_cb type:%d \n", msg);
  switch (msg)
  {
  case U8X8_MSG_DISPLAY_SETUP_MEMORY: // 9
    u8x8_d_helper_display_setup_memory(u8x8, &au8x8_null_display_info);
    break;
  case U8X8_MSG_DISPLAY_INIT: // 10
    u8x8_d_helper_display_init(u8x8);
    break;
  case U8X8_MSG_DISPLAY_SET_POWER_SAVE: // 11
    break;
  case U8X8_MSG_DISPLAY_SET_FLIP_MODE: // 13
    break;
  case U8X8_MSG_DISPLAY_SET_CONTRAST: // 14
    break;
  case U8X8_MSG_DISPLAY_DRAW_TILE: // 15
  {
    // // 刷新
    // // u8x8_cad_StartTransfer(u8x8);
    // x = ((u8x8_tile_t *)arg_ptr)->x_pos;
    // x *= 8;
    // x += u8x8->x_offset;

    // // // // set column address
    // // // u8x8_cad_SendCmd(u8x8, 0x010 | (x >> 4));
    // // // u8x8_cad_SendCmd(u8x8, 0x000 | ((x & 15))); /* probably wrong, should be SendCmd */

    // // // // set page address
    // // // u8x8_cad_SendCmd(u8x8, 0x0b0 | (((u8x8_tile_t *)arg_ptr)->y_pos)); /* probably wrong, should be SendCmd */

    // // unsigned char buffer[2];
    // // buffer[0] = I2C_CONTROL_BYTE_CMD_SINGLE;
    // // buffer[1] = 0xb0 | (((u8x8_tile_t *)arg_ptr)->y_pos);		linux_i2c_write(i2cdev, buffer, 2, NULL, 0);
    // // buffer[1] = 0x00 | (x & 15);	linux_i2c_write(i2cdev, buffer, 2, NULL, 0);
    // // buffer[1] = 0x10 | (x >> 4);	linux_i2c_write(i2cdev, buffer, 2, NULL, 0);

    // printf("page:%d, columLow:%d, columHigh:%d\n", (((u8x8_tile_t *)arg_ptr)->y_pos), (x & 15), (x >> 4));

    // do
    // {
    //   c = ((u8x8_tile_t *)arg_ptr)->cnt;
    //   ptr = ((u8x8_tile_t *)arg_ptr)->tile_ptr;

    //   // unsigned char tmp[1024];
    //   // tmp[0] = I2C_CONTROL_BYTE_DATA_STREAM;
    //   // memcpy(tmp + 1, ptr, c * 8);
    //   // linux_i2c_write(i2cdev, tmp, c * 8 + 1, NULL, 0);

    //   printf("hex:%d\n", c * 8);
    //   for (int i = 0; i < c * 8; i++)
    //   {
    //     printf("%x ", ptr[i]);
    //   }
    //   printf("\n");

    //   // u8x8_cad_SendData(u8x8, c * 8, ptr); /* note: SendData can not handle more than 255 bytes */
    //   arg_int--;
    // } while (arg_int > 0);

    // // // u8x8_cad_EndTransfer(u8x8);
  }
  break;
  case U8X8_MSG_DISPLAY_REFRESH: // 16
  {
    return 1;
  }
  break;
  }
  /* the null device callback will succeed for all messages */
  return 1;
}
// 用于操作屏幕
uint8_t au8x8_cad_empty(u8x8_t *u8x8, uint8_t msg, uint8_t arg_int, void *arg_ptr)
{
  // printf("au8x8_cad_empty type:%d \n", msg);
  switch (msg)
  {
  case U8X8_MSG_CAD_SEND_CMD:
    // u8x8_byte_SendByte(u8x8, arg_int);
    break;
  case U8X8_MSG_CAD_SEND_ARG:
    // u8x8_byte_SendByte(u8x8, arg_int);
    break;
  case U8X8_MSG_CAD_SEND_DATA:
  case U8X8_MSG_CAD_INIT:
  case U8X8_MSG_CAD_START_TRANSFER:
  case U8X8_MSG_CAD_END_TRANSFER:
    // return u8x8->byte_cb(u8x8, msg, arg_int, arg_ptr);
    return 1;
  default:
    return 0;
  }
  return 1;
}
// 用于数据传输接口
uint8_t au8x8_byte_empty(U8X8_UNUSED u8x8_t *u8x8, uint8_t msg, U8X8_UNUSED uint8_t arg_int, U8X8_UNUSED void *arg_ptr)
{
  // printf("au8x8_byte_empty type:%d \n", msg);
  switch (msg)
  {
  case U8X8_MSG_BYTE_SEND:
  case U8X8_MSG_BYTE_INIT:
  case U8X8_MSG_BYTE_SET_DC:
  case U8X8_MSG_BYTE_START_TRANSFER:
  case U8X8_MSG_BYTE_END_TRANSFER:
    break; /* do nothing */
  }
  return 1; /* always succeed */
}
// 硬件的抽象控制
uint8_t au8x8_dummy_cb(U8X8_UNUSED u8x8_t *u8x8, U8X8_UNUSED uint8_t msg, U8X8_UNUSED uint8_t arg_int, U8X8_UNUSED void *arg_ptr)
{
  // printf("au8x8_dummy_cb type:%d \n", msg);
  /* the dummy callback will not handle any message and will fail for all messages */
  // U8X8_MSG_DELAY_NANO 44
  // U8X8_MSG_DELAY_10MICRO 42
  // U8X8_MSG_DELAY_100NANO 43
  // U8X8_MSG_DELAY_I2C 45
  // U8X8_MSG_GPIO_I2C_CLOCK 64 + 12 = 76
  // U8X8_MSG_GPIO_I2C_DATA 64 + 13 = 77
  // U8X8_MSG_GPIO_MENU_SELECT 64 + 16 = 80
  // U8X8_MSG_GPIO_MENU_NEXT 64 + 17 = 81
  // U8X8_MSG_GPIO_MENU_PREV 64 + 18 = 82
  // U8X8_MSG_GPIO_MENU_HOME 64 + 19 = 83
  // U8X8_MSG_GPIO_RESET
  // 相关调用
  // U8X8_MSG_GPIO_AND_DELAY_INIT 40
  // U8X8_MSG_GPIO_RESET 75
  // U8X8_MSG_DELAY_MILLI 41
  // U8X8_MSG_GPIO_RESET 75
  // U8X8_MSG_DELAY_MILLI 41
  // U8X8_MSG_GPIO_RESET 75
  // U8X8_MSG_DELAY_MILLI 41
  return 0;
}
