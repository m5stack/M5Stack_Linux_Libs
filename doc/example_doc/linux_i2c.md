## I2C
I2C（Inter-Integrated Circuit）总线，这是一种常见的串行通信协议，用于连接多个微控制器、传感器、芯片等外部设备。

以下是初学者需要了解的关于I2C的重要概念：

1. **串行通信：** I2C是一种串行通信协议，它允许多个设备在同一根线上进行通信，这极大地减少了连接线的数量。

2. **双线制：** I2C使用两根线进行通信：SCL（Serial Clock Line）和SDA（Serial Data Line）。SCL用于传递时钟信号，而SDA用于传输数据。

3. **主从结构：** I2C通信通常涉及两种类型的设备：主设备和从设备。主设备生成时钟信号并控制通信，而从设备则响应主设备的指令。

4. **地址分配：** 每个从设备在总线上都有一个唯一的7位地址，主设备使用这些地址来选择特定的从设备进行通信。

5. **帧格式：** I2C通信由一系列的数据帧组成。每个帧通常包括一个起始位、一个地址字节、数据字节和一个停止位。

6. **时钟同步：** I2C协议使用主设备生成的时钟信号来同步通信，确保数据在正确的时间传输。

7. **速率：** I2C通信速率可以根据需要设置。标准模式（100 Kbps）和快速模式（400 Kbps）是常见的速率。

8. **应用领域：** I2C在嵌入式系统、传感器、存储设备、显示屏、电子芯片等领域得到广泛应用。

linux 相关例程:

```c
#include <stdio.h>
#include <linux_i2c/linuxi2c.h>
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
```

在编译上述程序时可以在 [M5Stack_Linux_Libs]() 的 examples/linux_i2c 目录下编译并运行.  
进入 linux 设备
``` bash
# 克隆仓库
git clone https://github.com/M5STACK/M5Stack_Linux_Libs.git

# 进入目录
cd M5Stack_Linux_Libs/examples/linux_i2c

# 编译
make 

#在 dist 目录下生成 linux_i2c 可执行程序.
```
