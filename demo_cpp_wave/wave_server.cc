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
using wave::ElevationRequest;
using wave::ElevationResponse;
using wave::ElevationService;
using wave::FlatDiscreteDirectionalWaveSpectrum;

double compute_elevation(const double x, const double y, const double t, const FlatDiscreteDirectionalWaveSpectrum& wave_spectrum);
double compute_elevation(const double x, const double y, const double t, const FlatDiscreteDirectionalWaveSpectrum& wave_spectrum)
{
    return - wave_spectrum.a() * sin(
                                        wave_spectrum.k() * (x * cos(wave_spectrum.psi()) + y * sin(wave_spectrum.psi()))
                                        - wave_spectrum.omega() * t
                                        + wave_spectrum.phase()
                                    );
}

class ElevationServiceImpl final : public ElevationService::Service {
    public:
        explicit ElevationServiceImpl(const FlatDiscreteDirectionalWaveSpectrum& wave_spectrum):
            wave_spectrum_(wave_spectrum) {}

        Status GetElevation(ServerContext* context, const ElevationRequest* request,
                            ElevationResponse* reply) override
        {
            reply->clear_z();

            for (const Point& point : request->points())
            {
                reply->add_z(compute_elevation(point.x(), point.y(), request->t(), wave_spectrum_));
            }

            return Status::OK;
        }

        Status GetElevations(ServerContext* context, const ElevationRequest* request,
                            ServerWriter<ElevationResponse>* writer) override
        {
            ElevationResponse elevation;
            for (double t = request->t_start(); t < request->t_end(); t += request->dt())
            {
                elevation.clear_z();
                elevation.set_t(t);
                for (const Point& point : request->points())
                {
                    elevation.add_z(compute_elevation(point.x(), point.y(), t, wave_spectrum_));
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
    ElevationServiceImpl service(wave_spectrum);

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
