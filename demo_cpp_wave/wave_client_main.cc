#include <iostream>
#include <memory>
#include <vector>
#include <string>
#include <chrono>
#include <grpcpp/grpcpp.h>
#include "args.hxx"
#include "wave_client.hh"

using wave::ElevationRequest;
using wave::ElevationRequestRepeated;

double test_unary_elevation(size_t vector_size, size_t loop_size, ElevationServiceClient& elevation_service)
{
    // Data
    const std::vector<double> x(vector_size, 1.3);
    const std::vector<double> y(vector_size, 2.7);
    const double t(0.1);
    auto start = std::chrono::system_clock::now();
    std::chrono::duration<double> diff = start-start;

    ElevationRequest request;
    add_points_to_request(request, x, y);
    request.set_t(t);

    // Compute average time response for requesting elevation
    for (size_t ind = 0; ind < loop_size; ++ind)
    {
        start = std::chrono::system_clock::now();
        elevation_service.get_elevation(request);
        diff += std::chrono::system_clock::now() - start;
    }
    return diff.count() * 1000 / loop_size;
}

double test_input_repeated_unary_elevation(size_t vector_size, size_t loop_size, ElevationServiceClient& elevation_service)
{
    // Data
    const std::vector<double> x(vector_size, 1.3);
    const std::vector<double> y(vector_size, 2.7);
    const double t(0.1);
    auto start = std::chrono::system_clock::now();
    std::chrono::duration<double> diff = start-start;

    ElevationRequestRepeated request;
    add_points_to_request_repeated(request, x, y);
    request.set_t(t);

    // Compute average time response for requesting elevation
    for (size_t ind = 0; ind < loop_size; ++ind)
    {
        start = std::chrono::system_clock::now();
        elevation_service.get_elevation_input_repeated(request);
        diff += std::chrono::system_clock::now() - start;
    }
    return diff.count() * 1000 / loop_size;
}

double test_output_repeated_unary_elevation(size_t vector_size, size_t loop_size, ElevationServiceClient& elevation_service, bool does_return_xy)
{
    // Data
    const std::vector<double> x(vector_size, 1.3);
    const std::vector<double> y(vector_size, 2.7);
    const double t(0.1);
    auto start = std::chrono::system_clock::now();
    std::chrono::duration<double> diff = start-start;

    ElevationRequest request;
    add_points_to_request(request, x, y);
    request.set_t(t);

    // Compute average time response for requesting elevation
    for (size_t ind = 0; ind < loop_size; ++ind)
    {
        start = std::chrono::system_clock::now();
        elevation_service.get_elevation_output_repeated(request, does_return_xy);
        diff += std::chrono::system_clock::now() - start;
    }
    return diff.count() * 1000 / loop_size;
}

double test_repeated_unary_elevation(size_t vector_size, size_t loop_size, ElevationServiceClient& elevation_service, bool does_return_xy)
{
    // Data
    const std::vector<double> x(vector_size, 1.3);
    const std::vector<double> y(vector_size, 2.7);
    const double t(0.1);
    auto start = std::chrono::system_clock::now();
    std::chrono::duration<double> diff = start-start;

    ElevationRequestRepeated request;
    add_points_to_request_repeated(request, x, y);
    request.set_t(t);

    // Compute average time response for requesting elevation
    for (size_t ind = 0; ind < loop_size; ++ind)
    {
        start = std::chrono::system_clock::now();
        elevation_service.get_elevation_repeated(request, does_return_xy);
        diff += std::chrono::system_clock::now() - start;
    }
    return diff.count() * 1000 / loop_size;
}


void writeMardownResults(size_t vector_size, size_t loop_size, ElevationServiceClient& elevation_service)
{
    std::cout << "## " << loop_size << " requests. Vector of size " << vector_size << "." << std::endl << std::endl
              << "Input                    | Output          | Average time per request (ms)" << std::endl
              << "-------------------------|--------------------------------------|---------" << std::endl;

    std::cout << "(repeated x, repeated y) | repeated z                           | "
              << test_repeated_unary_elevation(vector_size, loop_size, elevation_service, false) << std::endl;
    std::cout << "(repeated x, repeated y) | (repeated x, repeated y, repeated z) | "
              << test_repeated_unary_elevation(vector_size, loop_size, elevation_service, true) << std::endl;
    std::cout << "(repeated x, repeated y) | repeated (x, y, z)                   | "
              << test_input_repeated_unary_elevation(vector_size, loop_size, elevation_service) << std::endl;
    std::cout << "repeated (x, y)          | repeated z                           | "
              << test_output_repeated_unary_elevation(vector_size, loop_size, elevation_service, false) << std::endl;
    std::cout << "repeated (x, y)          | (repeated x, repeated y, repeated z) | "
              << test_output_repeated_unary_elevation(vector_size, loop_size, elevation_service, true) << std::endl;
    std::cout << "repeated (x, y)          | repeated (x, y, z)                   | "
              << test_unary_elevation(vector_size, loop_size, elevation_service) << std::endl;
    std::cout << std::endl;
}


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

    size_t loop_size = 10000;
    size_t vector_size = 1;
    writeMardownResults(vector_size, loop_size, elevation_service);

    loop_size = 1000;
    vector_size = 1000;
    writeMardownResults(vector_size, loop_size, elevation_service);

/*
    // Server streaming elevation
    const double dt(0.1);
    const double t_start(0.0);
    const double t_end(0.25);
    std::cout << "Server Streaming Elevation" << std::endl << std::endl;
    elevation_service.get_elevations(x, y, dt, t_start, t_end);
*/
    return 0;
}
