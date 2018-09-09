#include "../linear.hpp"

#include "../../addition.hpp"
#include "../../multiplication.hpp"
#include "../../comparison.hpp"

#include <gtest/gtest.h>

namespace fields {
namespace test {

class LinearTest : public ::testing::Test {
 protected:
     template <typename T, typename U>
     constexpr static auto is_same(T, U) {
         static_assert(std::is_same_v<T, U>);
     };
};

TEST_F(LinearTest, DerivativeIsCorrect) {
    using namespace operators;
    using namespace literals;

    {
        static_assert(d_dt(t) == 1);
        static_assert(d_dt<1>(t) == 1);
        static_assert(d_dt<2>(t) == 0);
        static_assert(d_dt<30>(t) == 0);

        static_assert(d_dx(x) == 1);
        static_assert(d_dx<1>(x) == 1);
        static_assert(d_dx<2>(x) == 0);
        static_assert(d_dx<30>(x) == 0);

        static_assert(d_dx(t) == 0);
        static_assert(d_dt(x) == 0);
        static_assert(d_dx<3>(t) == 0);
        static_assert(d_dt<7>(x) == 0);

        static_assert(d_dt<2>(x + t) == 0);
        static_assert(d_dx<4>(x - t) == 0);
    }

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