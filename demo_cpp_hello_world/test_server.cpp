#include "gtest/gtest.h"
#include <grpcpp/grpcpp.h>
#include "greeter_sync_server.hh"

#include "helloworld.grpc.pb.h"

class ServerDemo : public ::testing::Test
{
};

TEST_F(ServerDemo, say_hello_demo)
{
    grpc::ServerContext context;
    helloworld::HelloRequest request;
    helloworld::HelloReply reply;
    GreeterServiceImpl greeter_service;
    // grpc::Status status = greeter_service.SayHello(&context, &request, &reply);
    ASSERT_EQ("NED", "NED");
}

