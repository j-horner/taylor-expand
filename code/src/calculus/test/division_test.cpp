#include "../operators/math_operators.hpp"

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

class DivisionTest : public ::testing::Test {
protected:
    constexpr static auto a = A{};
    constexpr static auto b = B{};
    constexpr static auto c = C{};
    constexpr static auto d = D{};
};

TEST_F(DivisionTest, Division_Is_Correct) {
    using namespace literals;

    static_assert((b*a)/a == b);
    static_assert(b*a/a == b);
    static_assert(a/(b*a) == 1_c/b);
    static_assert((a*b*c)/a == b*c);
    static_assert(a*b*c/a == b*c);
    static_assert(a/(a*b*c) == 1_c/(b*c));
    static_assert((a*b*c)/(b*c) == a);
    static_assert(a*b*c/(b*c) == a);
    static_assert((b*c)/(a*b*c) == 1_c/a);
    static_assert(b*c/(a*b*c) == 1_c/a);
    static_assert((d*a)/(a*b*c) == d/(b*c));
    static_assert(d*a/(a*b*c) == d/(b*c));
    static_assert((a*b*c)/(d*a) == b*c/d);
    static_assert(a*b*c/(d*a) == b*c/d);
    static_assert((b*a)/(c*a) == b/c);
    static_assert(b*a/(c*a) == b/c);
    
	// static_assert((a*b)/(c/d) == a*b*d/c);
    // static_assert((c/d)/(a*b) == c/(d*a*b));
    // static_assert((a/b)/(a/c) == c/b);
    
	static_assert((b/a)/(c/a) == b/c);
    
	// static_assert((a/b)/(c/d) == (a*d)/(b*c));
}

}   // test
}   // fields