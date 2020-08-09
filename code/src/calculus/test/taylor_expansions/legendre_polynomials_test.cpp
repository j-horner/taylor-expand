#include "../../taylor_series.hpp"

#include "../../comparison.hpp"

#include <gtest/gtest.h>

#include <cmath>

namespace fields {
namespace test {

using namespace literals;

class LegendrePolynomialsTest : public ::testing::Test {
protected:

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


	/*template <Int N>
	constexpr static auto Legendre(Constant<N> n) { return (1_c/((2_c^n)*factorial(n)))*d_dt<N>((t*t - 1_c)^n); }*/

	template <Int N>
	constexpr static auto initial_P(Constant<N> n) {
		if constexpr (N < 0) {
			return 0_c;
		} else {
			if constexpr (N % 2 == 0) {
				const auto m = n / 2_c;
				return ((-1_c) ^ m) * (factorial(n) / (factorial(m) ^ 2_c)) / (2_c ^ n);
			}
			else {
				return 0_c;
			}
		}
	}


	template <Int N>
	constexpr static auto Legendre(Constant<N> n) {
		
		constexpr auto H = [] (auto Pn, auto dPn_dt) {
			constexpr auto n = Constant<N>{};
			return Vector{ dPn_dt,
						   (2_c*t*dPn_dt - n*(n + 1_c)*Pn)/(1_c - t*t)};
		};

		{
			auto y = make_field_vector<2>(H);

			// std::cout << y << std::endl;
			std::cout << d_dt(y) << std::endl;
			std::cout << d_dt<2>(y) << std::endl;
			// std::cout << d_dt<3>(y) << std::endl;
		}

		auto Pn_0 = initial_P(n);

		auto dPn_dt_0 = n * initial_P(n - 1_c);

		return taylor_expand<N>(H, Vector{ Pn_0, dPn_dt_0 }).template get<0>();
	}


};

TEST_F(LegendrePolynomialsTest, Convoluted_Algebra_Required_For_Legendre_Polynomial_Simplifications) {
	using namespace literals;

	// simplification bugs...
	// (2(2y[1]t - 6y[0])((1 - (t)^ { 2 }))^ { -1 }t + (2y[1] - 6y[1]))((1 - (t)^ { 2 }))^ { -1 } +2t((1 - (t)^ { 2 }))^ { -2 }(2y[1]t - 6y[0])

	constexpr auto a = A{};
	constexpr auto b = B{};

	constexpr auto c = 2_c*b - 6_c*a;
	constexpr auto d = (1_c - t*t)^(-1_c);

	// constexpr auto f = (2_c*c*d*t + (-4_c*b))*d;


	using fields::detail::is_multiple;
	using fields::detail::is_power;
	using fields::detail::contains_sub_multiple;

	static_assert(is_multiple<std::decay_t<decltype(2_c*c*d*t*d)>, std::decay_t<decltype(2_c*t*(d*d)*c)>>::value);

	static_assert(2_c*c*d*t*d + 2_c*t*(d*d)*c == 4_c*c*d*d*t);

	static_assert(0 == contains_sub_multiple<A, Addition<A, B>>::index);
	static_assert(1 == contains_sub_multiple<B, Addition<A, B>>::index);
	static_assert(1 == contains_sub_multiple<B, Addition<Multiplication<A, T, B>, B>>::index);

	static_assert(0 == contains_sub_multiple<Multiplication<T, A>, Addition<Multiplication<A, T, B>, B>>::index);
	static_assert(1 == contains_sub_multiple<Multiplication<T, B>, Addition<Multiplication<A, B>, Multiplication<B, T>>>::index);

	static_assert(false == is_power<A, B>::value, "Arbitrary different types are not powers of each other.");
	static_assert(false == is_power<A, A>::value, "Arbitrary type is not a power of itself unless it is of the form Power<F, N>");

	static_assert(is_power<std::decay_t<decltype(d* d)>, std::decay_t<decltype(d)>>::value);
	static_assert(is_power<std::decay_t<decltype(d)>, std::decay_t<decltype(d* d)>>::value);
	static_assert(is_power<std::decay_t<decltype(d)>, std::decay_t<decltype(d)>>::value);

	// using F = std::decay_t<decltype(f)>;
	// using G = std::decay_t<decltype(2_c*t*(d*d)*c)>;

	// static_assert(contains_sub_multiple<G, F>::index == 0);

	// static_assert(contains_sub_multiple<Multiplication<Constant<2>, T, Power<D, 2>, C>, Multiplication<Addition<Multiplication<Constant<2>, C, D, T>, Multiplication<Constant<-4>, B>>, D>>::index == 0);


	// static_assert(contains_sub_multiple<Multiplication<Constant<2>, T, Power<D, 2>, C>, Addition<fields::detail::multiply<Multiplication<Constant<2>, C, D, T>, D>::type, fields::detail::multiply<Multiplication<Constant<-4>, B>, D>>>::index == 0);

	// static_assert(0 == contains_sub_multiple<std::decay_t<decltype(2_c*t*(d*d)*c)>, std::decay_t<decltype((2_c*c*d*t + (-4_c*b))* d)>>::index);

	// constexpr auto e = f + 2_c*t*(d*d)*c;

	// static_assert(e == 4_c*c*t*d*d + ((-4_c)*d*b), "This simplification is required for Legendre Polynomials");

	// static_assert((2_c*c*d*t + (-4_c*b))*d + 2_c*t*(d*d)*c == 4_c*c*d*d*t + (-4_c*b*d));
	static_assert((2_c*c*d*t*d + (-4_c*b*d)) + 2_c*t*(d*d)*c == 4_c*c*d*d*t + (-4_c*b*d));
	static_assert((2_c*c*d*d*t + (-4_c*b*d)) + 2_c*t*(d*d)*c == 4_c*c*d*d*t + (-4_c*b*d));

}

TEST_F(LegendrePolynomialsTest, Legendre_Polynomials_Are_Correct) {

	
	// std::cout << Legendre(2_c) << std::endl;
	// std::cout << Legendre(3_c) << std::endl;
	// std::cout << Legendre(4_c) << std::endl;
	// std::cout << Legendre(5_c) << std::endl;
	// std::cout << Legendre(6_c) << std::endl;

	auto y = Legendre(2_c);
	
	/*for (auto t_ : { -0.9, -0.75, -0.5, -0.2, -0.1, 0.0, 0.1, 0.2, 0.5, 0.75, 0.9 }) {
		
		// EXPECT_NEAR(Legendre(0_c)(0, 0, t_), std::legendre(0u, t_), std::abs(util::pow(t_, 10)));
		// EXPECT_NEAR(Legendre(1_c)(0, 0, t_), std::legendre(1u, t_), std::abs(util::pow(t_, 10)));
		// EXPECT_NEAR(Legendre(2_c)(0, 0, t_), std::legendre(2u, t_), std::abs(util::pow(t_, 10)));
		// EXPECT_NEAR(Legendre(3_c)(0, 0, t_), std::legendre(3u, t_), std::abs(util::pow(t_, 10)));
		// EXPECT_NEAR(Legendre(4_c)(0, 0, t_), std::legendre(4u, t_), std::abs(util::pow(t_, 10)));
		// EXPECT_NEAR(Legendre(5_c)(0, 0, t_), std::legendre(5u, t_), std::abs(util::pow(t_, 10)));
		// EXPECT_NEAR(Legendre(6_c)(0, 0, t_), std::legendre(6u, t_), std::abs(util::pow(t_, 10)));
		// EXPECT_NEAR(Legendre(7_c)(0, 0, t_), std::legendre(7u, t_), std::abs(util::pow(t_, 10)));
		// EXPECT_NEAR(Legendre(8_c)(0, 0, t_), std::legendre(8u, t_), std::abs(util::pow(t_, 10)));
		// EXPECT_NEAR(Legendre(9_c)(0, 0, t_), std::legendre(9u, t_), std::abs(util::pow(t_, 10)));
		// EXPECT_NEAR(Legendre(10_c)(0, 0, t_), std::legendre(10u, t_), std::abs(util::pow(t_, 10)));
	}
	*/

}

}	// test
}	// fields