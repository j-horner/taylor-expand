#include "../multiplication.hpp"

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

class MultiplicationTest : public ::testing::Test {
 protected:
};

TEST_F(MultiplicationTest, Mutliplication_Is_Correct) {
    using namespace literals;

    constexpr auto a = A{};
    constexpr auto b = B{};
    constexpr auto c = C{};
    constexpr auto d = D{};

    {
        constexpr auto y = a*b*c*d;

        static_assert(y(2) == 384);

        static_assert(a == y.get<0>());
        static_assert(d == y.get<3>());

        static_assert(b*c == y.sub_product<1, 3>());

        static_assert((a*b)*c == a*(b*c));

        constexpr auto f = a*b;
        constexpr auto g = c*d;

        static_assert(f*g == y);
    }

    static_assert((3_c/4_c)*a*5_c/6_c == (5_c/8_c)*a);
    static_assert(a*b*2_c*c*3_c*d*4_c == 24_c*a*b*c*d);

    static_assert((b*a)*(c/a) == b*c);
    // static_assert(b*a*c/a == b*c);       // currently fails as it deduces (b*a*c)/a and a is stuck in the middle
    static_assert((a*b*c)*(d/a) == b*c*d);
    static_assert(a*b*c*d/a == b*c*d);
    static_assert((d/a)*(a*b*c) == d*b*c);
    static_assert(d/a*a*b*c == d*b*c);
    static_assert((a/b)*(b/c) == a/c);
    static_assert(a/b*b/c == a/c);
    static_assert((a/b)*(c/a) == c/b);
    // static_assert(a/b*c/a == c/b);              // TODO     not sure why this doesn't work but +- equivalent does (line 88 in addition_test.cpp)
    // static_assert((a/b)*(c/d) == (a*c)/(b*d));
    // static_assert(a/b*c/d == a*c/(b*d));
    static_assert(a*(b/c) == a*b/c);
    static_assert((a/b)*c == a*c/b);
    static_assert(a/b*c == a*c/b);
}

}   // test
}   // fields