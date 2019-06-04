#include "../../taylor_series.hpp"

#include <gtest/gtest.h>

#include <cmath>

namespace fields {
namespace test {

class ExpTest : public ::testing::Test {
protected:
};

TEST_F(ExpTest, Exponential_Is_Correct) {
    using namespace operators;
    using namespace literals;

    constexpr auto H = [] (auto y) { return y; };

    constexpr auto y_0 = 1_c;

    constexpr auto y_exact = [] (double t_) { return std::exp(t_); };

    {
        constexpr auto y0 = taylor_expand<0>(H, y_0);
        constexpr auto y1 = taylor_expand<1>(H, y_0);
        constexpr auto y2 = taylor_expand<2>(H, y_0);
        constexpr auto y3 = taylor_expand<3>(H, y_0);
        static_assert(y0 == 1);
        static_assert(y1 == 1_c + t);
        static_assert(y2 == 1_c + t + (t^2_c)/2_c);
        static_assert(y3 == 1_c + t + (t^2_c)/2_c + (t^3_c)/6_c);

        constexpr auto y = taylor_expand<20>(H, y_0);

        for (auto t_ :{-0.9, -0.75, -0.5, -0.2, -0.1, 0.0, 0.1, 0.2, 0.5, 0.75, 0.9}) {
            EXPECT_NEAR(y(0, 0, t_), y_exact(t_), std::abs(util::pow(t_, 20)));
        }
    }
}

}   // test
}   // fields