#include <vector>
#include <grpcpp/grpcpp.h>
#include "wave.grpc.pb.h"

using grpc::Channel;
using wave::ElevationRequest;
using wave::ElevationResponse;
using wave::ElevationRequestRepeated;
using wave::ElevationResponseRepeated;
using wave::ElevationService;

void add_points_to_request(ElevationRequest& request, const std::vector<double>& x, const std::vector<double>& y);
void add_points_to_request_repeated(ElevationRequestRepeated& request, const std::vector<double>& x, const std::vector<double>& y);

void display_elevations(const ElevationResponse& elevation_response);

class ElevationServiceClient
{
    public:
        ElevationServiceClient(const std::shared_ptr<Channel>& channel)
            : stub_(ElevationService::NewStub(channel)) {}
        ElevationResponse get_elevation(const ElevationRequest& resquest);
        ElevationResponse get_elevation_input_repeated(const ElevationRequestRepeated& resquest);
        ElevationResponseRepeated get_elevation_output_repeated(const ElevationRequest& resquest, bool does_return_xy);
        ElevationResponseRepeated get_elevation_repeated(const ElevationRequestRepeated& resquest, bool does_return_xy);
        void get_elevations(const std::vector<double>& x, const std::vector<double>& y,
                            const double dt, const double t_start, const double t_end);
    private:
        std::unique_ptr<ElevationService::Stub> stub_;
};