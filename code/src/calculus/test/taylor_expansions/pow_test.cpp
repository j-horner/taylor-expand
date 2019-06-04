#include "../../taylor_series.hpp"

#include <gtest/gtest.h>

#include <cmath>

namespace fields {
namespace test {

class PowTest : public ::testing::Test {
protected:
};

TEST_F(PowTest, Pow_Is_correct) {
    using namespace operators;
    using namespace literals;

    constexpr auto H = [] (auto y) { return x*y/t; };

    constexpr auto y_0 = 1_c;
    constexpr auto t_0 = 1_c;

    constexpr auto y_exact = [] (double t_, double x_) { return std::pow(t_, x_); };

    {
        constexpr auto y0 = taylor_expand<0>(H, y_0, t_0);
        constexpr auto y1 = taylor_expand<1>(H, y_0, t_0);
        constexpr auto y2 = taylor_expand<2>(H, y_0, t_0);
        constexpr auto y3 = taylor_expand<3>(H, y_0, t_0);

        {
            constexpr auto t_ = t - t_0;

            static_assert(y0 == 1);
            static_assert(y1 == 1_c + x*t_);
            static_assert(y2 == 1_c + x*t_ + (1_c/2_c)*(x*x + (-1_c*x))*(t_^2_c));
            static_assert(y3 == 1_c + x*t_ + (1_c/2_c)*(x*x + (-1_c*x))*(t_^2_c) + (1_c/6_c)*((x^3_c) + (-2_c*(x^2_c)) + x + (-1_c)*(x*x + (-1_c*x)))*(t_^3_c));
        }
        /*constexpr auto y = taylor_expand<4>(H, y_0, t_0);

        for (auto t_ : {-0.9, -0.75, -0.5, -0.2, -0.1, 0.0, 0.1, 0.2, 0.5, 0.75, 0.9}) {
            EXPECT_NEAR(y(0, 0, t_), y_exact(t_), std::abs(util::pow(t_, 20)));
        }*/
    }
}

}   // test
}   // fields