#include "hv/TcpServer.h"
#include "hv/hlog.h"
using namespace hv;

int main(int argc, char* argv[]) {


    hlog_set_level(LOG_LEVEL_SILENT);
    // hlog_destory();
    if (argc < 2) {
        printf("Usage: %s port\n", argv[0]);
        return -10;
    }
    int port = atoi(argv[1]);

    TcpServer srv;
    int listenfd = srv.createsocket(port);
    if (listenfd < 0) {
        return -20;
    }
    printf("server listen on port %d, listenfd=%d ...\n", port, listenfd);
    srv.onConnection = [](const SocketChannelPtr& channel) {
        std::string peeraddr = channel->peeraddr();
        if (channel->isConnected()) {
            printf("%s connected! connfd=%d\n", peeraddr.c_str(), channel->fd());
        } else {
            printf("%s disconnected! connfd=%d\n", peeraddr.c_str(), channel->fd());
        }
    };
    srv.onMessage = [](const SocketChannelPtr& channel, Buffer* buf) {
        // echo
        printf("< %.*s\n", (int)buf->size(), (char*)buf->data());
        channel->write(buf);
    };
    srv.onWriteComplete = [](const SocketChannelPtr& channel, Buffer* buf) {
        printf("> %.*s\n", (int)buf->size(), (char*)buf->data());
    };
    srv.setThreadNum(4);
    srv.start();

    // press Enter to stop
    while (getchar() != '\n');
    return 0;
}



// #include "hv/hloop.h"

// void on_close(hio_t* io) {
// }

// void on_recv(hio_t* io, void* buf, int readbytes) {
// 	// 回显数据
//     hio_write(io, buf, readbytes);
// }

// void on_accept(hio_t* io) {
// 	// 设置close回调
//     hio_setcb_close(io, on_close);
//     // 设置read回调
//     hio_setcb_read(io, on_recv);
//     // 开始读
//     hio_read(io);
// }

// int main(int argc, char** argv) {
//     if (argc < 2) {
//         printf("Usage: cmd port\n");
//         return -10;
//     }
//     int port = atoi(argv[1]);

//     // 创建事件循环
//     hloop_t* loop = hloop_new(0);
//     // 创建TCP服务
//     hio_t* listenio = hloop_create_tcp_server(loop, "0.0.0.0", port, on_accept);
//     if (listenio == NULL) {
//         return -20;
//     }
//     // 运行事件循环
//     hloop_run(loop);
//     // 释放事件循环
//     hloop_free(&loop);
//     return 0;
// }
