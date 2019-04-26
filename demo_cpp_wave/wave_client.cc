#include <iostream>
#include <memory>
#include <vector>
#include <string>
#include <grpcpp/grpcpp.h>
#include "wave.grpc.pb.h"

#include "wave_client.hh"

using grpc::Channel;
using grpc::ClientContext;
using grpc::ClientReader;
using grpc::Status;
using wave::Point;
using wave::ElevationRequest;
using wave::ElevationPoint;
using wave::ElevationResponse;
using wave::ElevationService;

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

ElevationResponse ElevationServiceClient::get_elevation(const ElevationRequest& request)
{
    ElevationResponse reply;
    ClientContext context;

    Status status = stub_->GetElevation(&context, request, &reply);
    if (status.ok())
    {
        return reply;
    }
    else
    {
        std::cout << status.error_code() << ": " << status.error_message() << std::endl;
        std::cout << "ElevationService failed." << std::endl;
    }
}

void ElevationServiceClient::get_elevations(const std::vector<double>& x, const std::vector<double>& y,
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
        display_elevations(elevationResponse);
    }
    Status status = reader->Finish();

    if (not(status.ok()))
    {
        std::cout << status.error_code() << ": " << status.error_message() << std::endl;
        std::cout << "ElevationService failed." << std::endl;
    }
}
