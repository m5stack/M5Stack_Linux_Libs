#include "hv/HttpServer.h"
#include "hv/hlog.h"
#include "hv/json.hpp"
#include <iostream>
#include <thread>
#include <list>
#include <mutex>
std::mutex myMutex;
std::list<HttpContextPtr> test;

void threadFunctionasdas(const HttpContextPtr &ctx)
{
    // std::cout << "Hello from thread " << id << std::endl;

    
    while (1)
    {
        std::lock_guard<std::mutex> lock(myMutex);
        static int nisad = 0;
        char nihao[32];
        sprintf(nihao, "{\"nihao:\"%d}", nisad++);
        // int mk = ctx->send(nihao);
        int mk = ctx->writer->write(nihao, strlen(nihao));

        printf("get mk:%d\n", mk);
        sleep(1);
    }
}

int threadFunction()
{
    while (1)
    {
        static int nisad = 0;
        char nihao[32];
        sprintf(nihao, "{\"nihao:\"%d}", nisad++);
        {
            printf("asdsad-------------10\n");
            std::lock_guard<std::mutex> lock(myMutex);
            printf("asdsad-------------11\n");

            auto it = test.begin();
            while (it != test.end()) {
                int mk = (*it)->writer->write(nihao, strlen(nihao));
                if(mk == -1)
                {
                    (*it)->writer->End();
                    it = test.erase(it);
                }
                else
                {
                    ++it;
                }
            }
        }
        sleep(1);
    }
    return 0;
}

int main()
{

    // std::thread thread1(threadFunction, 1);

    HttpService router;

    // curl -v http://ip:port/user/123
    router.GET("/echo", [](const HttpContextPtr &ctx)
               {
                printf("asdsad-------------1\n");
                std::lock_guard<std::mutex> lock(myMutex);
                printf("asdsad-------------2\n");
                // std::thread(threadFunctionasdas, ctx).detach();
                test.push_back(ctx);
                printf("asdsad-------------3\n");

        return 0; });
    std::thread asdas(threadFunction);
    http_server_t server;
    server.port = 8000;
    server.service = &router;
    http_server_run(&server);
    return 0;
}
