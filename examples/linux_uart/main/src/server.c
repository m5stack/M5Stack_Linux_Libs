#include "main.h"
#include "linux_uart/linux_uart.h"
#include <stdio.h>
#include <stdlib.h>

void server_main()
{
    int fd;
    char data[128];
    uart_t uart_parm = {
        .baud = 115200,
        .data_bits = 8,
        .stop_bits = 1,
        .parity = 'n'
    };
    fd = linux_uart_init(uart_dev, &uart_parm);
    if(fd < 0)
    {
        LOG_PRINT("dev: %s open fail!", uart_dev);
        abort();
    }
    for(;;){
        if(linux_uart_read(fd, 128, data)){
            if(debugging){
                LOG_PRINT("ttyS2 reace:%s  \n", data);
            }
            linux_uart_write(fd, 128, data);
        }
    }
}