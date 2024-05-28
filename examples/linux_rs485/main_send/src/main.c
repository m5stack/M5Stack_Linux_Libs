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
    printf("Usage: rs485_send -d </dev/ttySTM3> -b <115200>  <msg> [-h]\n");
}

int main(int argc, char *argv[]) {
    int opt;
    char *dev_name = NULL;
    char *baud     = NULL;
    int fd, ret;
    if (argc <= 1) {
        print_usage();
        exit(EXIT_SUCCESS);
    }

    while ((opt = getopt(argc, argv, "d:b:h")) != -1) {
        switch (opt) {
            case 'd':
                dev_name = optarg;
                break;
            case 'b':
                baud = optarg;
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
    assert((argc - optind) > 0);

    uart_t uart_parm = {
        .baud = atoi(baud), .data_bits = 8, .stop_bits = 1, .parity = 'n'};
    fd = linux_uart_init(dev_name, &uart_parm);
    if (fd > 0) {
        linux_uart_write(fd, strlen(argv[optind]), (uint8_t *)argv[optind]);
    }
    linux_uart_deinit(fd);

    return 0;
}
