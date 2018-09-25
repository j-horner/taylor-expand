#include "../taylor_series.hpp"

#include "../multiplication.hpp"
#include "../comparison.hpp"
#include "../functions/linear.hpp"


#include <gtest/gtest.h>

#include <iostream>
#include <type_traits>
#include <vector>

#include <cmath>

namespace fields {
namespace test {

// Class to test the integration of various equations
class TaylorSeriesTest : public ::testing::Test {
 protected:
};

TEST_F(TaylorSeriesTest, Series_Expansion_Is_Correct) {
    using namespace operators;
    using namespace literals;

    {
        constexpr auto H = [] (auto) { return 0_c; };

        constexpr auto phi_0 = [] {};
        {
                constexpr auto y0 = taylor_expand<0>(H, phi_0);
                constexpr auto y1 = taylor_expand<1>(H, phi_0);
                constexpr auto y2 = taylor_expand<2>(H, phi_0);
                constexpr auto y3 = taylor_expand<3>(H, phi_0);
                static_assert(y0 == phi_0);
                static_assert(y1 == y0);
                static_assert(y2 == y0);
                static_assert(y3 == y0);
        }
    }
    {
        constexpr auto H = [] (auto) { return 3_c; };

        constexpr auto phi_0 = [] {};
        {
                constexpr auto y0 = taylor_expand<0>(H, phi_0);
                constexpr auto y1 = taylor_expand<1>(H, phi_0);
                constexpr auto y2 = taylor_expand<2>(H, phi_0);
                constexpr auto y3 = taylor_expand<3>(H, phi_0);
                static_assert(y0 == phi_0);
                static_assert(y1 == phi_0 + 3_c*t);
                static_assert(y2 == y1);
                static_assert(y3 == y1);
        }

    }
    {
        constexpr auto H = [] (auto) { return 7_c*x - 4_c; };

        constexpr auto phi_0 = [] {};
        {
                constexpr auto y0 = taylor_expand<0>(H, phi_0);
                constexpr auto y1 = taylor_expand<1>(H, phi_0);
                constexpr auto y2 = taylor_expand<2>(H, phi_0);
                constexpr auto y3 = taylor_expand<3>(H, phi_0);
                static_assert(y0 == phi_0);
                static_assert(y1 == phi_0 + (7_c*x - 4_c)*t);
                static_assert(y2 == y1);
                static_assert(y3 == y1);
        }
    }
    {
        constexpr auto H = [] (auto) { return t - ((x*t)^2_c)/2_c; };

        constexpr auto phi_0 = [] {};
        {
                constexpr auto y0 = taylor_expand<0>(H, phi_0);
                constexpr auto y1 = taylor_expand<1>(H, phi_0);
                constexpr auto y2 = taylor_expand<2>(H, phi_0);
                constexpr auto y3 = taylor_expand<3>(H, phi_0);

                static_assert(y0 == phi_0);
                static_assert(y1 == y0);
                static_assert(y2 == y1 + (t*t)/2_c);
                static_assert(y3 == y0 + (t*t)/2_c + (-1_c*x*x*(t^3_c))/6_c);
        }
    }
    {
        constexpr auto H = [] (auto) { return 1_c/(1_c + t); };

        constexpr auto phi_0 = 0_c;
        {
                constexpr auto y0 = taylor_expand<0>(H, phi_0);
                constexpr auto y1 = taylor_expand<1>(H, phi_0);
                constexpr auto y2 = taylor_expand<2>(H, phi_0);
                constexpr auto y3 = taylor_expand<3>(H, phi_0);

                static_assert(y0 == 0);
                static_assert(y1 == t);
                static_assert(y2 == t + (-1_c*t*t)/2_c);
                static_assert(y3 == t + (-1_c*t*t)/2_c + (t^3_c)/3_c);
        }
    }
    {
        constexpr auto H = [] (auto phi) { return phi; };

        {
            constexpr auto phi_0 = 1_c;

            {
                constexpr auto y0 = taylor_expand<0>(H, phi_0);
                constexpr auto y1 = taylor_expand<1>(H, phi_0);
                constexpr auto y2 = taylor_expand<2>(H, phi_0);
                constexpr auto y3 = taylor_expand<3>(H, phi_0);
                static_assert(y0 == 1);
                static_assert(y1 == 1_c + t);
                static_assert(y2 == 1_c + t + (t^2_c)/2_c);
                static_assert(y3 == 1_c + t + (t^2_c)/2_c + (t^3_c)/6_c);
            }
        }
    }
    {
        constexpr auto H = [] (auto phi) { return d_dx<2>(phi); };

        constexpr auto phi_0 = 1_c/(1_c + x);
        {
                constexpr auto y0 = taylor_expand<0>(H, phi_0);
                constexpr auto y1 = taylor_expand<1>(H, phi_0);
                constexpr auto y2 = taylor_expand<2>(H, phi_0);
                constexpr auto y3 = taylor_expand<3>(H, phi_0);
                static_assert(y0 == phi_0);
                static_assert(y1 == phi_0 + d_dx<2>(phi_0)*t);
                static_assert(y2 == phi_0 + d_dx<2>(phi_0)*t + d_dx<4>(phi_0)*(t*t)/2_c);
                static_assert(y3 == phi_0 + d_dx<2>(phi_0)*t + d_dx<4>(phi_0)*(t*t)/2_c + d_dx<6>(phi_0)*(t^3_c)/6_c);
        }
    }
}

}   // test
}   // fields