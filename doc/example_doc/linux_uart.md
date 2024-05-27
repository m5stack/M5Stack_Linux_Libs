## UART

UART (Universal Asynchronous Receiver-Transmitter) is a communication protocol and hardware interface used for data transfer between computers and external devices. It is a serial communication protocol used for data transmission between devices, such as computers and external devices like microcontrollers, sensors, modems, GPS modules, etc.

Key concepts about UART:

1. **Serial Communication:** UART transmits data in a serial manner, meaning data bits are sent one by one rather than in parallel, which saves the number of data lines.

2. **Asynchronous Communication:** UART is an asynchronous communication protocol, meaning the timing of transmission and reception is not synchronized. There is no fixed clock signal between data bits; instead, start and stop bits are used to indicate the beginning and end of each data frame.

3. **Data Frame:** UART data transmission is divided into data frames, each consisting of a start bit, data bits, optional parity bit, and one or more stop bits.

4. **Baud Rate:** Baud rate refers to the number of bits transmitted per second. In UART communication, both the sending and receiving devices must use the same baud rate to interpret the data correctly. Common baud rates include 9600, 115200, etc.

5. **Start and Stop Bits:** Each data frame starts with a start bit to indicate the beginning of the frame. Following the data bits are one or more stop bits used to mark the end of the data frame.

6. **Parity Bit (Optional):** The parity bit is used to detect errors in data transmission. Common parity methods include parity check and checksum.

7. **UART Hardware:** At the hardware level, UART is typically implemented by an integrated circuit called a UART chip, UART controller, or serial port controller. It includes circuits for sending and receiving data, FIFO buffers, baud rate generator, etc.

8. **RS-232:** Typically, UART communication uses the RS-232 serial communication standard. RS-232 defines details such as physical connections, voltage levels, and data formats.

9. **Serial Port:** UART is usually connected to external devices through a computer's serial port (also known as a COM port). However, serial port connections have become less common in modern computers, with USB and other interfaces being more prevalent.

examples:

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

When compiling the above program, you can compile and run it in the examples/linux_uart directory of [M5Stack_Linux_Libs]().  
Enter the Linux device:
``` bash
# Clone repository
git clone https://github.com/M5STACK/M5Stack_Linux_Libs.git

# Enter directory
cd M5Stack_Linux_Libs/examples/linux_uart

# Compile
scons 

```
