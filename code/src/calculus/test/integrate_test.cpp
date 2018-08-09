// #include "../integrate.hpp"
#include "../taylor_series.hpp"

#include "../multiplication.hpp"

#include <gtest/gtest.h>

#include <iostream>
#include <type_traits>

#include <cmath>

namespace fields {
namespace test {

// Class to test the integration of various equations
class IntegrateTest : public ::testing::Test {
 protected:

     template <typename T, typename U>
     constexpr static auto is_same(T, U) {
         static_assert(std::is_same_v<T, U>);
     };

};

TEST_F(IntegrateTest, TimeDerivativeIsCorrect) {
    using namespace operators;
    using namespace literals;
    using namespace fields::detail;

    constexpr auto check_trivial_derivatives = [] (auto phi) {
        // true for any phi
        is_same(d_dt<0>(phi), phi);
        is_same(d_dt<1>(phi), d_dt(phi));
    };

    {
        constexpr auto H = [] (auto phi) { return phi; };

        constexpr auto phi = make_field(H);

        check_trivial_derivatives(phi);

        // only true because of the form of H
        is_same(d_dt<1>(phi), phi);
        is_same(d_dt<10>(phi), phi);
    }

    {
        constexpr auto H = [] (auto phi) { return phi + 5_c; };

        constexpr auto phi = make_field(H);

        check_trivial_derivatives(phi);

        // only true because of the form of H
        is_same(d_dt<1>(phi), phi + 5_c);
        is_same(d_dt<10>(phi), phi + 5_c);
    }

    {
        constexpr auto H = [] (auto phi) { return 2_c*phi; };

        constexpr auto phi = make_field(H);

        check_trivial_derivatives(phi);

        // only true because of the form of H
        is_same(d_dt<1>(phi), 2_c*phi);
        is_same(d_dt<2>(phi), 4_c*phi);
        is_same(d_dt<5>(phi), 32_c*phi);
        is_same(d_dt<10>(phi), 1024_c*phi);
    }

    {
        constexpr auto H = [] (auto phi) { return phi*phi; };

        constexpr auto phi = make_field(H);

        check_trivial_derivatives(phi);

        // only true because of the form of H
        is_same(d_dt<1>(phi), phi*phi);
        is_same(d_dt<2>(phi), Constant<util::factorial(2)>{}*phi*phi*phi);
        is_same(d_dt<5>(phi), Constant<util::factorial(5)>{}*phi*phi*phi*phi*phi*phi);
        is_same(d_dt<10>(phi), Constant<util::factorial(10)>{}*phi*phi*phi*phi*phi*phi*phi*phi*phi*phi*phi);
        // is_same(d_dt<20>(phi), Constant<util::factorial(20)>{}*phi*phi*phi*phi*phi*phi*phi*phi*phi*phi*phi*phi*phi*phi*phi*phi*phi*phi*phi*phi*phi);
    }

}

}   // test
}   // fields