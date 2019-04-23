/*
 *
 * Copyright 2015 gRPC authors.
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *     http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 *
 */

#include <chrono>
#include <iostream>
#include <memory>
#include <string>

#include <grpcpp/grpcpp.h>

#include <args.hxx>

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

  // Assembles the client's payload, sends it and presents the response back
  // from the server.
  std::string SayHello(const std::string& user) {
    // Data we are sending to the server.
    HelloRequest request;
    request.set_name(user);

    // Container for the data we expect from the server.
    HelloReply reply;

    // Context for the client. It could be used to convey extra information to
    // the server and/or tweak certain RPC behaviors.
    ClientContext context;

    // The actual RPC.
    Status status = stub_->SayHello(&context, request, &reply);

    // Act upon its status.
    if (status.ok()) {
      return reply.message();
    } else {
      std::cout << status.error_code() << ": " << status.error_message()
                << std::endl;
      return "RPC failed";
    }
  }

 private:
  std::unique_ptr<Greeter::Stub> stub_;
};

// int main(int argc, char** argv)
// {
//   // Instantiate the client. It requires a channel, out of which the actual RPCs
//   // are created. This channel models a connection to an endpoint (in this case,
//   // localhost at port 50051). We indicate that the channel isn't authenticated
//   // (use of InsecureChannelCredentials()).
//   std::cout << "Client" << std::endl;
//   // GreeterClient greeter(grpc::CreateChannel(
//   //     "grpc_net:50051", grpc::InsecureChannelCredentials()));
//   //GreeterClient greeter(grpc::CreateChannel(
//   //    "localhost:50051", grpc::InsecureChannelCredentials()));
//   GreeterClient greeter(grpc::CreateChannel(
//       "172.19.0.2:50051", grpc::InsecureChannelCredentials()));
//   std::string user("world");
//   std::string reply = greeter.SayHello(user);
//   std::cout << "Greeter received: " << reply << std::endl;
//
//   return 0;
// }

int main(int argc, char const * const argv[])
{
    args::ArgumentParser parser("This is a test grpc client demo program.", "Enjoy.");
    args::HelpFlag help(parser, "help", "Display this help menu", {'h', "help"});
    args::ValueFlag<int> input_port(parser, "port", "The websocket port to use", {'p', "port"});
    args::ValueFlag<std::string> input_ip(parser, "ip", "The websocket port to use", {"ip"});
    try
    {
        parser.ParseCLI(argc, argv);
    }
    catch (args::Help)
    {
        std::cout << parser;
        return 0;
    }
    catch (args::ParseError e)
    {
        std::cerr << e.what() << std::endl;
        std::cerr << parser;
        return 1;
    }
    catch (args::ValidationError e)
    {
        std::cerr << e.what() << std::endl;
        std::cerr << parser;
        return 1;
    }
    std::string port("50051");
    std::string ip("localhost");
    if (input_port) { std::cout << "input_port: " << args::get(input_port) << std::endl; port = std::to_string(args::get(input_port));}
    if (input_ip) { std::cout << "input_ip: " << args::get(input_ip) << std::endl; ip = args::get(input_ip); }

    // Instantiate the client. It requires a channel, out of which the actual RPCs
    // are created. This channel models a connection to an endpoint (in this case,
    // localhost at port 50051). We indicate that the channel isn't authenticated
    // (use of InsecureChannelCredentials()).
    std::cout << "Sync Client" << std::endl;
    std::cout << "Connecting to " << ip << ":" << port << std::endl;
    // GreeterClient greeter(grpc::CreateChannel(
    //     "grpc_net:50051", grpc::InsecureChannelCredentials()));
    //GreeterClient greeter(grpc::CreateChannel(
    //    "localhost:50051", grpc::InsecureChannelCredentials()));
    GreeterClient greeter(grpc::CreateChannel(
        ip + ":" + port, grpc::InsecureChannelCredentials()));
      // "172.19.0.2:50051", grpc::InsecureChannelCredentials()));
    std::string user("world");
    auto start = std::chrono::system_clock::now();
    std::string reply = greeter.SayHello(user);
    auto end = std::chrono::system_clock::now();
    std::chrono::duration<double> diff = end-start;
    std::cout << diff.count() << " s\n";
    if (reply == "RPC failed")
    {
        std::cout << "Connection failed " << std::endl;
        return 1;
    }
    else
    {
        std::cout << "Greeter received: " << reply << std::endl;
        return 0;
    }
}
