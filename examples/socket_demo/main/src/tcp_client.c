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
        perror("Socket creation failed\n");
        exit(EXIT_FAILURE);
    }

    // Define the server address
    struct sockaddr_in server_addr;
    memset(&server_addr, 0, sizeof(server_addr));  // Clear the structure
    server_addr.sin_family =
        AF_INET;  // Set the address family to AF_INET (IPv4)
    server_addr.sin_addr.s_addr =
        inet_addr("127.0.0.1");           // Set the IP address to localhost
    server_addr.sin_port = htons(10000);  // Set the port number to 10000

    // Connect to the server
    if (connect(sockfd, (struct sockaddr *)&server_addr, sizeof(server_addr)) <
        0) {
        perror("Connection to the server failed\n");
        close(sockfd);
        exit(EXIT_FAILURE);
    }

    // Communication loop
    while (1) {
        char buf[1024];
        memset(&buf, 0, sizeof(buf));  // Clear the buffer

        // Read input from the standard input (keyboard)
        fgets(buf, sizeof(buf), stdin);

        // Send the input to the server
        ssize_t size = write(sockfd, buf, strlen(buf));
        if (size < 0) {
            perror("Write to socket failed\n");
            break;
        }
        printf("Client msg: %s\n", buf);

        // Clear the buffer
        memset(&buf, 0, sizeof(buf));

        // Read the server's response
        ssize_t read_size = read(
            sockfd, buf, sizeof(buf) - 1);  // Reserve space for null terminator
        if (read_size > 0) {
            printf("Server msg: %s\n", buf);
        } else if (read_size == 0) {
            printf("Server disconnected\n");
            break;
        } else {
            perror("Read from socket failed\n");
            break;
        }
    }

    // Close the socket
    close(sockfd);
    return 0;
}
