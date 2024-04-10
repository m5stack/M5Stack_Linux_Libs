# 总线
总线是一项伟大的发明，总线的伟大在与它可以使很多的器件按照特定的方式联系在一起从而共同的协作从而建立起现代的各中设备。在计算机建立的时候科学家说要有总线，于是便有了总线。

总线在不同的系统中有着不同的含义，从这个词本身出发，总线就是集总的连接的意思。在本文中，着重体现总线在不同硬件的不同连接通信方式。

在硬件层面，总线有很多中，其中 uart、i2c、spi、can、ir 是我们经常使用的总线，在本文中，我们将着重讲述这几种总线。

## UART

UART（Universal Asynchronous Receiver-Transmitter）是一种通信协议和硬件接口，用于在计算机和外部设备之间传输数据。它是一种串行通信协议，用于在设备之间进行数据传输，如在计算机和微控制器、传感器、调制解调器、GPS模块等外部设备之间。

关于UART的重要概念：

1. **串行通信：** UART使用串行方式传输数据，这意味着数据位逐个传输，而不是并行传输，从而节省了数据线的数量。

2. **异步通信：** UART是异步通信协议，这意味着发送和接收的时钟不同步。数据位之间没有固定的时钟信号，而是使用起始位和停止位来标识每个数据帧的开始和结束。

3. **数据帧：** UART数据传输被划分为数据帧，其中每个帧由一个起始位、数据位、可选的校验位和一个或多个停止位组成。

4. **波特率：** 波特率是指每秒钟传输的比特数。在UART通信中，发送和接收设备必须使用相同的波特率才能正确地解释数据。常见的波特率包括9600、115200等。

5. **起始位和停止位：** 每个数据帧都以一个起始位开始，用于指示数据帧的开始。之后是数据位，然后是一个或多个停止位，用于标识数据帧的结束。

6. **校验位（可选）：** 校验位用于检测数据传输中的错误。常见的校验方式包括奇偶校验和校验和。

7. **UART硬件：** 在硬件层面，UART通常由一个称为UART芯片、UART控制器或串口控制器的集成电路来实现。它包括用于发送和接收数据的电路、FIFO缓冲区、波特率发生器等。

8. **RS-232：** 通常情况下，UART通信使用RS-232串行通信标准。RS-232定义了物理连接、电平以及数据格式等细节。

9. **串口：** UART通常通过计算机的串口（也称为COM端口）与外部设备连接。然而，在现代计算机中，串口连接变得不太常见，更常见的是使用USB和其他接口。

linux 相关例程

```c
#include "linux_uart/linux_uart.h"
#ifdef HAVE_MAIN
#include "test.h"
#else
#include <stdio.h>
int Debug_s = 1;
#define LOG_PRINT(fmt, ...) do{\
	if(Debug_s)\
	{\
		printf(fmt"  [info:%s:%d] [%s]\n", ##__VA_ARGS__, __FILE__, __LINE__, __FUNCTION__);\
	}\
}while(0);
void main_clint();

// open device "/dev/ttyS1"
char uart_dev[20] = "/dev/ttyS1";
int main(int argc,char *argv[])
{
    main_clint();
    return 0;
}
#endif
void main_clint()
{
    // open id index
    int fd, ret;
    char data[128];
    // uart parm 115200,8,1,0
    uart_t uart_parm = {
        .baud = 115200,
        .data_bits = 8,
        .stop_bits = 1,
        .parity = 'n'
    };
    // open dev and init
    fd = linux_uart_init(uart_dev, &uart_parm);
    if(fd < 0)
    {
        LOG_PRINT("dev: %s open fail!", uart_dev);
        abort();
    }
    sprintf(data, "test");
    // send data, Blocking function
    ret = linux_uart_write(fd, strlen(data), data);
    if(ret < 0)
    {
        LOG_PRINT("dev: %s write fail!", uart_dev);
        abort();
    }
    memset(data, 0, 128);
    // reace data, Blocking function
    linux_uart_read(fd, 128, data);

    if(strcmp(data, "test") == 0)
    {
        printf("uart test success!\n");
    }
    else
    {
        printf("uart test fail!\n");
    }
    linux_uart_deinit(fd);
}

```

在编译上述程序时可以在 [M5Stack_Linux_Libs]() 的 examples/linux_uart 目录下编译并运行.  
进入 linux 设备
``` bash
# 克隆仓库
git clone https://github.com/M5STACK/M5Stack_Linux_Libs.git

# 进入目录
cd M5Stack_Linux_Libs/examples/linux_uart

# 编译
make 

#在 dist 目录下生成 linux_uart 可执行程序.
```


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

## SPI
SPI（Serial Peripheral Interface），这是一种常见的串行通信协议，用于在微控制器、传感器、存储设备等外部设备之间进行高速数据传输。

以下是初学者需要了解的关于SPI的重要概念：

1. **串行通信：** SPI是一种串行通信协议，它允许设备在同一时间内传输多个比特的数据，从而实现高速数据传输。

2. **主从结构：** SPI通信涉及两种类型的设备：主设备和从设备。主设备生成时钟信号并控制通信，而从设备响应主设备的指令。

3. **通信线：** SPI使用多条线进行通信：
   - SCLK（Serial Clock）：主设备生成的时钟信号，用于同步数据传输。
   - MOSI（Master Out Slave In）：主设备向从设备发送数据的线路。
   - MISO（Master In Slave Out）：从设备向主设备发送数据的线路。
   - SS/CS（Slave Select/Chip Select）：用于选择从设备的线路，允许在多个设备之间进行通信。

4. **时钟同步：** SPI通信中的数据传输是同步的，即数据是根据时钟信号进行传输的。

5. **帧格式：** SPI通信由一系列的数据帧组成。数据传输通常在每个时钟周期中都会发生，允许高速传输。

6. **速率：** SPI通信速率可以非常高，因为数据传输是同步的。速率可以根据需要设置。

7. **全双工：** SPI是一种全双工通信协议，这意味着在同一时间内可以同时进行双向数据传输。

8. **应用领域：** SPI广泛应用于嵌入式系统、存储设备、传感器、显示屏、通信设备等领域。

linux 相关例程:

```c
//-----------------------------------------------------------------------------
#include "linux_spi/linux_spi.h"
#include <stdio.h>
//-----------------------------------------------------------------------------
#define SPI_DEVICE "/dev/spidev0.0"
//-----------------------------------------------------------------------------
int main()
{
  spi_t spi;
  char buf[1024];
  int i;

  int retv = spi_init(&spi,
                      SPI_DEVICE, // filename like "/dev/spidev0.0"
                      0,          // SPI_* (look "linux/spi/spidev.h")
                      0,          // bits per word (usually 8)
                      2500000);   // max speed [Hz]

  printf(">>> spi_init() return %d\n", retv);

  retv = spi_read(&spi, buf, 1024);
  printf(">>> spi_read(1024) return %d\n", retv);

  for (i = 0; i < 1024; i++)
    buf[i] = 0x55;

  retv = spi_write(&spi, buf, 1024);
  printf(">>> spi_write(1024) return %d\n", retv);

  spi_free(&spi);

  return 0;
}

```

在编译上述程序时可以在 [M5Stack_Linux_Libs]() 的 examples/linux_spi 目录下编译并运行.  
进入 linux 设备
``` bash
# 克隆仓库
git clone https://github.com/M5STACK/M5Stack_Linux_Libs.git

# 进入目录
cd M5Stack_Linux_Libs/examples/linux_spi

# 编译
make 

#在 dist 目录下生成 linux_spi 可执行程序.
```

# CAN
CAN（Controller Area Network），这是一种常见的实时通信协议，用于在汽车、工业自动化、机器人和其他领域中进行高速、可靠的数据通信。

关于CAN的重要概念：

1. **多主多从架构：** CAN通信是多主多从的通信架构，多个设备可以同时传输和接收数据，从而实现高效的分布式通信。

2. **帧格式：** CAN通信使用帧格式来传输数据。基本帧由标识符、数据、控制位和CRC（循环冗余校验）组成。

3. **标识符：** 每个CAN帧都有一个唯一的标识符，用于标识消息的类型和优先级。标识符由11位或29位组成，决定了消息的优先级。

4. **数据率：** CAN支持多种数据传输率，例如125 Kbps、500 Kbps、1 Mbps等，您可以根据需要进行选择。

5. **冲突检测：** CAN使用CSMA/CR（Carrier Sense Multiple Access / Collision Resolution）机制来检测和解决数据冲突。

6. **错误检测和纠正：** CAN使用CRC来检测错误，并且具有一些错误纠正的能力，确保可靠的数据传输。

7. **广播和单播：** CAN支持广播消息，使得所有设备都能收到消息。同时，也支持通过标识符进行单播，只有特定设备会接收消息。

8. **应用领域：** CAN通信广泛应用于汽车内部网络、工业自动化、航空航天、机器人和医疗设备等领域。

CAN 在 linux 被抽象成了一个网络设备,所以对 CAN 总线的编程被称为[SocketCAN](https://en.wikipedia.org/wiki/SocketCAN).

在linux 中使用can 首先要设置can的通信速率,然后开启can总线
```bash
# 查看 CAN 接口
sudo ip link list

# 设置 CAN 接口速率为 500000
sudo ip link set can0 type can bitrate 500000

# 开启 CAN 接口
sudo ip link set up can0

# 关闭 CAN 接口
sudo ip link set down can0
```


为了展示 CAN 编程,创建一个虚拟的 CAN 总线.
```bash
sudo modprobe can
sudo modprobe can_raw
sudo modprobe vcan
sudo ip link add dev vcan0 type vcan
sudo ip link set up vcan0
sudo ip link show vcan0

```

然后在 C 程序中读写 CAN 总线.
``` C
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>

#include <net/if.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <sys/ioctl.h>

#include <linux/can.h>
#include <linux/can/raw.h>

int main(void)
{
    int s;
    int nbytes;
    struct sockaddr_can addr;
    struct can_frame frame;
    struct ifreq ifr;

    const char *ifname = "vcan0";

    if ((s = socket(PF_CAN, SOCK_RAW, CAN_RAW)) == -1)
    {
        perror("Error while opening socket");
        return -1;
    }

    strcpy(ifr.ifr_name, ifname);
    ioctl(s, SIOCGIFINDEX, &ifr);

    addr.can_family = AF_CAN;
    addr.can_ifindex = ifr.ifr_ifindex;

    printf("%s at index %d\n", ifname, ifr.ifr_ifindex);

    if (bind(s, (struct sockaddr *)&addr, sizeof(addr)) == -1)
    {
        perror("Error in socket bind");
        return -2;
    }

    frame.can_id = 0x123;
    frame.can_dlc = 2;
    frame.data[0] = 0x11;
    frame.data[1] = 0x22;

    nbytes = write(s, &frame, sizeof(struct can_frame));

    printf("Wrote %d bytes\n", nbytes);

    return 0;
}

```
可以使用 candump 实用程序在 vcan0 接口上分析数据包，该实用程序是 SocketCAN can-utils 包的一部分。
如果没有安装 can-utils,可以使用下面的命令安装.
``` bsah
sudo apt install can-utils
```
首先在一个终端中运行命令:
``` bash
candump vcan0
```
然后另起一个终端编译运行上述程序,然后就能在 运行 candump 命令的终端看到:
``` bash
  vcan0  123   [2]  11 22
```
看完信息可以按 `ctrl + c` 结束 candump 程序的运行.

在编译上述程序时可以在 [M5Stack_Linux_Libs]() 的 examples/linux_can 目录下编译并运行.
``` bash
# 克隆仓库
git clone https://github.com/M5STACK/M5Stack_Linux_Libs.git

# 进入目录
cd M5Stack_Linux_Libs/examples/linux_can

# 编译
make 

# 运行
make run

推荐：
    https://zhuanlan.zhihu.com/p/470681140?utm_id=0
```
