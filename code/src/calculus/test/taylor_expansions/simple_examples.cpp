#include "../../taylor_series.hpp"

#include "../../comparison.hpp"

#include <gtest/gtest.h>

namespace fields {
namespace test {

class SimpleExamplesTest : public ::testing::Test {
protected:
};

TEST_F(SimpleExamplesTest, Series_Expansion_Is_Correct) {
    using namespace literals;

    {
        constexpr auto H = [] (auto) { return 0_c; };

        constexpr auto y_0 = [] {};
        {
            constexpr auto y0 = taylor_expand<0>(H, y_0);
            constexpr auto y1 = taylor_expand<1>(H, y_0);
            constexpr auto y2 = taylor_expand<2>(H, y_0);
            constexpr auto y3 = taylor_expand<3>(H, y_0);
            static_assert(y0 == y_0);
            static_assert(y1 == y0);
            static_assert(y2 == y0);
            static_assert(y3 == y0);
        }
    }
    {
        constexpr auto H = [] (auto) { return 3_c; };

        constexpr auto y_0 = [] {};
        {
            constexpr auto y0 = taylor_expand<0>(H, y_0);
            constexpr auto y1 = taylor_expand<1>(H, y_0);
            constexpr auto y2 = taylor_expand<2>(H, y_0);
            constexpr auto y3 = taylor_expand<3>(H, y_0);
            static_assert(y0 == y_0);
            static_assert(y1 == y_0 + 3_c*t);
            static_assert(y2 == y1);
            static_assert(y3 == y1);
        }

    }
    {
        constexpr auto H = [] (auto) { return 7_c*x - 4_c; };

        constexpr auto y_0 = [] {};
        {
            constexpr auto y0 = taylor_expand<0>(H, y_0);
            constexpr auto y1 = taylor_expand<1>(H, y_0);
            constexpr auto y2 = taylor_expand<2>(H, y_0);
            constexpr auto y3 = taylor_expand<3>(H, y_0);
            static_assert(y0 == y_0);
            static_assert(y1 == y_0 + (7_c*x - 4_c)*t);
            static_assert(y2 == y1);
            static_assert(y3 == y1);
        }
    }
    {
        constexpr auto H = [] (auto) { return t - ((x*t)^2_c)/2_c; };

        constexpr auto y_0 = [] {};
        {
            constexpr auto y0 = taylor_expand<0>(H, y_0);
            constexpr auto y1 = taylor_expand<1>(H, y_0);
            constexpr auto y2 = taylor_expand<2>(H, y_0);
            constexpr auto y3 = taylor_expand<3>(H, y_0);

            static_assert(y0 == y_0);
            static_assert(y1 == y0);
            static_assert(y2 == y1 + (t*t)/2_c);
            static_assert(y3 == y0 + (t*t)/2_c + (-1_c*x*x*(t^3_c))/6_c);
        }
    }
}

}   // test
}   // fields