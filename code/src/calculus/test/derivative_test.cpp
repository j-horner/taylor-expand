#include "../derivative.hpp"

#include "../comparison.hpp"
#include "../functions/linear.hpp"

#include <gtest/gtest.h>

namespace fields {

namespace operators {

struct A {
    template <typename T> constexpr auto operator()(T) const { return 3; }
};
struct B {
    template <typename T> constexpr auto operator()(T x) const { return 2*x; }
};
struct C {
    template <typename T> constexpr auto operator()(T x) const { return 4*x; }
};
struct D {
    template <typename T> constexpr auto operator()(T x) const { return x*x; }
};

template <Int N = 1> struct dA_dx {};
template <Int N = 1> struct dB_dx {};
template <Int N = 1> struct dC_dx {};
template <Int N = 1> struct dD_dx {};

template <Int N = 1> constexpr auto d_dx(A) { return dA_dx<N>{}; }
template <Int N = 1> constexpr auto d_dx(B) { return dB_dx<N>{}; }
template <Int N = 1> constexpr auto d_dx(C) { return dC_dx<N>{}; }
template <Int N = 1> constexpr auto d_dx(D) { return dD_dx<N>{}; }

}

namespace test {

using namespace operators;

class DerivativeTest : public ::testing::Test {
protected:
    constexpr static auto a = A{};
    constexpr static auto b = B{};
    constexpr static auto c = C{};
    constexpr static auto d = D{};
};

TEST_F(DerivativeTest, Derivative_Of_Sums_Is_Correct) {
    constexpr auto da_dx = dA_dx{};
    constexpr auto db_dx = dB_dx{};
    constexpr auto dc_dx = dC_dx{};
    constexpr auto dd_dx = dD_dx{};

    static_assert(d_dx(a + b) == da_dx + db_dx);
    static_assert(d_dx(a + b + c) == da_dx + db_dx + dc_dx);
    static_assert(d_dx(a + b + c + d) == da_dx + db_dx + dc_dx + dd_dx);

    static_assert(d_dx(a - b) == da_dx - db_dx);
}

TEST_F(DerivativeTest, Derivative_Of_Products_Is_Correct) {
    using namespace literals;

    constexpr auto da_dx = dA_dx{};
    constexpr auto db_dx = dB_dx{};
    constexpr auto dc_dx = dC_dx{};
    constexpr auto dd_dx = dD_dx{};

    static_assert(d_dx(a*b) == da_dx*b + a*db_dx);
    static_assert(d_dx(a*b*c) == da_dx*b*c + a*db_dx*c + a*b*dc_dx);
    static_assert(d_dx(a*b*c*d) == da_dx*b*c*d + a*db_dx*c*d + a*b*dc_dx*d + a*b*c*dd_dx);

    static_assert(d_dx(a/b) == da_dx*(b^-1_c) + (-a)*(b^-2_c)*db_dx);
}

TEST_F(DerivativeTest, Derivative_Of_Linear_Is_Correct) {
    using namespace literals;

    {
        static_assert(d_dt(t) == 1);
        static_assert(d_dt<1>(t) == 1);
        static_assert(d_dt<2>(t) == 0);
        static_assert(d_dt<30>(t) == 0);

        static_assert(d_dx(x) == 1);
        static_assert(d_dx<1>(x) == 1);
        static_assert(d_dx<2>(x) == 0);
        static_assert(d_dx<30>(x) == 0);

        static_assert(d_dx(t) == 0);
        static_assert(d_dt(x) == 0);
        static_assert(d_dx<3>(t) == 0);
        static_assert(d_dt<7>(x) == 0);

        static_assert(d_dt<2>(x + t) == 0);
        static_assert(d_dx<4>(x - t) == 0);
    }

    {
        constexpr auto y = x;
        static_assert(d_dx(y) == 1);
    }
    {
        constexpr auto y = x + x + x + x;

        static_assert(y == 4_c*x);
        static_assert(d_dx(y) == 4);
    }
    {
        constexpr auto y = x - x;

        static_assert(y == 0);
        static_assert(d_dx(y) == 0);
    }
    {
        constexpr auto y = x + x - x + x + x + x - x - x - x + x - x - x;

        static_assert(y == 0);
        static_assert(d_dx(y) == 0);
    }
    {
        constexpr auto y = x*x;
        static_assert(d_dx(y) == 2_c*x);
    }
    {
        constexpr auto y = x/x;
        static_assert(d_dx(y) == 0);
    }
    {
        constexpr auto y = x*x*x;
        static_assert(d_dx(y) == 3_c*x*x);
    }
    {
        constexpr auto y = x*x*x*x*x*x;
        static_assert(d_dx(y) == 6_c*x*x*x*x*x);
    }
}

TEST_F(DerivativeTest, Time_Derivative_Of_Fields_Is_Correct) {
    using namespace operators;
    using namespace literals;
    using namespace fields::detail;

    {
        constexpr auto H = [] (auto phi) { return phi; };

        constexpr auto phi = make_field(H);

        static_assert(d_dt(phi) == H(phi));

        static_assert(d_dt<0>(phi) == phi);
        static_assert(d_dt<1>(phi) == d_dt(phi));

        // only true because of the form of H
        static_assert(d_dt<1>(phi) == phi);
        static_assert(d_dt<10>(phi) == phi);
    }
    {
        constexpr auto H = [] (auto phi) { return phi + 5_c; };

        constexpr auto phi = make_field(H);

        static_assert(d_dt(phi) == H(phi));

        static_assert(d_dt<0>(phi) == phi);
        static_assert(d_dt<1>(phi) == d_dt(phi));

        // only true because of the form of H
        static_assert(d_dt<1>(phi) == phi + 5_c);
        static_assert(d_dt<10>(phi) == phi + 5_c);
    }
    {
        constexpr auto H = [] (auto phi) { return 2_c*phi; };

        constexpr auto phi = make_field(H);

        static_assert(d_dt(phi) == H(phi));

        static_assert(d_dt<0>(phi) == phi);
        static_assert(d_dt<1>(phi) == d_dt(phi));

        // only true because of the form of H
        static_assert(d_dt<1>(phi) == 2_c*phi);
        static_assert(d_dt<2>(phi) == 4_c*phi);
        static_assert(d_dt<5>(phi) == 32_c*phi);
        static_assert(d_dt<10>(phi) == 1024_c*phi);
    }
    {
        constexpr auto H = [] (auto phi) { return phi*phi; };

        constexpr auto phi = make_field(H);

        static_assert(d_dt(phi) == H(phi));

        static_assert(d_dt<0>(phi) == phi);
        static_assert(d_dt<1>(phi) == d_dt(phi));

        // only true because of the form of H
        static_assert(d_dt<1>(phi) == factorial(1_c)*phi*phi);
        static_assert(d_dt<2>(phi) == factorial(2_c)*phi*phi*phi);
        static_assert(d_dt<5>(phi) == factorial(5_c)*phi*phi*phi*phi*phi*phi);
        static_assert(d_dt<10>(phi) == factorial(10_c)*phi*phi*phi*phi*phi*phi*phi*phi*phi*phi*phi);
        static_assert(d_dt<20>(phi) == factorial(20_c)*phi*phi*phi*phi*phi*phi*phi*phi*phi*phi*phi*phi*phi*phi*phi*phi*phi*phi*phi*phi*phi);
    }
    {
        constexpr auto H = [] (auto) { return x + t; };

        constexpr auto phi = make_field(H);

        static_assert(d_dt(phi) == H(phi));

        static_assert(d_dt<0>(phi) == phi);
        static_assert(d_dt<1>(phi) == d_dt(phi));

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

        static_assert(d_dt(phi) == H(phi));

        static_assert(d_dt<0>(phi) == phi);
        static_assert(d_dt<1>(phi) == d_dt(phi));

        static_assert(d_dt(phi) == (x + t*t)*phi);

        static_assert(d_dt<2>(phi) == 2_c*t*phi + (x + t*t)*(x + t*t)*phi);

        // NOTE:    During d_dt<3>(phi), the code currently does not do a full simplification
        static_assert(d_dt<3>(phi) == ((2_c*phi) + (2_c*t*(x + t*t)*phi)) + ((4_c*(x + t*t)*t*phi) + ((x + t*t)*(x + t*t)*(x + t*t)*phi)));
    }
    {
        constexpr auto H = [] (auto phi) { return d_dx<2>(phi); };

        constexpr auto phi = make_field(H);

        static_assert(d_dt(phi) == H(phi));

        static_assert(d_dt<0>(phi) == phi);
        static_assert(d_dt<1>(phi) == d_dt(phi));

        static_assert(d_dt<1>(phi) == d_dx<2>(phi));
        static_assert(d_dt<2>(phi) == d_dx<4>(phi));
        static_assert(d_dt<5>(phi) == d_dx<10>(phi));
        static_assert(d_dt<10>(phi) == d_dx<20>(phi));
        static_assert(d_dt<20>(phi) == d_dx<40>(phi));
        static_assert(d_dt<100>(phi) == d_dx<200>(phi));     // compiles but takes a few seconds
    }
}

}   // test
}   // fields