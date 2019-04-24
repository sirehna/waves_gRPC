#include <iostream>
#include <memory>
#include <vector>
#include <string>
#include <chrono>
#include <grpcpp/grpcpp.h>
#include "args.hxx"
#include "wave_client.hh"

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
    display_elevations(elevation_service.get_elevation(x, y, t));
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
