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

struct dA_dx {};
struct dB_dx {};
struct dC_dx {};
struct dD_dx {};

constexpr auto d_dx(A) -> dA_dx { return {}; }
constexpr auto d_dx(B) -> dB_dx { return {}; }
constexpr auto d_dx(C) -> dC_dx { return {}; }
constexpr auto d_dx(D) -> dD_dx { return {}; }

class MultiplicationTest : public ::testing::Test {
 protected:
    constexpr static auto a = A{};
    constexpr static auto b = B{};
    constexpr static auto c = C{};
    constexpr static auto d = D{};
};

TEST_F(MultiplicationTest, Mutliplication_Is_Correct) {
    using namespace operators;
    using namespace literals;

    {
        constexpr auto y = a*b*c*d;

        static_assert(y(2) == 384, "3*2x*4x*x*x(2) != 384");

        static_assert(std::is_same_v<std::decay_t<decltype(a)>, std::decay_t<decltype(y.get<0>())>>);
        static_assert(std::is_same_v<std::decay_t<decltype(d)>, std::decay_t<decltype(y.get<3>())>>);

        static_assert(b*c == y.sub_product<1, 3>());

        static_assert((a*b)*c == a*(b*c), "* is not associative");

        constexpr auto f = a*b;
        constexpr auto g = c*d;

        static_assert(f*g == y, "(a*b)*(c*d) != a*b*c*d");
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
    static_assert((a/b)*(c/d) == (a*c)/(b*d));
    // static_assert(a/b*c/d == a*c/(b*d));
    static_assert(a*(b/c) == a*b/c);
    static_assert((a/b)*c == a*c/b);
    static_assert(a/b*c == a*c/b);
}

}   // test
}   // fields