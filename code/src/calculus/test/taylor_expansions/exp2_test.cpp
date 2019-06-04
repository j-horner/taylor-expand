#include "../../taylor_series.hpp"

#include <gtest/gtest.h>

#include <cmath>

namespace fields {
namespace test {

class Exp2Test : public ::testing::Test {
protected:
};

TEST_F(Exp2Test, Exponential_2_Is_Correct) {
    using namespace operators;
    using namespace literals;

    // constexpr static auto ln2 = 0.69314718055994530941723212145818_c;
    constexpr static auto ln2 = 0.6931_c;

    constexpr auto H = [] (auto y) { return ln2*y; };

    constexpr auto y_0 = 1_c;

    constexpr auto y_exact = [] (double t_) { return std::exp2(t_); };

    {
        constexpr auto y0 = taylor_expand<0>(H, y_0);
        constexpr auto y1 = taylor_expand<1>(H, y_0);
        constexpr auto y2 = taylor_expand<2>(H, y_0);
        constexpr auto y3 = taylor_expand<3>(H, y_0);

        constexpr auto tau = ln2*t;

        static_assert(y0 == 1);
        static_assert(y1 == 1_c + tau);
        static_assert(y2 == 1_c + tau + (tau^2_c)/2_c);
        static_assert(y3 == 1_c + tau + (tau^2_c)/2_c + (tau^3_c)/6_c);

        constexpr auto y = taylor_expand<4>(H, y_0);

        for (auto t_ :{-0.9, -0.75, -0.5, -0.2, -0.1, 0.0, 0.1, 0.2, 0.5, 0.75, 0.9}) {
            EXPECT_NEAR(y(0, 0, t_), y_exact(t_), std::abs(util::pow(t_, 4)));
        }
    }
}

}   // test
}   // fields