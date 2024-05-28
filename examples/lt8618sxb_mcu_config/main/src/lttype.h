/*
* SPDX-FileCopyrightText: 2024 M5Stack Technology CO LTD
*
* SPDX-License-Identifier: MIT
*/
#ifndef _LT8618SX_TYPE_H
#define _LT8618SX_TYPE_H





typedef unsigned char UINT8;
typedef unsigned short UINT16;
typedef unsigned int UINT32;

typedef unsigned char uint8_t;
typedef unsigned short uint16_t;
typedef unsigned int uint32_t;

typedef unsigned char u8;
typedef unsigned short u16;
typedef unsigned int u32;

typedef volatile unsigned int vu32;
typedef volatile unsigned short vu16;
typedef volatile unsigned char vu8;

typedef enum
{ 
  Bit_RESET = 0,
  Bit_SET = !Bit_RESET
}BOOL;

typedef enum {
  Disable = 0,
  Enable = !Disable
}
FunctionalState;





#endif