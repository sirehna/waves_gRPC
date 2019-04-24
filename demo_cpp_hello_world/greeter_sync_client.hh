#ifndef GREETER_SYNC_CLIENT_HPP_
#define GREETER_SYNC_CLIENT_HPP_

#include <iostream>
#include <memory>
#include <string>

#include <grpcpp/grpcpp.h>
#include "helloworld.grpc.pb.h"

using grpc::Channel;
using grpc::ClientContext;
using grpc::Status;
using helloworld::HelloRequest;
using helloworld::HelloReply;
using helloworld::Greeter;

class GreeterClient {
 public:
  GreeterClient(std::shared_ptr<Channel> channel)
      : stub_(Greeter::NewStub(channel)) {}
  std::string SayHello(const std::string& user);
 private:
  std::unique_ptr<Greeter::Stub> stub_;
};

#endif