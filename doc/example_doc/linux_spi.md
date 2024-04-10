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