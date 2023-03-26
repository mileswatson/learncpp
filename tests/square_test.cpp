#include <gtest/gtest.h>
#include <learncpp/square/square.hpp>

// Demonstrate some basic assertions.
TEST(SquareTest, BasicAssertions)
{
    EXPECT_EQ(square(0), 0);
    EXPECT_EQ(square(5), 25);
    EXPECT_EQ(square(-2), 4);
}
