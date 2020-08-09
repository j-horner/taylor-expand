#include "../operators/math_operators.hpp"

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

class SubtractionTest : public ::testing::Test {
protected:
    constexpr static auto a = A{};
    constexpr static auto b = B{};
    constexpr static auto c = C{};
    constexpr static auto d = D{};
};

TEST_F(SubtractionTest, Subtraction_Is_Correct) {
    using namespace literals;

    static_assert(a - a == 0);

    // operator- with Addition<...>
    static_assert(b + a - a == b);
    static_assert((b + a) - a == b);
    static_assert(b + a - (a + c + d) == b - c - d);
    static_assert((b + a) - (a + c + d) == b - c - d);
    static_assert(a + c + d - (b + a) == c + d - b);
    static_assert((a + c + d) - (b + a) == c + d - b);
    static_assert(b + a - (c + a) == b - c);
    static_assert((b + a) - (c + a) == b - c);
    static_assert(a - (b + a) == -b);
    static_assert(a + b + c - a == b + c);
    static_assert((a + b + c) - a == b + c);
    static_assert(a - (a + b + c) == -(b + c));

    // operator- with Subtraction<...>
    static_assert(b - a - a == b - 2_c*a);
    static_assert((b - a) - a == b - 2_c*a);
    static_assert(a - (b - a) == 2_c*a - b);
    static_assert(a - b - a == -b);
    static_assert((a - b) - a == -b);
    static_assert(a - (a - b) == b);
    static_assert(a - b - c == a - (b + c));
    static_assert((a - b) - c == a - (b + c));
    static_assert(a - (b - c) == (a + c) - b);
    static_assert((a - b) - (c - d) == (a + d) - (b + c));

    // operator- with Multiplication<...>
    static_assert(b*a - a == (b - 1_c)*a);
    static_assert(a - b*a == (1_c - b)*a);
    static_assert(a*b - a == a*(b - 1_c));
    static_assert(a - a*b == a*(1_c - b));
    static_assert(a*b*c - a == a*(b*c - 1_c));
    static_assert(a - a*b*c == a*(1_c - b*c));
    static_assert(b*c - a*b*c == (1_c - a)*b*c);
    static_assert(a*b*c - b*c == (a - 1_c)*b*c);

    // operator- with Division<...>
    /*static_assert(a/b - a == a*(1_c - b)/b);
    static_assert(a - a/b == a*(b - 1_c)/b);
    static_assert(a/b - c == (a - b*c)/b);
    static_assert(c - a/b == (c*b - a)/b);
    static_assert(a/b - c/d == (a*d - b*c)/(b*d));*/
}

}   // test
}   // fields