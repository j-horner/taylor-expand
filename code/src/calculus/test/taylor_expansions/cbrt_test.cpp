#include "../../taylor_series.hpp"

#include "../../comparison.hpp"

#include <gtest/gtest.h>

#include <cmath>

namespace fields {
namespace test {

class CbrtTest : public ::testing::Test {
protected:
};

TEST_F(CbrtTest, Cbrt_Is_correct) {
    using namespace literals;

    constexpr auto H = [] (auto y) { return (1_c/3_c)*(y^(-2_c)); };

    constexpr auto y_0 = 1_c;
    constexpr auto t_0 = 1_c;

    constexpr auto y_exact = [] (double t_) { return std::cbrt(t_); };

    {
        constexpr auto y0 = taylor_expand<0>(H, y_0, t_0);
        constexpr auto y1 = taylor_expand<1>(H, y_0, t_0);
        constexpr auto y2 = taylor_expand<2>(H, y_0, t_0);
        constexpr auto y3 = taylor_expand<3>(H, y_0, t_0);

        {
            constexpr auto t_ = t - t_0;

            static_assert(y0 == 1);
            static_assert(y1 == 1_c + (1_c/3_c)*t_);
            static_assert(y2 == 1_c + (1_c/3_c)*t_ + (-1_c/9_c)*(t_^2_c));
            static_assert(y3 == 1_c + (1_c/3_c)*t_ + (-1_c/9_c)*(t_^2_c) + (5_c/81_c)*(t_^3_c));
        }

        // N = 17 --> integer overflow!
		constexpr auto y = taylor_expand<16>(H, y_0, t_0);

        for (auto t_ : {-0.9, -0.5, -0.1, 0.0, 0.1, 0.5, 0.9}) {
            const auto tau = 1.0 + t_;

            EXPECT_NEAR(y(0, 0, tau), y_exact(tau), std::abs(util::pow(t_, 15))) << "t = " << tau << std::endl;
        }
    }
}

}   // test
}   // fields