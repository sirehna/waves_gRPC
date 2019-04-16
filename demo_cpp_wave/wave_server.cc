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

#include <iostream>
#include <memory>
#include <vector>
#include <string>
#include <cmath>

#define PI (4.0 * std::atan(1.0))
#define G 9.81

#include <grpcpp/grpcpp.h>

#ifdef BAZEL_BUILD
#include "examples/protos/wave.grpc.pb.h"
#else
#include "wave.grpc.pb.h"
#endif

using grpc::Server;
using grpc::ServerBuilder;
using grpc::ServerContext;
using grpc::Status;
using wave::Point;
using wave::Elevation;
using wave::WaveService;
using wave::FlatDiscreteDirectionalWaveSpectrum;

// Logic and data behind the server's behavior.
class WaveServiceImpl final : public WaveService::Service {
  Status GetElevation(ServerContext* context, const Point* request,
                  Elevation* reply) override {
    FlatDiscreteDirectionalWaveSpectrum waveSpectrum;
    waveSpectrum.set_a(2.0);
    waveSpectrum.set_omega(2 * PI / 12);
    waveSpectrum.set_psi(PI / 4);
    waveSpectrum.set_k(waveSpectrum.omega() * waveSpectrum.omega() / G);
    waveSpectrum.set_phase(0.0);

    reply->clear_z();

    int max_size = std::min(request->x_size(), request->y_size());
    for (int index = 0; index < max_size; index++) {
      reply->add_z(- waveSpectrum.a() * sin(
                        waveSpectrum.k() * (request->x(index) * cos(waveSpectrum.psi()) + request->y(index) * sin(waveSpectrum.psi()))
                        - waveSpectrum.omega() * request->t()
                        + waveSpectrum.phase()
                    )
                  );
    }

    return Status::OK;
  }
};


void RunServer() {
  std::string server_address("0.0.0.0:50051");
  // std::string server_address("grpc_net:50051");
  // std::string server_address("localhost:50051");
  WaveServiceImpl service;

  ServerBuilder builder;
  // Listen on the given address without any authentication mechanism.
  builder.AddListeningPort(server_address, grpc::InsecureServerCredentials());
  // Register "service" as the instance through which we'll communicate with
  // clients. In this case it corresponds to an *synchronous* service.
  builder.RegisterService(&service);
  // Finally assemble the server.
  std::unique_ptr<Server> server(builder.BuildAndStart());
  std::cout << "Server listening on " << server_address << std::endl;

  // Wait for the server to shutdown. Note that some other thread must be
  // responsible for shutting down the server for this call to ever return.
  server->Wait();
}

int main(int argc, char** argv) {
  RunServer();

  return 0;
}
