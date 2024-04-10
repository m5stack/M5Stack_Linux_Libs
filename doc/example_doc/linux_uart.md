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
