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
    static_assert(b*a*c/a == b*c);       
    static_assert((a*b*c)*(d/a) == b*c*d);
    static_assert(a*b*c*d/a == b*c*d);
    static_assert((d/a)*(a*b*c) == d*b*c);
    static_assert(d/a*a*b*c == d*b*c);
    static_assert((a/b)*(b/c) == a/c);
    static_assert(a/b*b/c == a/c);
    static_assert((a/b)*(c/a) == c/b);
    static_assert(a/b*c/a == c/b);
    // static_assert((a/b)*(c/d) == (a*c)/(b*d));
    // static_assert(a/b*c/d == a*c/(b*d));
    static_assert(a*(b/c) == a*b/c);
    static_assert((a/b)*c == a*c/b);
    static_assert(a/b*c == a*c/b);
}

TEST_F(MultiplicationTest, Contains_Multiple_Is_Correct) {

	using fields::detail::contains_multiple;

    static_assert(util::contains<A, Multiplication<A, B, C>>::index == 0);
    static_assert(util::contains<A, Multiplication<B, A, C>>::index == 1);
    static_assert(util::contains<A, Multiplication<B, C, A>>::index == 2);

	constexpr auto a = A{};
	constexpr auto b = B{};
	constexpr auto c = C{};

	static_assert(util::contains<std::decay_t<decltype(a)>, std::decay_t<decltype(b*a*c)>>::index == 1);

	static_assert(contains_multiple<A, Addition<A, B, C>>::index == 0);
	static_assert(contains_multiple<A, Addition<B, A, C>>::index == 1);
	static_assert(contains_multiple<A, Addition<B, C, A>>::index == 2);
	static_assert(contains_multiple<A, Addition<B, C, D>>::index == -1);

	using fields::detail::is_multiple;

	static_assert(is_multiple<A, Multiplication<D, A>>::value);

	static_assert(contains_multiple<A, Addition<Multiplication<D, A>, B, C>>::index == 0);
	static_assert(contains_multiple<A, Addition<B, Multiplication<D, A>, C>>::index == 1);
	static_assert(contains_multiple<A, Addition<B, C, Multiplication<D, A>>>::index == 2);
	static_assert(contains_multiple<A, Addition<Multiplication<D, B>, B, C>>::index == -1);

	static_assert(contains_multiple<A, Addition<Multiplication<A, B, C, D>, B, C>>::index == 0);
	static_assert(contains_multiple<A, Addition<Multiplication<D, A, B, C>, B, C>>::index == 0);
	static_assert(contains_multiple<A, Addition<B, Multiplication<D, B, A, C>, C>>::index == 1);
	static_assert(contains_multiple<A, Addition<B, C, Multiplication<B, C, D, A>>>::index == 2);
	static_assert(contains_multiple<A, Addition<Multiplication<D, B, C>, B, C>>::index == -1);
}

}   // test
}   // fields