#include "gtest/gtest.h"

class Demo : public ::testing::Test
{
};

TEST_F(Demo, simple_demo)
{
    ASSERT_EQ("NED", "NED");
}

