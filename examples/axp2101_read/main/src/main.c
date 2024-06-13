/*
* SPDX-FileCopyrightText: 2024 M5Stack Technology CO LTD
*
* SPDX-License-Identifier: MIT
*/
#include <assert.h>
#include <linux_i2c/linuxi2c.h>
#include <math.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
// Define a bus object
LINUXI2CDevice i2cdev;

int main(int argc, char *argv[]) {
    char i2c_bus[20];
    // CoreMP135 axp2101 in /dev/i2c-0
    if (argc > 1)
        strcpy(i2c_bus, argv[1]);
    else
        sprintf(i2c_bus, "/dev/i2c-0");
    int ret;
    // init bus object
    linuxi2c_init_device(&i2cdev);
    // open bus
    i2cdev.bus = linuxi2c_open(i2c_bus);
    if (i2cdev.bus <= 0) {
        printf("i2c bus: %s open fail!\b", i2c_bus);
        abort();
    }
    i2cdev.addr = 0x34;

    uint8_t data[2];

    data[0] = 0xff;
    linuxi2c_write(&i2cdev, 0x30, &data, 1);
    data[0] = 0x1E;
    linuxi2c_write(&i2cdev, 0x50, &data, 1);

    while (1) {
        int reg = 0x34;
        linuxi2c_read(&i2cdev, reg, &data, 2);
        float vbat = (((data[0] & 0x3f) << 8) | data[1]) * 1.0f;
        printf("vbat: \t%.02fV \n", (vbat / 1000 * 1.0f));

        reg += 2;
        linuxi2c_read(&i2cdev, reg, &data, 2);
        float ts = (((data[0] & 0x3f) << 8) | data[1]) * 0.5;
        ts =
            1 / (log((ts / 1000.0 / 50e-6) / 10e3) / 3428 + 1 / (273.15 + 25)) -
            273.15;
        printf("ts: \t%.02f°C \n", ts);

        reg += 2;
        linuxi2c_read(&i2cdev, reg, &data, 2);
        float vbus = (((data[0] & 0x3f) << 8) | data[1]) * 1.0f;
        printf("vbus: \t%.02fV \n", (vbus / 1000.0));

        reg += 2;
        linuxi2c_read(&i2cdev, reg, &data, 2);
        float vsys = (((data[0] & 0x3f) << 8) | data[1]) * 1.0f;
        printf("vsys: \t%.02fV \n", (vsys / 1000.0));

        reg += 2;
        linuxi2c_read(&i2cdev, reg, &data, 2);
        float tdie = (22 + ((7274 - ((data[0] << 8) | data[1])) / 20)) * 1.0f;
        printf("tdie: \t%.02f°C \n", (tdie));
        printf("-------------------\n");
        sleep(1);
    }

    linuxi2c_close(i2cdev.bus);

    return 0;
}