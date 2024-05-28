/*
* SPDX-FileCopyrightText: 2024 M5Stack Technology CO LTD
*
* SPDX-License-Identifier: MIT
*/
#include <assert.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#include "linux_uart/linux_uart.h"

void print_usage() {
    printf(
        "Usage: rs485_reace -d </dev/ttySTM3> -b <115200>  [-t time_out] [-h]. "
        "-t 0 allway online.\n");
}

int main(int argc, char *argv[]) {
    int opt;
    char *dev_name = NULL;
    char *baud     = NULL;
    int time_out   = 0;
    int fd, ret;
    if (argc <= 1) {
        print_usage();
        exit(EXIT_SUCCESS);
    }

    while ((opt = getopt(argc, argv, "d:b:t:h")) != -1) {
        switch (opt) {
            case 'd':
                dev_name = optarg;
                break;
            case 'b':
                baud = optarg;
                break;
            case 't':
                time_out = abs(atoi(optarg) * 10);
                break;
            case 'h':
                print_usage();
                exit(EXIT_SUCCESS);
            default:
                print_usage();
                exit(EXIT_FAILURE);
        }
    }
    assert(dev_name != NULL);
    assert(baud != NULL);

    uart_t uart_parm = {.baud       = atoi(baud),
                        .data_bits  = 8,
                        .stop_bits  = 1,
                        .parity     = 'n',
                        .wait_flage = 1};
    fd               = linux_uart_init(dev_name, &uart_parm);
    if (fd > 0) {
        while ((usleep(100000), (time_out == 0 ? 1 : --time_out))) {
            char data = 0;
            int len;
            do {
                len = linux_uart_read(fd, 1, &data);
                if (len > 0) {
                    printf("0x%02x ", data);
                }
            } while (len > 0);
            if (data != 0) {
                printf("\n");
            }
        }
    }

    linux_uart_deinit(fd);

    return 0;
}
