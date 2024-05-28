/*
* SPDX-FileCopyrightText: 2024 M5Stack Technology CO LTD
*
* SPDX-License-Identifier: MIT
*/
#include <stdio.h>
#include <assert.h>
#include <stdint.h>
#include <string.h>
#include "linux_i2c/linuxi2c.h"

LINUXI2CDevice i2cdev;

int main(int argc, char *argv[])
{
	char i2c_bus[20] = "/dev/i2c-1";
	linuxi2c_init_device(&i2cdev);
	i2cdev.bus = linuxi2c_open(i2c_bus);
	i2cdev.addr = 0x38;

	uint8_t data[] = {0xaa, 0xbb, 0xcc, 0xdd, 0xee, 0xff, 0x01, 0x02, 0x03};

	// linuxi2c_write(&i2cdev, 0x05, data, sizeof(data));
	linuxi2c_primitive_write(&i2cdev, data, 1, data, sizeof(data));

	linuxi2c_close(i2cdev.bus);

	return 0;
}