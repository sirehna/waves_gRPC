#include "gtest/gtest.h"
#include <chrono>
#include "wave_client.hh"
using wave::ElevationRequest;

class ServerDemo : public ::testing::Test
{
    protected:
        void SetUp() override {
            port = "50051";
            ip = "server";
        }

        std::string port;
        std::string ip;
};

TEST_F(ServerDemo, get_elevation_demo)
{
    ElevationServiceClient elevation_service(grpc::CreateChannel(
        ip + ":" + port, grpc::InsecureChannelCredentials()));

    const std::vector<double> x{1.3, 2, 0, 2.7, 0.5, 0.1, 1.3, 2, 0, 2.7, 0.5, 0.1, 1.3, 2, 0, 2.7, 0.5, 0.1, 1.3, 2};
    const std::vector<double> y{2.7, 0.5, 0, 1.3, 2, 0.2, 1.3, 2, 1, 2.7, 0.5, 0.1, 2.7, 0.5, 0, 1.3, 2, 0.2, 1.3, 2};
    const double t(0.1);

    std::vector<std::chrono::duration<double>> times{};

    ElevationRequest request;
    add_points_to_request(request, x, y);
    request.set_t(t);

    for (size_t index = 0; index < 10; ++index)
    {
        auto start = std::chrono::system_clock::now();
        ElevationResponse reply = elevation_service.get_elevation(request);
        const std::chrono::duration<double> diff = std::chrono::system_clock::now() - start;
        times.push_back(diff);
        std::cout << "Request duration: " << diff.count() << " s." << std::endl;
        EXPECT_DOUBLE_EQ(reply.elevation_points(0).z(), 1.2605762463595371);
    }
}
