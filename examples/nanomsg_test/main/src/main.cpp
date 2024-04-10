// #include <stdlib.h>
// #include <stdio.h>
// #include <string.h>
// #include <unistd.h>
// #include <nanomsg/nn.h>
// #include <nanomsg/pipeline.h>

// #define NODE0 "node0"
// #define NODE1 "node1"

// void
// fatal(const char *func)
// {
//         fprintf(stderr, "%s: %s\n", func, nn_strerror(nn_errno()));
//         exit(1);
// }

// int
// node0(const char *url)
// {
//         int sock;
//         int rv;
//         printf("url: \"%s\"\n", url); 
//         if ((sock = nn_socket(AF_SP, NN_PULL)) < 0) {
//                 fatal("nn_socket");
//         }
//         if ((rv = nn_bind(sock, url)) < 0) {
//                 fatal("nn_bind");
//         }
//         for (;;) {
//                 char *buf = NULL;
//                 int bytes;
//                 if ((bytes = nn_recv(sock, &buf, NN_MSG, 0)) < 0) {
//                         fatal("nn_recv");
//                 }
//                 printf("NODE0: RECEIVED \"%s\"\n", buf); 
//                 nn_freemsg(buf);
//         }
// }

// int
// node1(const char *url, const char *msg)
// {
//         int sz_msg = strlen(msg) + 1; // '\0' too
//         int sock;
//         int rv;
//         int bytes;

//         if ((sock = nn_socket(AF_SP, NN_PUSH)) < 0) {
//                 fatal("nn_socket");
//         }
//         if ((rv = nn_connect(sock, url)) < 0) {
//                 fatal("nn_connect");
//         }
//         printf("NODE1: SENDING \"%s\"\n", msg);
//         if ((bytes = nn_send(sock, msg, sz_msg, 0)) < 0) {
//                 fatal("nn_send");
//         }
//         sleep(1); // wait for messages to flush before shutting down
//         return (nn_shutdown(sock, 0));
// }

// int
// main(const int argc, const char **argv)
// {
//         if ((argc > 1) && (strcmp(NODE0, argv[1]) == 0))
//                 return (node0(argv[2]));

//         if ((argc > 2) && (strcmp(NODE1, argv[1]) == 0))
//                 return (node1(argv[2], argv[3]));

//         fprintf(stderr, "Usage: pipeline %s|%s <URL> <ARG> ...'\n",
//                 NODE0, NODE1);
//         return (1);
// }



/*
    Copyright (c) 2013 250bpm s.r.o.

    Permission is hereby granted, free of charge, to any person obtaining a copy
    of this software and associated documentation files (the "Software"),
    to deal in the Software without restriction, including without limitation
    the rights to use, copy, modify, merge, publish, distribute, sublicense,
    and/or sell copies of the Software, and to permit persons to whom
    the Software is furnished to do so, subject to the following conditions:

    The above copyright notice and this permission notice shall be included
    in all copies or substantial portions of the Software.

    THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
    IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
    FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL
    THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
    LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING
    FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS
    IN THE SOFTWARE.
*/

#include "nanomsg/nn.hpp"

#include <nanomsg/pair.h>

#include <cassert>

int main ()
{
    nn::socket s1 (AF_SP, NN_PAIR);
    s1.bind ("inproc://a");
    nn::socket s2 (AF_SP, NN_PAIR);
    s2.connect ("inproc://a");

    s2.send ("ABC", 3, 0);
    char buf [3];
    int rc = s1.recv (buf, sizeof (buf), 0);
    assert (rc == 3);

    return 0;
}