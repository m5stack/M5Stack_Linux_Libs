#include <stdio.h>
#include <linux_i2c/linuxi2c.h>
#include <stdlib.h>
#include <assert.h>
// Define a bus object
LINUXI2CDevice i2cdev;
int main(int argc,char *argv[])
{
    char i2c_bus[20] = "/dev/i2c-1";
    int ret;
    // init bus object
    linuxi2c_init_device(&i2cdev);
    // open bus
    i2cdev.bus = linuxi2c_open(i2c_bus);
    if(i2cdev.bus <= 0)
    {
        printf("i2c bus: %s open fail!\b", i2c_bus);
        abort();
    }
    char data = 0x01;
    // write data
    ret = linuxi2c_write(&i2cdev, 0x3c, &data, 1);
    if(ret < 0)
    {
        goto end;
    }
    data = 0;
    // read data
    ret = linuxi2c_read(&i2cdev, 0x3c, &data, 1);
    if(ret < 0)
    {
        goto end;
    }
    printf("get i2c bus:%s addr:0x3c val:%x\n", i2c_bus, data);
    end:
        // Release the i2c bus
        linuxi2c_close(i2cdev.bus);

    return 0;
}