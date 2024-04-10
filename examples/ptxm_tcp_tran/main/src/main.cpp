#define USE_MONGOOSE 1

#if USE_LIBHV
#include "hv/TcpServer.h"
#include "hv/hlog.h"
#include "ptmx/ptmx_creat.h"
using namespace hv;

TcpServer srv;
ptmx ptm;

void msg_handl(const char *msg, int size)
{
    srv.broadcast(msg, size);
}

int main(int argc, char *argv[])
{

    hlog_set_level(LOG_LEVEL_SILENT);
    if (argc < 3)
    {
        printf("Usage: %s exe port\n", argv[0]);
        return -10;
    }
    int port = atoi(argv[2]);

    int listenfd = srv.createsocket(port);
    if (listenfd < 0)
    {
        return -20;
    }
    // printf("server listen on port %d, listenfd=%d ...\n", port, listenfd);

    // srv.onConnection = [](const SocketChannelPtr& channel) {
    //     std::string peeraddr = channel->peeraddr();
    //     if (channel->isConnected()) {
    //         printf("%s connected! connfd=%d\n", peeraddr.c_str(), channel->fd());
    //     } else {
    //         printf("%s disconnected! connfd=%d\n", peeraddr.c_str(), channel->fd());
    //     }
    // };
    // srv.onMessage = [](const SocketChannelPtr& channel, Buffer* buf) {
    //     // echo
    //     printf("< %.*s\n", (int)buf->size(), (char*)buf->data());
    //     channel->write(buf);
    // };
    // srv.onWriteComplete = [](const SocketChannelPtr& channel, Buffer* buf) {
    //     printf("> %.*s\n", (int)buf->size(), (char*)buf->data());
    // };
    srv.setThreadNum(1);
    srv.setMaxConnectionNum(2);
    srv.start();
    ptm.set_msg_call_back(msg_handl);
    ptm.open();

    int pid = fork();
    if (pid == 0)
    {
        freopen(ptm.get_slave_ptmx_name().c_str(), "w", stdout);
        freopen(ptm.get_slave_ptmx_name().c_str(), "w", stderr);
        freopen(ptm.get_slave_ptmx_name().c_str(), "r", stdin);
        execl(argv[1], argv[1],
              NULL);
    }

    // press Enter to stop
    while (1)
    {
        sleep(1);
    }
    return 0;
}

#elif USE_MONGOOSE
#include "mongoose.h"
#include "ptmx/ptmx_creat.h"


struct mg_mgr mgr;
ptmx ptm;

void msg_handl(const char *msg, int size)
{
    for (struct mg_connection *item = mgr.conns->next; item != NULL; item = item->next)
    {
        mg_send(item, msg, size);
    }
}



static void cb(struct mg_connection *c, int ev, void *ev_data, void *fn_data)
{
    switch (ev)
    {
    case MG_EV_OPEN:
        break;
    case MG_EV_ACCEPT:
    {
        MG_INFO(("SERVER accepted a connection"));
    }
    break;
    case MG_EV_READ:
    {
        // mg_send(c, c->recv.buf, c->recv.len);   // Echo received data back
        mg_iobuf_del(&c->recv, 0, c->recv.len); // And discard it
    }
    break;
    case MG_EV_CLOSE:
        break;
    case MG_EV_ERROR:
        break;
    default:
        break;
    }
}




int main(int argc, char *argv[])
{
    if (argc < 3)
    {
        printf("Usage: %s exe port\n", argv[0]);
        return -10;
    }
    int port = atoi(argv[2]);

    mg_mgr_init(&mgr); // Init manager
    char urls[100];
    sprintf(urls, "tcp://0.0.0.0:%d", port);
    mg_listen(&mgr, urls, cb, &mgr); // Setup listener

    ptm.set_msg_call_back(msg_handl);
    ptm.open();

    int pid = fork();
    if (pid == 0)
    {
        freopen(ptm.get_slave_ptmx_name().c_str(), "w", stdout);
        freopen(ptm.get_slave_ptmx_name().c_str(), "w", stderr);
        freopen(ptm.get_slave_ptmx_name().c_str(), "r", stdin);
        execl(argv[1], argv[1],
              NULL);
    }
    for (;;)
    {
        mg_mgr_poll(&mgr, 1000); // Event loop
    }
    mg_mgr_free(&mgr); // Cleanup
    return 0;
}

#endif