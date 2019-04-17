#ifndef GREETER_SYNC_SERVER_HPP_
#define GREETER_SYNC_SERVER_HPP_

#include <grpcpp/grpcpp.h>

#ifdef BAZEL_BUILD
#include "examples/protos/helloworld.grpc.pb.h"
#else
#include "helloworld.grpc.pb.h"
#endif

class GreeterServiceImpl final : public helloworld::Greeter::Service {
    grpc::Status SayHello(grpc::ServerContext* context, const helloworld::HelloRequest* request, helloworld::HelloReply* reply) override;
};

namespace greeterserver {
    void RunServer();
}

#endif