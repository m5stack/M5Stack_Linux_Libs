/*
* SPDX-FileCopyrightText: 2024 M5Stack Technology CO LTD
*
* SPDX-License-Identifier: MIT
*/
#ifndef _LT8618SX_IO_H
#define _LT8618SX_IO_H

#include"lttype.h"
#include <stdio.h>
// #include <stdint.h>
#include <unistd.h>
// #include <stdbool.h>
// #include <assert.h>

void HDMI_I2C_INIT(void);
UINT8 HDMI_WriteI2C_Byte( UINT8 RegAddr, UINT8 d );
UINT8 HDMI_ReadI2C_Byte( UINT8 RegAddr );
void HDMI_I2C_DEINIT(void);
void RESET_Lt8618SX(void);
void Delay_ms(int ms);







extern int Debug_s;


#define LOG_PRINT(fmt, ...)                                                                       \
	do                                                                                            \
	{                                                                                             \
		if (Debug_s)                                                                              \
		{                                                                                         \
			printf(fmt "  [info:%s:%d] [%s]\n", ##__VA_ARGS__, __FILE__, __LINE__, __FUNCTION__); \
		}                                                                                         \
	} while (0);

// #define Debug_Printf printf
#define Debug_DispNum(data) printf("%d\n", data)


// Debug_DispStrNum



#endif