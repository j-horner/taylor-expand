#include "../../taylor_series.hpp"

#include <gtest/gtest.h>

#include <cmath>

namespace fields {
namespace test {

// Class to test the integration of various equations
class TanTest : public ::testing::Test {
 protected:
};

TEST_F(TanTest, Tan_Is_Correct) {
    using namespace operators;
    using namespace literals;

    constexpr auto H = [] (auto y) { return 1_c + y*y; };

    constexpr auto y_0 = 0_c;

    constexpr auto y_exact = [] (double t_) { return std::tan(t_); };

    {
        constexpr auto y0 = taylor_expand<0>(H, y_0);
        constexpr auto y1 = taylor_expand<1>(H, y_0);
        constexpr auto y2 = taylor_expand<2>(H, y_0);
        constexpr auto y3 = taylor_expand<3>(H, y_0);
        constexpr auto y5 = taylor_expand<5>(H, y_0);
        constexpr auto y7 = taylor_expand<7>(H, y_0);

        static_assert(y0 == 0);
        static_assert(y1 == t);
        static_assert(y2 == t);
        static_assert(y3 == t + (t^3_c)/3_c);
        static_assert(y5 == t + (t^3_c)/3_c + (2_c/15_c)*(t^5_c));
        static_assert(y7 == t + (t^3_c)/3_c + (2_c/15_c)*(t^5_c) + (17_c/315_c)*(t^7_c));

        constexpr auto y = taylor_expand<9>(H, y_0);

        for (auto t_ : {-0.9, -0.75, -0.5, -0.2, -0.1, 0.0, 0.1, 0.2, 0.5, 0.75, 0.9}) {
            EXPECT_NEAR(y(0, 0, t_), y_exact(t_), std::abs(util::pow(t_, 5)));
        }
    }
}

}   // test
}   // fields