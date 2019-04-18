#include <iostream>
#include <memory>
#include <vector>
#include <string>
#include <cmath>

#include <grpcpp/grpcpp.h>

#include "wave.grpc.pb.h"

#define PI (4.0 * std::atan(1.0))
#define G 9.81

using grpc::Server;
using grpc::ServerBuilder;
using grpc::ServerContext;
using grpc::ServerWriter;
using grpc::Status;
using wave::Point;
using wave::Elevation;
using wave::WaveService;
using wave::FlatDiscreteDirectionalWaveSpectrum;


class WaveServiceImpl final : public WaveService::Service {
    public:
        explicit WaveServiceImpl(const FlatDiscreteDirectionalWaveSpectrum& wave_spectrum):
            wave_spectrum_(wave_spectrum) {}

        Status GetElevation(ServerContext* context, const Point* request,
                            Elevation* reply) override
        {
            reply->clear_z();

            const size_t max_size = std::min(request->x_size(), request->y_size());
            for (size_t index = 0; index < max_size; ++index)
            {
                reply->add_z(- wave_spectrum_.a() * sin(
                                    wave_spectrum_.k() * (request->x(index) * cos(wave_spectrum_.psi()) + request->y(index) * sin(wave_spectrum_.psi()))
                                    - wave_spectrum_.omega() * request->t()
                                    + wave_spectrum_.phase()
                                )
                            );
            }

            return Status::OK;
        }

        Status GetElevations(ServerContext* context, const Point* request,
                            ServerWriter<Elevation>* writer) override
        {
            Elevation elevation;
            const size_t max_size = std::min(request->x_size(), request->y_size());
            for (double t = request->t_start(); t < request->t_end(); t = t + request->dt())
            {
                elevation.clear_z();
                elevation.set_t(t);
                for (size_t index = 0; index < max_size; ++index)
                {
                    elevation.add_z(- wave_spectrum_.a() * sin(
                                            wave_spectrum_.k() * (request->x(index) * cos(wave_spectrum_.psi()) + request->y(index) * sin(wave_spectrum_.psi()))
                                            - wave_spectrum_.omega() * t
                                            + wave_spectrum_.phase()
                                        )
                                    );
                }
                writer->Write(elevation);
            }
            return Status::OK;
        }

    private:
        FlatDiscreteDirectionalWaveSpectrum wave_spectrum_;
};

void compute_wave_spectrum(FlatDiscreteDirectionalWaveSpectrum& wave_spectrum);
void compute_wave_spectrum(FlatDiscreteDirectionalWaveSpectrum& wave_spectrum)
{
    wave_spectrum.set_a(2.0);
    wave_spectrum.set_omega(2 * PI / 12);
    wave_spectrum.set_psi(PI / 4);
    wave_spectrum.set_k(wave_spectrum.omega() * wave_spectrum.omega() / G);
    wave_spectrum.set_phase(0.0);
}

void run_server(const FlatDiscreteDirectionalWaveSpectrum& wave_spectrum);
void run_server(const FlatDiscreteDirectionalWaveSpectrum& wave_spectrum)
{
    std::string server_address("0.0.0.0:50051");
    WaveServiceImpl service(wave_spectrum);

    ServerBuilder builder;
    builder.AddListeningPort(server_address, grpc::InsecureServerCredentials());
    builder.RegisterService(&service);
    std::unique_ptr<Server> server(builder.BuildAndStart());
    std::cout << "Server listening on " << server_address << std::endl;
    server->Wait();
}

int main(int argc, char** argv)
{
    FlatDiscreteDirectionalWaveSpectrum wave_spectrum;
    compute_wave_spectrum(wave_spectrum);

    run_server(wave_spectrum);

    return 0;
}
