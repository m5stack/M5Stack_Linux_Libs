#include <iostream>
#include <memory>
#include <string>
 
#include <grpcpp/grpcpp.h>
 
#ifdef BAZEL_BUILD
#include "examples/protos/helloworld.grpc.pb.h"
#else
#include "helloworld.grpc.pb.h"
#endif
 
using grpc::Channel;
using grpc::ClientContext;
using grpc::Status;
using helloworld::HelloRequest;
using helloworld::HelloReply;
using helloworld::Greeter;
 
class GreeterClient {
public:
    GreeterClient(std::shared_ptr<Channel> channel) : stub_(Greeter::NewStub(channel)) {}
 
    //客户端请求函数
    std::string SayHello(const std::string &user)
    {
        //我们将要发送到服务器端的数据
        HelloRequest request;
        request.set_name(user);
 
        //我们从服务端收到的数据
        HelloReply reply;
 
        //客户端RPC上下文
        ClientContext context;
 
        //调用rpc函数
        Status status = stub_->SayHello(&context, request, &reply);
 
        //返回结果处理
        if(status.ok())
        {
            return reply.message();
        }
        else
        {
            std::cout << status.error_code() << ": " << status.error_message() << std::endl;
            return "RPC failed";
        }
    }
 
private:
    std::unique_ptr<Greeter::Stub> stub_;
};
 
int main(int argc, char** argv)
{
    GreeterClient greeter(grpc::CreateChannel("127.0.0.1:50051", grpc::InsecureChannelCredentials()));

    std::string user("world");

    std::string reply = greeter.SayHello(user);

    std::cout << "Greeter received: " << reply << std::endl;
 
    return 0;
}
