#ifndef GREETER_SYNC_SERVER_HPP_
#define GREETER_SYNC_SERVER_HPP_

#include <grpcpp/grpcpp.h>

#include "helloworld.grpc.pb.h"

class GreeterServiceImpl final : public helloworld::Greeter::Service {
    grpc::Status SayHello(grpc::ServerContext* context, const helloworld::HelloRequest* request, helloworld::HelloReply* reply) override;
};

namespace greeterserver {
    void RunServer();
}

#endif
