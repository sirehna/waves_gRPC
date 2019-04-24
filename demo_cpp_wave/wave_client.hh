#include <vector>
#include <grpcpp/grpcpp.h>
#include "wave.grpc.pb.h"

using grpc::Channel;
using wave::ElevationRequest;
using wave::ElevationResponse;
using wave::ElevationService;

void add_points_to_request(ElevationRequest& request, const std::vector<double>& x, const std::vector<double>& y);

void display_elevations(const ElevationResponse& elevation_response);

class ElevationServiceClient
{
    public:
        ElevationServiceClient(const std::shared_ptr<Channel>& channel)
            : stub_(ElevationService::NewStub(channel)) {}
        ElevationResponse get_elevation(const std::vector<double>& x, const std::vector<double>& y, const double t);
        void get_elevations(const std::vector<double>& x, const std::vector<double>& y,
                            const double dt, const double t_start, const double t_end);
    private:
        std::unique_ptr<ElevationService::Stub> stub_;
};