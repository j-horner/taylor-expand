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

    static_assert(d_dx(a + b) == da_dx + db_dx, "d_dx(a + b) != da_dx + db_dx");
    static_assert(d_dx(a + b + c) == da_dx + db_dx + dc_dx, "d_dx(a + b + c) != da_dx + db_dx + dc_dx");
    static_assert(d_dx(a + b + c + d) == da_dx + db_dx + dc_dx + dd_dx, "d_dx(a + b + c + d) != da_dx + db_dx + dc_dx + dd_dx");

    static_assert(d_dx(a - b) == da_dx - db_dx);
}

TEST_F(DerivativeTest, Derivative_Of_Products_Is_Correct) {
    constexpr auto da_dx = dA_dx{};
    constexpr auto db_dx = dB_dx{};
    constexpr auto dc_dx = dC_dx{};
    constexpr auto dd_dx = dD_dx{};

    static_assert(d_dx(a*b) == da_dx*b + a*db_dx);
    static_assert(d_dx(a*b*c) == da_dx*b*c + a*db_dx*c + a*b*dc_dx);
    static_assert(d_dx(a*b*c*d) == da_dx*b*c*d + a*db_dx*c*d + a*b*dc_dx*d + a*b*c*dd_dx);

    static_assert(d_dx(a/b) == (da_dx*b - a*db_dx)/(b*b));
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
        static_assert(y(10) == 10, "x(10) != 10");
        static_assert(d_dx(y) == 1, "dx/dx != 1");
    }
    {
        constexpr auto y = x + x + x + x;

        static_assert(y == 4_c*x, "y != 4x");
        static_assert(y(2) == 8, "4x(2) != 8");
        static_assert(d_dx(y) == 4, "d(4x)/dx != 4");
    }
    {
        constexpr auto y = x - x;

        static_assert(y == 0, "0x != 0");
        static_assert(y(23) == 0, "0x(23) != 0");
        static_assert(d_dx(y) == 0, "d(0x)/dx != 0");
    }
    {
        constexpr auto y = x + x - x + x + x + x - x - x - x + x - x - x;

        static_assert(y == 0, "0x != 0");
        static_assert(y(23) == 0, "0x(23) != 0");
        static_assert(d_dx(y) == 0, "d(0x)/dx != 0");
    }
    {
        constexpr auto y = x*x;

        static_assert(y(2) == 4, "x^2(2) != 4");
        static_assert(d_dx(y) == 2_c*x, "d(4x)/dx != 4x^3");
    }
    {
        constexpr auto y = x/x;

        static_assert(y(2) == 1, "1(2) != 1");
        static_assert(d_dx(y) == 0, "d(1)/dx != 0");
    }
    {
        constexpr auto y = x*x*x;

        static_assert(y(4) == 64, "x^3(4) != 64");
        static_assert(d_dx(y) == 3_c*x*x, "d(x^3)/dx != 3x^2");
    }
    {
        constexpr auto y = x*x*x*x*x*x;

        static_assert(y(4) == 4096, "x^6(4) != 4096");
        static_assert(d_dx(y) == 6_c*x*x*x*x*x, "d(x^6)/dx != 6x^5");
    }
}

}   // test
}   // fields