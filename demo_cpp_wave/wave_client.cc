/*
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

#include <iostream>
#include <memory>
#include <vector>
#include <string>

#include <grpcpp/grpcpp.h>

#include <args.hxx>

#include "NumericalErrorException.hpp"

#include "wave.grpc.pb.h"

using grpc::Channel;
using grpc::ClientContext;
using grpc::ClientReader;
using grpc::Status;
using wave::Point;
using wave::Elevation;
using wave::WaveService;

void displayElevations(const std::vector<double> z, const std::vector<double> x, const std::vector<double> y, const double t);
void displayElevations(const std::vector<double> z, const std::vector<double> x, const std::vector<double> y, const double t) {
  if (z.size() > 0){
    for (size_t index = 0; index < z.size(); ++index) {
      std::cout << "WaveService (x: " << x[index] << ", y: " << y[index] << ", t: " << t <<  ") received: " << z[index] << std::endl;
    }
  } else {
    std::cout << "WaveService received no data." << std::endl;
  }
}

class WaveServiceClient {
 public:
  WaveServiceClient(std::shared_ptr<Channel> channel)
      : stub_(WaveService::NewStub(channel)) {}

  // Assembles the client's payload, sends it and presents the response back
  // from the server.
  void ClientGetElevation(const std::vector<double> x, const std::vector<double> y, const double t) {
    // Data we are sending to the server.
    Point request;
    size_t max_size = std::min(x.size(), y.size());
    for (size_t index = 0; index < max_size; ++index) {
      request.add_x(x[index]);
      request.add_y(y[index]);
    }
    request.set_t(t);

    // Container for the data we expect from the server.
    Elevation reply;

    // Context for the client. It could be used to convey extra information to
    // the server and/or tweak certain RPC behaviors.
    ClientContext context;

    // The actual RPC.
    Status status = stub_->GetElevation(&context, request, &reply);

    // Act upon its status.
    if (status.ok()) {
      std::vector<double> z(reply.z().begin(), reply.z().end());
      displayElevations(z, x, y, t);
    } else {
      std::cout << status.error_code() << ": " << status.error_message()
                << std::endl;
      std::cout << "WaveService failed." << std::endl;
    }
  }

  // Assembles the client's payload, sends it and presents the response back
  // from the server.
  void ClientGetElevations(const std::vector<double> x, const std::vector<double> y,
                           const double dt, const double t_start, const double t_end) {
    // Data we are sending to the server.
    Point request;
    size_t max_size = std::min(x.size(), y.size());
    for (size_t index = 0; index < max_size; ++index) {
      request.add_x(x[index]);
      request.add_y(y[index]);
    }
    request.set_dt(dt);
    request.set_t_start(t_start);
    request.set_t_end(t_end);

    // Container for the data we expect from the server.
    Elevation elevation;

    // Context for the client. It could be used to convey extra information to
    // the server and/or tweak certain RPC behaviors.
    ClientContext context;

    std::unique_ptr<ClientReader<Elevation> > reader(stub_->GetElevations(&context, request));
    while (reader->Read(&elevation)) {
      std::vector<double> z(elevation.z().begin(), elevation.z().end());
      displayElevations(z, x, y, elevation.t());
    }
    Status status = reader->Finish();

    if (!status.ok()) {
      std::cout << status.error_code() << ": " << status.error_message()
                << std::endl;
      std::cout << "WaveService failed." << std::endl;
    }
  }

  private:
    std::unique_ptr<WaveService::Stub> stub_;
};


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
    std::cout << "Client" << std::endl;
    WaveServiceClient waveService(grpc::CreateChannel(
        ip + ":" + port, grpc::InsecureChannelCredentials()));
    std::vector<double> x{1.3, 2, 0};
    std::vector<double> y{2.7, 0.5, 0};
    double t(0.1);

    std::cout << std::endl;
    std::cout << "Unary Elevation" << std::endl << std::endl;
    waveService.ClientGetElevation(x, y, t);

    double dt(0.1);
    double t_start(0.0);
    double t_end(0.25);

    std::cout << std::endl;
    std::cout << "Server Streaming Elevation" << std::endl << std::endl;
    waveService.ClientGetElevations(x, y, dt, t_start, t_end);
    std::cout << std::endl;

    return 0;
}