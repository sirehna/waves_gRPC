#include "gtest/gtest.h"
#include <chrono>
#include "wave_client.hh"

class ServerDemo : public ::testing::Test
{
};

TEST_F(ServerDemo, get_elevation_demo)
{
    std::string port("50051");
    std::string ip("server");
    ElevationServiceClient elevation_service(grpc::CreateChannel(
        ip + ":" + port, grpc::InsecureChannelCredentials()));

    const std::vector<double> x{1.3, 2, 0};
    const std::vector<double> y{2.7, 0.5, 0};
    const double t(0.1);

    auto start = std::chrono::system_clock::now();

    ElevationResponse reply = elevation_service.get_elevation(x, y, t);

    std::chrono::duration<double> diff = std::chrono::system_clock::now() - start;
    std::cout << "elevation_service.get_elevation took " << diff.count() << " s." << std::endl;

    EXPECT_DOUBLE_EQ(reply.elevation_points(0).z(), -0.053363484126936325);
}
