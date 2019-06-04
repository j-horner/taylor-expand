#include "../../taylor_series.hpp"

#include <gtest/gtest.h>

#include <cmath>

namespace fields {
namespace test {

class LogTest : public ::testing::Test {
protected:
};

TEST_F(LogTest, Logarithm_Is_Correct) {
    using namespace operators;
    using namespace literals;

    constexpr auto H = [] (auto) { return 1_c/t; };

    constexpr auto y_0 = 0_c;
    constexpr auto t_0 = 1_c;

    constexpr auto y_exact = [] (double t_) { return std::log(t_); };

    {
        constexpr auto y0 = taylor_expand<0>(H, y_0, t_0);
        constexpr auto y1 = taylor_expand<1>(H, y_0, t_0);
        constexpr auto y2 = taylor_expand<2>(H, y_0, t_0);
        constexpr auto y3 = taylor_expand<3>(H, y_0, t_0);

        {
            constexpr auto t_ = t - t_0;

            static_assert(y0 == 0);
            static_assert(y1 == t_);
            static_assert(y2 == t_ + (-1_c*(t_^2_c))/2_c);
            static_assert(y3 == t_ + (-1_c*(t_^2_c))/2_c + (t_^3_c)/3_c);
        }

        constexpr auto y = taylor_expand<20>(H, y_0, t_0);

        for (auto t_ :{-0.9, -0.75, -0.5, -0.2, -0.1, 0.0, 0.1, 0.2, 0.5, 0.75, 0.9}) {
            const auto tau = 1.0 + t_;
            EXPECT_NEAR(y(0, 0, tau), y_exact(tau), std::abs(util::pow(t_, 15)));
        }
    }
}
}   // test
}   // fields