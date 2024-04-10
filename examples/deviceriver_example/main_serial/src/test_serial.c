#include "linux_uart/linux_uart.h"
#include <stdlib.h>
#include <string.h>
#include "main.h"

#include <stdio.h>
int Debug_s = 1;
#define LOG_PRINT(fmt, ...)                                                                       \
    do                                                                                            \
    {                                                                                             \
        if (Debug_s)                                                                              \
        {                                                                                         \
            printf(fmt "  [info:%s:%d] [%s]\n", ##__VA_ARGS__, __FILE__, __LINE__, __FUNCTION__); \
        }                                                                                         \
    } while (0);
void main_clint();
char uart_dev[20] = "/dev/ttyS1";
int main(int argc, char *argv[])
{
    main_clint();
    return 0;
}

void main_clint()
{
    int fd, ret;
    char data[128];
    uart_t uart_parm = {
        .baud = 115200,
        .data_bits = 8,
        .stop_bits = 1,
        .parity = 'n'};
    fd = linux_uart_init(uart_dev, &uart_parm);
    if (fd < 0)
    {
        LOG_PRINT("dev: %s open fail!", uart_dev);
        abort();
    }
    sprintf(data, "test");
    ret = linux_uart_write(fd, strlen(data), (uint8_t *)data);
    if (ret < 0)
    {
        LOG_PRINT("dev: %s write fail!", uart_dev);
        abort();
    }
    memset(data, 0, 128);
    linux_uart_read(fd, 128, data);
    if (strcmp(data, "test") == 0)
    {
        printf("uart test success!\n");
    }
    else
    {
        printf("uart test fail!\n");
    }
    linux_uart_deinit(fd);
}
