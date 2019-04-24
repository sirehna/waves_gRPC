#include "gtest/gtest.h"
#include "greeter_sync_client.hh"

class ServerDemo : public ::testing::Test
{
};

TEST_F(ServerDemo, say_hello_demo)
{
    std::string port("50051");
    std::string ip("server");
    GreeterClient greeter(grpc::CreateChannel(
        ip + ":" + port, grpc::InsecureChannelCredentials()));
    std::string user("world");
    std::string reply = greeter.SayHello(user);
    ASSERT_EQ(reply, "Hello world");
}
