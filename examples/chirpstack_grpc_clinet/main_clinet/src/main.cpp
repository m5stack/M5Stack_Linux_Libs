#include <iostream>
#include <memory>
#include <string>

#include <grpcpp/grpcpp.h>

#include "device.grpc.pb.h"
#include "device.pb.h"

using api::DeviceQueueItem;
using api::EnqueueDeviceQueueItemRequest;
using api::EnqueueDeviceQueueItemResponse;
using grpc::Channel;
using grpc::ClientContext;
using grpc::Status;

using api::DeviceService;

int Debug_s = 1;

#define LOG_PRINT(fmt, ...)                                                                       \
    do                                                                                            \
    {                                                                                             \
        if (Debug_s)                                                                              \
        {                                                                                         \
            printf(fmt "  [info:%s:%d] [%s]\n", ##__VA_ARGS__, __FILE__, __LINE__, __FUNCTION__); \
        }                                                                                         \
    } while (0);

int main(int argc, char **argv)
{
    LOG_PRINT("");
    auto stub_ = DeviceService::NewStub(grpc::CreateChannel("192.168.28.239:8080", grpc::InsecureChannelCredentials()));
    LOG_PRINT("");
    EnqueueDeviceQueueItemRequest in;
    EnqueueDeviceQueueItemResponse out;

    LOG_PRINT("");
    DeviceQueueItem *queue_item = new DeviceQueueItem();
    queue_item->set_confirmed(false);
    queue_item->set_data("0123");
    queue_item->set_dev_eui("4bee530d29435bef");
    queue_item->set_f_port(10);

    in.set_allocated_queue_item(queue_item);

    LOG_PRINT("");
    ClientContext context;
    context.AddMetadata("authorization", "Bearer eyJ0eXAiOiJKV1QiLCJhbGciOiJIUzI1NiJ9.eyJhdWQiOiJjaGlycHN0YWNrIiwiaXNzIjoiY2hpcnBzdGFjayIsInN1YiI6ImM3YzdhNjM3LTYzNDYtNDM1ZS04YWNmLTQ2YzAxMjJlNTY4MCIsInR5cCI6ImtleSJ9.1GBzX5khO9oqR5hNQMf_nBTtZSFeOMxgPhMq6E89QdE");
    LOG_PRINT("");
    Status status = stub_->Enqueue(&context, in, &out);

    LOG_PRINT("");
    if (status.ok())
    {
        std::cout << status.error_code() << ": " << status.error_message() << std::endl;
        LOG_PRINT("RPC failed");
    }

    LOG_PRINT("");
    // GreeterClient greeter();

    // std::string user("world");

    // std::string reply = greeter.SayHello(user);

    // std::cout << "Greeter received: " << reply << std::endl;

    return 0;
}
