// #include "../integrate.hpp"
#include "../taylor_series.hpp"

#include "../functions/linear.hpp"
#include "../multiplication.hpp"

#include <gtest/gtest.h>

#include <iostream>
#include <type_traits>

#include <cmath>

// helper struct to make compiler error messages more legible when working with lambdas
// struct H {
//     template <typename T> constexpr auto operator()(T phi) const {
//         using namespace fields;
//         using namespace operators;
//         return (x + t*t)*phi;
//     }
// };

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
        using namespace operators;

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
        is_same(d_dt<1>(phi), factorial(1_c)*phi*phi);
        is_same(d_dt<2>(phi), factorial(2_c)*phi*phi*phi);
        is_same(d_dt<5>(phi), factorial(5_c)*phi*phi*phi*phi*phi*phi);
        is_same(d_dt<10>(phi),factorial(10_c)*phi*phi*phi*phi*phi*phi*phi*phi*phi*phi*phi);

        // Compiler has a heart attack...
        // is_same(d_dt<20>(phi), factorial(20_c)*phi*phi*phi*phi*phi*phi*phi*phi*phi*phi*phi*phi*phi*phi*phi*phi*phi*phi*phi*phi*phi);
    }

    {
        constexpr auto H = [] (auto) { return x + t; };

        constexpr auto phi = make_field(H);

        check_trivial_derivatives(phi);

        // only true because of the form of H
        static_assert(d_dt(phi) == x + t);
        static_assert(d_dt<2>(phi) == 1);
        static_assert(d_dt<3>(phi) == 0);
        static_assert(d_dt<5>(phi) == 0);
        static_assert(d_dt<10>(phi) == 0);
        static_assert(d_dt<20>(phi) == 0);
    }

    {
        constexpr auto H = [] (auto phi) { return (x + t*t)*phi; };

        constexpr auto phi = make_field(H);

        check_trivial_derivatives(phi);

        is_same(d_dt(phi), (x + t*t)*phi);
        is_same(d_dt<2>(phi), 2_c*t*phi + (x + t*t)*(x + t*t)*phi);

        // NOTE:    During d_dt<3>(phi), the code currently does not make the following simplification.
        //
        // From:
        // Addition<Multiplication<Constant<2,1>,Phi>,
        //         Multiplication<Constant<2,1>,T,Addition<X,Multiplication<T,T>>,Phi>,
        //         Multiplication<Constant<2,1>,T,Addition<X,Multiplication<T,T>>,Phi>,
        //         Multiplication<Constant<2,1>,Addition<X,Multiplication<T,T>>,T,Phi>,
        //         Multiplication<Addition<X,Multiplication<T,T>>,Addition<X,Multiplication<T,T>>,Addition<X,Multiplication<T,T>>,Phi>>
        //
        // To:
        // Addition<Multiplication<Constant<2,1>,Phi>,
        //         Multiplication<Constant<4,1>,T,Addition<X,Multiplication<T,T>>,Phi>,
        //         Multiplication<Constant<2,1>,Addition<X,Multiplication<T,T>>,T,Phi>,
        //         Multiplication<Addition<X,Multiplication<T,T>>,Addition<X,Multiplication<T,T>>,Addition<X,Multiplication<T,T>>,Phi>>
        //
        is_same(d_dt<3>(phi), ((2_c*phi) + (2_c*t*(x + t*t)*phi)) + ((2_c*t*(x + t*t)*phi) + (2_c*(x + t*t)*t*phi) + ((x + t*t)*(x + t*t)*(x + t*t)*phi)));
    }

    {
        constexpr auto H = [] (auto phi) { return d_dx<2>(phi); };

        constexpr auto phi = make_field(H);

        check_trivial_derivatives(phi);

        is_same(d_dt<1>(phi), d_dx<2>(phi));
        is_same(d_dt<2>(phi), d_dx<4>(phi));
        is_same(d_dt<5>(phi), d_dx<10>(phi));
        is_same(d_dt<10>(phi), d_dx<20>(phi));
        is_same(d_dt<20>(phi), d_dx<40>(phi));
    }

}

}   // test
}   // fields