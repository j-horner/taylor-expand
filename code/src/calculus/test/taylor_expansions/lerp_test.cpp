#include "../../taylor_series.hpp"

#include <gtest/gtest.h>

namespace fields {
namespace test {

class LerpTest : public ::testing::Test {
protected:
};

TEST_F(LerpTest, Lerp_Is_Correct) {
    using namespace operators;
    using namespace literals;

    constexpr auto a = 4.3_c;
    constexpr auto b = 7.9_c;

    constexpr auto H = [a, b] (auto) { return b - a; };

    constexpr auto y_0 = a;

    constexpr auto y_exact = [a, b] (double t_) { return a + (b - a)*t_; };     // TODO : use std::lerp(a, b, t_); in C++20

    {
        constexpr auto y0 = taylor_expand<0>(H, y_0);
        constexpr auto y1 = taylor_expand<1>(H, y_0);
        constexpr auto y2 = taylor_expand<2>(H, y_0);
        constexpr auto y3 = taylor_expand<3>(H, y_0);
        constexpr auto y = taylor_expand<20>(H, y_0);

        static_assert(y0 == a);
        static_assert(y1 == a + (b - a)*t);
        static_assert(y2 == a + (b - a)*t);
        static_assert(y3 == a + (b - a)*t);
        static_assert(y == a + (b - a)*t);

        for (auto t_ :{-0.9, -0.75, -0.5, -0.2, -0.1, 0.0, 0.1, 0.2, 0.5, 0.75, 0.9}) {
            EXPECT_NEAR(y(0, 0, t_), y_exact(t_), std::abs(util::pow(t_, 15)));
        }
    }
}

}   // test
}   // fields