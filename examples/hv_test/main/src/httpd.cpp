#include "hv/HttpServer.h"
#include "hv/hlog.h"
#include "hv/json.hpp"
int main()
{
    HttpService router;

    // curl -v http://ip:port/
    router.Static("/", "./");

    // curl -v http://ip:port/proxy/get
    router.Proxy("/proxy/", "http://httpbin.org/");

    // curl -v http://ip:port/ping
    router.GET("/ping", [](HttpRequest *req, HttpResponse *resp)
               { return resp->String("pong"); });

    // curl -v http://ip:port/data
    router.GET("/data", [](HttpRequest *req, HttpResponse *resp)
               {
        static char data[] = "0123456789";
        return resp->Data(data, 10 /*, false */); });

    // curl -v http://ip:port/paths
    router.GET("/paths", [&router](HttpRequest *req, HttpResponse *resp)
               { return resp->Json(router.Paths()); });

    // curl -v http://ip:port/get?env=1
    router.GET("/get", [](HttpRequest *req, HttpResponse *resp)
               {
        resp->json["origin"] = req->client_addr.ip;
        resp->json["url"] = req->url;
        resp->json["args"] = req->query_params;
        resp->json["headers"] = req->headers;
        return 200; });

    // curl -v http://ip:port/echo -d "hello,world!"
    router.POST("/echo", [](const HttpContextPtr &ctx)
                { return ctx->send(ctx->body(), ctx->type()); });

    // curl -v http://ip:port/user/123
    router.GET("/user/{id}", [](const HttpContextPtr &ctx)
               {
        hv::Json resp;
        resp["id"] = ctx->param("id");
        return ctx->send(resp.dump(2)); });

    http_server_t server;
    server.port = 8000;
    server.service = &router;
    http_server_run(&server);
    return 0;
}
