#include "../linear.hpp"

#include <gtest/gtest.h>

#include "../../addition.hpp"
#include "../../multiply.hpp"

namespace fields {
namespace test {

class LinearTest : public ::testing::Test {
};

TEST_F(LinearTest, DerivativeIsCorrect) {
    using namespace operators;
    using namespace literals;

    {
        constexpr auto y = x;
        static_assert(y(10) == 10, "x(10) != 10");
        static_assert(d_dx(y) == 1, "dx/dx != 1");
    }
    {
        constexpr auto y = x + x + x + x;

        static_assert(y == 4_c*x, "y != 4x");
        static_assert(y(2) == 8, "4x(2) != 8");
        static_assert(d_dx(y) == 4, "d(4x)/dx != 4");
    }
    {
        constexpr auto y = x - x;

        static_assert(y == 0, "0x != 0");
        static_assert(y(23) == 0, "0x(23) != 0");
        static_assert(d_dx(y) == 0, "d(0x)/dx != 0");
    }
    {
        constexpr auto y = x + x - x + x + x + x - x - x - x + x - x - x;

        static_assert(y == 0, "0x != 0");
        static_assert(y(23) == 0, "0x(23) != 0");
        static_assert(d_dx(y) == 0, "d(0x)/dx != 0");
    }
    {
        constexpr auto y = x*x;

        static_assert(y(2) == 4, "x^2(2) != 4");
        static_assert(d_dx(y) == 2_c*x, "d(4x)/dx != 4x^3");
    }
    {
        constexpr auto y = x/x;

        static_assert(y(2) == 1, "1(2) != 1");
        static_assert(d_dx(y) == 0, "d(1)/dx != 0");
    }
    {
        constexpr auto y = x*x*x;

        static_assert(y(4) == 64, "x^3(4) != 64");
        static_assert(d_dx(y) == 3_c*x*x, "d(x^3)/dx != 3x^2");
    }
    {
        constexpr auto y = x*x*x*x*x*x;

        static_assert(y(4) == 4096, "x^6(4) != 4096");
        static_assert(d_dx(y) == 6_c*x*x*x*x*x, "d(x^6)/dx != 6x^5");
    }



}

}
}