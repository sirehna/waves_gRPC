#include <iostream>
#include <memory>
#include <vector>
#include <string>
#include <chrono>

#include <grpcpp/grpcpp.h>

#include <args.hxx>

#include "wave.grpc.pb.h"

using grpc::Channel;
using grpc::ClientContext;
using grpc::ClientReader;
using grpc::Status;
using wave::Point;
using wave::ElevationRequest;
using wave::ElevationPoint;
using wave::ElevationResponse;
using wave::ElevationService;

void add_points_to_request(ElevationRequest& request, const std::vector<double>& x, const std::vector<double>& y);
void add_points_to_request(ElevationRequest& request, const std::vector<double>& x, const std::vector<double>& y)
{
    const size_t max_size = std::min(x.size(), y.size());
    for (size_t index = 0; index < max_size; ++index)
    {
        Point* added_point = request.add_points();
        added_point->set_x(x[index]);
        added_point->set_y(y[index]);
    }
}

void display_elevations(const ElevationResponse& elevation_response);
void display_elevations(const ElevationResponse& elevation_response)
{
    if (elevation_response.elevation_points_size() > 0)
    {
        for (const ElevationPoint& elevation_point : elevation_response.elevation_points())
        {
            std::cout << "ElevationService (x: " << elevation_point.x() << ", y: " << elevation_point.y() << ", t: " << elevation_response.t() <<  ") received: " << elevation_point.z() << std::endl;
        }
    }
    else
    {
        std::cout << "ElevationService received no data." << std::endl;
    }
}

class ElevationServiceClient
{
    public:
        ElevationServiceClient(const std::shared_ptr<Channel>& channel)
            : stub_(ElevationService::NewStub(channel)) {}

        void get_elevation(const std::vector<double>& x, const std::vector<double>& y, const double t)
        {
            ElevationRequest request;
            add_points_to_request(request, x, y);
            request.set_t(t);

            ElevationResponse reply;
            ClientContext context;

            Status status = stub_->GetElevation(&context, request, &reply);
            if (status.ok())
            {
                // display_elevations(reply);
            }
            else
            {
                std::cout << status.error_code() << ": " << status.error_message() << std::endl;
                std::cout << "ElevationService failed." << std::endl;
            }
        }

        void get_elevations(const std::vector<double>& x, const std::vector<double>& y,
                            const double dt, const double t_start, const double t_end)
        {
            ElevationRequest request;
            add_points_to_request(request, x, y);
            request.set_t_start(t_start);
            request.set_t_end(t_end);
            request.set_dt(dt);

            ElevationResponse elevationResponse;
            ClientContext context;

            std::unique_ptr<ClientReader<ElevationResponse> > reader(stub_->GetElevations(&context, request));
            while (reader->Read(&elevationResponse))
            {
                // display_elevations(elevationResponse);
            }
            Status status = reader->Finish();

            if (not(status.ok()))
            {
                std::cout << status.error_code() << ": " << status.error_message() << std::endl;
                std::cout << "ElevationService failed." << std::endl;
            }
        }

    private:
        std::unique_ptr<ElevationService::Stub> stub_;
};


int main(int argc, char const * const argv[])
{
    // Inputs
    args::ArgumentParser parser("This is a test grpc client demo program.", "Enjoy.");
    args::HelpFlag help(parser, "help", "Display this help menu", {'h', "help"});
    args::ValueFlag<int> input_port(parser, "port", "The port to use", {'p', "port"});
    args::ValueFlag<std::string> input_ip(parser, "ip", "The ip to use", {"ip"});
    try
    {
        parser.ParseCLI(argc, argv);
    }
    catch (args::Help)
    {
        std::cout << parser;
        return 0;
    }
    catch (const args::ParseError& e)
    {
        std::cerr << e.what() << std::endl;
        std::cerr << parser;
        return 1;
    }
    catch (const args::ValidationError& e)
    {
        std::cerr << e.what() << std::endl;
        std::cerr << parser;
        return 1;
    }
    catch (const std::exception& e)
    {
        std::cerr << "An internal error has occurred: " << e.what() << std::endl;
        return -1;
    }

    std::string port("50051");
    std::string ip("localhost");
    if (input_port)
    {
      std::cout << "input_port: " << args::get(input_port) << std::endl;
      port = std::to_string(args::get(input_port));
    }
    if (input_ip) {
      std::cout << "input_ip: " << args::get(input_ip) << std::endl;
      ip = args::get(input_ip);
    }

    std::cout << "Client" << std::endl;
    ElevationServiceClient elevation_service(grpc::CreateChannel(
        ip + ":" + port, grpc::InsecureChannelCredentials()));
    std::cout << std::endl;

    // Data
    const std::vector<double> x{1.3, 2, 0};
    const std::vector<double> y{2.7, 0.5, 0};
    const double t(0.1);
    const double dt(0.1);
    const double t_start(0.0);
    const double t_end(0.25);

    // Unary elevation
    auto start = std::chrono::system_clock::now();
    std::cout << "Unary Elevation" << std::endl << std::endl;
    elevation_service.get_elevation(x, y, t);
    auto end = std::chrono::system_clock::now();
    std::chrono::duration<double> diff = end-start;
    std::cout << diff.count() << " s\n";
    std::cout << std::endl;

    // Server streaming elevation
    start = std::chrono::system_clock::now();
    std::cout << "Server Streaming Elevation" << std::endl << std::endl;
    elevation_service.get_elevations(x, y, dt, t_start, t_end);
    end = std::chrono::system_clock::now();
    diff = end-start;
    std::cout << diff.count() << " s\n";
    std::cout << std::endl;

    return 0;
}
