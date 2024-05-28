/*
* SPDX-FileCopyrightText: 2024 M5Stack Technology CO LTD
*
* SPDX-License-Identifier: MIT
*/
#include "hdmi_io.h"
// #include <stdio.h>
#include <assert.h>
#include "linux_i2c/linuxi2c.h"

LINUXI2CDevice i2cdev;

void HDMI_I2C_INIT(void)
{
	int ret;
	const char *lines = "/dev/i2c-0";
	unsigned char addr = 0x39;

	linuxi2c_init_device(&i2cdev);

	i2cdev.bus = linuxi2c_open(lines);
	if (i2cdev.bus == -1)
	{
		perror("Failed to open I2C bus");
		assert(i2cdev.bus != -1);
	}

	ret = linuxi2c_select(i2cdev.bus, addr, 0);
	if (ret != 0)
	{
		perror("Failed to select I2C device");
		assert(ret == 0);
	}

	i2cdev.addr = addr;
}
UINT8 HDMI_WriteI2C_Byte(UINT8 RegAddr, UINT8 d)
{
	linuxi2c_write(&i2cdev, RegAddr, &d, 1);
	return 1;
}
UINT8 HDMI_ReadI2C_Byte(UINT8 RegAddr)
{
	uint8_t data;
	linuxi2c_read(&i2cdev, RegAddr, &data, 1);
	return data;
}
void HDMI_I2C_DEINIT(void)
{
	// int ret;
	if (i2cdev.bus != -1)
	{
		linuxi2c_close(i2cdev.bus);
		i2cdev.bus = -1;
	}
}



void RESET_Lt8618SX(void)
{
	// clr_P03;
	// Delay_ms(50);
	// set_P03;
	// Delay_ms(100);
}



void Delay_ms(int ms)
{
	usleep(ms * 1000);
}


int Debug_s = 1;