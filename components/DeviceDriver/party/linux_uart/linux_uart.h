/*
* Copyright(C), 2020-2023, Red Hat Inc.
* File name: 
* Author: dianjixz
* Version: v 1.0.0
* Date: 
* Description: 
* Function List: 
* History: 
*/


#ifndef __LINUX_UART_H
#define __LINUX_UART_H

#include "stdint.h"
#if __cplusplus
extern "C"
{
#endif
#define PRINF_HEX_ARR(str,buf,len)\
do{\
    char *buff = (char *)buf;\
    printf("\e[32m[%s](%d):\e[0m", str, len);\
    for (int i = 0;i < len; ++i)\
    {\
        printf("0x%.2X ", buff[i] & 0xff);\
    }\
    printf("\r\n");\
} while (0);


typedef struct{
	int baud;              // UART baud rate. 115200
	int data_bits;         // UART data bits. 8
	int stop_bits;         // UART stop bits. 1
	char parity;           // parity.         'n'
    int wait_flage;        // wait!            0
}uart_t;

/**
 * @brief init uart
 * @note 
 * @param [in] dev    device name
 * @param [in] param  struct uart_t pointer
 * @retval [return] [>0] fd index
 * @retval [return] [<=0] fail code
 */
int linux_uart_init(char* dev, void* param);

/**
 * @brief close uart
 * @note 
 * @param [in] fd    fd index
 * @retval 
 */
void linux_uart_deinit(int fd);

/**
 * @brief Read data from UART.
 * @note 
 * @param [in] fd    fd index
 * @param [in] cnt    buff size
 * @param [in] buf    buff pointer
 * @retval [return] [>0] The size of the read data.
 * @retval [return] [<0] Read failure code.
 */
int linux_uart_read(int fd, int cnt, void* buf);

/**
 * @brief Send data from UART.
 * @note 
 * @param [in] fd    fd index
 * @param [in] cnt    buff size
 * @param [in] buf    buff pointer
 * @retval [return] [>0] The size of the sent data.
 * @retval [return] [<0] Send failure code.
 */
int linux_uart_write(int fd, int cnt, void* buf);

#if __cplusplus
}
#endif

#endif /* __LINUX_UART_H */
