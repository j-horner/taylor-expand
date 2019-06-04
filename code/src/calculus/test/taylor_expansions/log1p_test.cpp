#include "../../taylor_series.hpp"

#include <gtest/gtest.h>

#include <cmath>

namespace fields {
namespace test {

class Log1PTest : public ::testing::Test {
protected:
};

TEST_F(Log1PTest, Logarithm_1_Plus_T_Is_Correct) {
    using namespace operators;
    using namespace literals;

    constexpr auto H = [] (auto) { return 1_c/(1_c + t); };

    constexpr auto y_0 = 0_c;

    constexpr auto y_exact = [] (double t_) { return std::log1p(t_); };

    {
        constexpr auto y0 = taylor_expand<0>(H, y_0);
        constexpr auto y1 = taylor_expand<1>(H, y_0);
        constexpr auto y2 = taylor_expand<2>(H, y_0);
        constexpr auto y3 = taylor_expand<3>(H, y_0);

        static_assert(y0 == 0);
        static_assert(y1 == t);
        static_assert(y2 == t + (-1_c*t*t)/2_c);
        static_assert(y3 == t + (-1_c*t*t)/2_c + (t^3_c)/3_c);

        constexpr auto y = taylor_expand<20>(H, y_0);

        for (auto t_ :{-0.9, -0.75, -0.5, -0.2, -0.1, 0.0, 0.1, 0.2, 0.5, 0.75, 0.9}) {
            EXPECT_NEAR(y(0, 0, t_), y_exact(t_), std::abs(util::pow(t_, 15)));
        }
    }
}

}   // test
}   // fields