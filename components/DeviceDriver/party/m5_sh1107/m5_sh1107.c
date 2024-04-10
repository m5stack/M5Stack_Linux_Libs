
#include "m5_sh1107.h"

#include <assert.h>
#include <stdio.h>
#include <stdint.h>
LINUXI2CDevice i2cdev;

int m5_sh1107_dev_deinit()
{
    linuxi2c_close(i2cdev.bus);
    return 0;
}

int m5_sh1107_dev_init(int num)
{
	linuxi2c_init_device(&i2cdev);
	char device_name[20];
	sprintf(device_name, "/dev/i2c-%d", num);
	i2cdev.bus = linuxi2c_open(device_name);
	i2cdev.addr = 0x3c;

    if(i2cdev.bus <= 0 )
    {
        printf("m5_sh1107_dev_init i2c init faile!\n");
        return -1;
    }
    unsigned char buffer[2];
    buffer[0] = I2C_CONTROL_BYTE_CMD_SINGLE;
	buffer[1] = 0xAE;   linuxi2c_primitive_write(&i2cdev, NULL, 0, buffer, 2);
	buffer[1] = 0x21;   linuxi2c_primitive_write(&i2cdev, NULL, 0, buffer, 2);
	buffer[1] = 0xDC;   linuxi2c_primitive_write(&i2cdev, NULL, 0, buffer, 2);
	buffer[1] = 0x00;   linuxi2c_primitive_write(&i2cdev, NULL, 0, buffer, 2);
	buffer[1] = 0xA0;   linuxi2c_primitive_write(&i2cdev, NULL, 0, buffer, 2);
	buffer[1] = 0xC8;   linuxi2c_primitive_write(&i2cdev, NULL, 0, buffer, 2);
	buffer[1] = 0xA8;   linuxi2c_primitive_write(&i2cdev, NULL, 0, buffer, 2);
	buffer[1] = 0x7F;   linuxi2c_primitive_write(&i2cdev, NULL, 0, buffer, 2);
	buffer[1] = 0xD3;   linuxi2c_primitive_write(&i2cdev, NULL, 0, buffer, 2);
	buffer[1] = 0x60;   linuxi2c_primitive_write(&i2cdev, NULL, 0, buffer, 2);
	buffer[1] = 0xD5;   linuxi2c_primitive_write(&i2cdev, NULL, 0, buffer, 2);
	buffer[1] = 0x50;   linuxi2c_primitive_write(&i2cdev, NULL, 0, buffer, 2);
	buffer[1] = 0xD9;   linuxi2c_primitive_write(&i2cdev, NULL, 0, buffer, 2);
	buffer[1] = 0xF1;   linuxi2c_primitive_write(&i2cdev, NULL, 0, buffer, 2);
	buffer[1] = 0xDB;   linuxi2c_primitive_write(&i2cdev, NULL, 0, buffer, 2);
	buffer[1] = 0x35;   linuxi2c_primitive_write(&i2cdev, NULL, 0, buffer, 2);
	buffer[1] = 0xAD;   linuxi2c_primitive_write(&i2cdev, NULL, 0, buffer, 2);
	buffer[1] = 0x81;   linuxi2c_primitive_write(&i2cdev, NULL, 0, buffer, 2);
	buffer[1] = 0x81;   linuxi2c_primitive_write(&i2cdev, NULL, 0, buffer, 2);
	buffer[1] = 0x80;   linuxi2c_primitive_write(&i2cdev, NULL, 0, buffer, 2);
	buffer[1] = 0xA4;   linuxi2c_primitive_write(&i2cdev, NULL, 0, buffer, 2);
	buffer[1] = 0xA6;   linuxi2c_primitive_write(&i2cdev, NULL, 0, buffer, 2);
	buffer[1] = 0xAF;   linuxi2c_primitive_write(&i2cdev, NULL, 0, buffer, 2);
    return 0;
}

int m5_sh1107_dev_set_img(int page, int seg, void * images, int width)
{
	uint8_t columLow = seg & 0x0F;
	uint8_t columHigh = (seg >> 4) & 0x0F;

    unsigned char buffer[2];
    buffer[0] = I2C_CONTROL_BYTE_CMD_SINGLE;
	buffer[1] = 0xB0 | page;		linuxi2c_primitive_write(&i2cdev, buffer, 2, NULL, 0);
	buffer[1] = 0x00 + columLow;	linuxi2c_primitive_write(&i2cdev, buffer, 2, NULL, 0);
	buffer[1] = 0x10 + columHigh;	linuxi2c_primitive_write(&i2cdev, buffer, 2, NULL, 0);

    unsigned char tmp[1024];
    tmp[0] = I2C_CONTROL_BYTE_DATA_STREAM;
    memcpy(tmp + 1, images, width);
    linuxi2c_primitive_write(&i2cdev, tmp, width + 1, NULL, 0);
    return 0;
}

