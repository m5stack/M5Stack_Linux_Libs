/*
 * SPDX-FileCopyrightText: 2024 M5Stack Technology CO LTD
 *
 * SPDX-License-Identifier: MIT
 */
#include <arpa/inet.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <unistd.h>

int main() {
    // Create a socket
    int sockfd = socket(AF_INET, SOCK_STREAM, 0);
    if (sockfd < 0) {
        perror("socket creation failed\n");
        exit(EXIT_FAILURE);
    }

    // Define server address
    struct sockaddr_in ser;
    bzero(&ser, sizeof(ser));
    ser.sin_family      = AF_INET;                 // IPv4
    ser.sin_addr.s_addr = inet_addr("127.0.0.1");  // Localhost IP address
    ser.sin_port        = htons(10000);            // Port number

    // Bind the socket to the IP address and port
    if (bind(sockfd, (struct sockaddr *)&ser, sizeof(ser)) < 0) {
        perror("bind failed\n");
        close(sockfd);
        exit(EXIT_FAILURE);
    }

    // Listen for incoming connections
    if (listen(sockfd, 100) < 0) {
        perror("listen failed\n");
        close(sockfd);
        exit(EXIT_FAILURE);
    }

    // Accept a connection
    struct sockaddr_in cli;
    socklen_t len = sizeof(cli);
    bzero(&cli, sizeof(cli));
    int clifd = accept(sockfd, (struct sockaddr *)&cli, &len);
    if (clifd < 0) {
        perror("accept failed\n");
        close(sockfd);
        exit(EXIT_FAILURE);
    }

    // Print client information
    printf("New Client fd: %d, IP: %s, PORT: %d\n", clifd,
           inet_ntoa(cli.sin_addr), ntohs(cli.sin_port));

    // Read and write data
    while (1) {
        char buf[1024];
        bzero(&buf, sizeof(buf));
        ssize_t size = read(clifd, buf, sizeof(buf));
        if (size > 0) {
            // Print client message
            printf("Client %d msg: %s\n", clifd, buf);
            bzero(&buf, sizeof(buf));
            // Read input from STDIN
            fgets(buf, sizeof(buf), stdin);
            // Write data back to client
            write(clifd, buf, sizeof(buf));
        } else if (size == 0) {
            // Connection closed by client
            printf("Client disconnected\n");
            close(clifd);
            break;
        } else if (size == -1) {
            // Read error
            perror("read error");
            close(clifd);
            break;
        }
    }

    // Close the server socket
    close(sockfd);
    return 0;
}
