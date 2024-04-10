#include <iostream>
#include <memory>
#include <string>
 
#include <grpcpp/grpcpp.h>
 
#ifdef BAZEL_BUILD
#include "examples/protos/helloworld.grpc.pb.h"
#else
#include "helloworld.grpc.pb.h"
#endif
 
using grpc::Server;
using grpc::ServerBuilder;
using grpc::ServerContext;
using grpc::Status;
using helloworld::HelloRequest;
using helloworld::HelloReply;
using helloworld::Greeter;
 
//服务端行为
class GreeterServiceImpl final : public Greeter::Service {
    Status SayHello(ServerContext *context, const HelloRequest *request, HelloReply *reply) override {
        std::string prefix("Hello ");
        reply->set_message(prefix + request->name());
        return Status::OK;
    }
};
 
void RunServer()
{
    std::string server_address("0.0.0.0:50051");
    GreeterServiceImpl service;
 
    ServerBuilder builder;

    //在没有任何身份验证机制的情况下监听给定的地址
    builder.AddListeningPort(server_address, grpc::InsecureServerCredentials());

    //注册服务
    builder.RegisterService(&service);

    //启动服务
    std::unique_ptr<Server> server(builder.BuildAndStart());
    std::cout << "Server listening on " << server_address << std::endl;
 
    //等待服务
    server->Wait();
}
 
int main(int argc, char **argv)
{
    RunServer();
 
    return 0;
}
