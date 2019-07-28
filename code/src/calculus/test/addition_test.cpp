#include "../addition.hpp"

#include "../comparison.hpp"

#include <gtest/gtest.h>

namespace fields {
namespace test {

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

class AdditionTest : public ::testing::Test {
 protected:
};

TEST_F(AdditionTest, Addition_Is_Correct) {
    using namespace operators;
    using namespace literals;

	constexpr auto a = A{};
	constexpr auto b = B{};
	constexpr auto c = C{};
	constexpr auto d = D{};

    {
        constexpr auto y = a + b + c + d;   // x^2 + 6x + 3

        static_assert(y(2) == 19);

        static_assert(a == y.get<0>());
        static_assert(d == y.get<3>());

        static_assert(b + c == y.sub_sum<1, 3>());

        static_assert((a + b) + c == a + (b + c));


        constexpr auto f = a + b;
        constexpr auto g = c + d;

        static_assert(f + g == y);
    }

    static_assert(a + a == 2_c*a);

    // operator+ with Addition<...>
    static_assert(b + a + a == b + 2_c*a);
    static_assert((b + a) + a == b + 2_c*a);
    static_assert(a + b + c + a == b + c + 2_c*a);
    static_assert((a + b + c) + a == b + c + 2_c*a);

    // operator+ with Subtraction<...>
    static_assert(b - a + a == b);
    static_assert((b - a) + a == b);
    static_assert(a - b + a == 2_c*a - b);
    static_assert((a - b) + a == 2_c*a - b);
    static_assert(a + a - b == 2_c*a - b);
    static_assert(a + (a - b) == 2_c*a - b);
    static_assert((b + a) + (c - a) == b + c);
    // static_assert(b + a + c - a == b + c);       // TODO     a stuck in the middle
    static_assert((b - a) + (c + a) == b + c);
    static_assert(b - a + c + a == b + c);
    static_assert((a + b + c) + (d - a) == b + c + d);
    static_assert(a + b + c + d - a == b + c + d);
    static_assert((d - a) + (a + b + c) == d + b + c);
    static_assert(d - a + a + b + c == d + b + c);
    static_assert((b - a) + (a - c) == b - c);
    static_assert(b - a + a - c == b - c);
    static_assert((a - c) + (b - a) == b - c);
    static_assert(a - c + b - a == b - c);
    static_assert(a - b + c == (a + c) - b);
    static_assert((a - b) + c == (a + c) - b);
    static_assert(c + a - b == (c + a) - b);
    static_assert(c + (a - b) == (c + a) - b);
    static_assert((a - b) + (c - d) == (a + c) - (b + d));

    // operator+ with Multiplication<...>
    static_assert(b*a + a == (b + 1_c)*a);
    static_assert(a + b*a == (1_c + b)*a);
    static_assert(a*b + a == a*(b + 1_c));
    static_assert(a + a*b == a*(1_c + b));
    static_assert(a*b*c + a == a*(b*c + 1_c));
    static_assert(a + a*b*c == a*(1_c + b*c));
    static_assert(b*c + a*b*c == (1_c + a)*b*c);
    static_assert(a*b*c + b*c == (a + 1_c)*b*c);

    // operator+ with Division<...>
    static_assert(a/b + a == a*(1_c + b)/b);
    static_assert(a + a/b == a*(b + 1_c)/b);
    // static_assert(a/b + c == (a + b*c)/b);
    // static_assert(c + a/b == (c*b + a)/b);
    static_assert(a/b + c/d == (a*d + b*c)/(b*d));
}

}   // test
}   // fields