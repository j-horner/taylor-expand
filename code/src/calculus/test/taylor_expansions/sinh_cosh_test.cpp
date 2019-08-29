#include "../../taylor_series.hpp"

#include "../../comparison.hpp"

#include <gtest/gtest.h>

#include <cmath>

namespace fields {
namespace test {

class SinhCoshTest : public ::testing::Test {
protected:
};

TEST_F(SinhCoshTest, Sinh_Cosh_Is_Correct) {
	using namespace literals;

	constexpr auto H = [](auto sinh, auto cosh) { return Vector{ cosh, sinh }; };

	constexpr auto y_0 = Vector{ 0_c, 1_c };

	constexpr auto sinh_exact = [](double t_) { return std::sinh(t_); };
	constexpr auto cosh_exact = [](double t_) { return std::cosh(t_); };

	{
		constexpr auto y0 = taylor_expand<0>(H, y_0);
		static_assert(y0 == y_0);

		// structured bindings cannot be constexpr...
		constexpr auto y = taylor_expand<6>(H, y_0);

		constexpr auto sinh = y.get<0>();
		constexpr auto cosh = y.get<1>();

		static_assert(sinh == t + (1_c / 6_c) * (t ^ 3_c) + (1_c / 120_c) * (t ^ 5_c));
		static_assert(cosh == 1_c + (1_c / 2_c) * (t ^ 2_c) + (1_c / 24_c) * (t ^ 4_c) + (1_c / 720_c) * (t ^ 6_c));
	}

	{
		// N = 21 --> integer overflow!
		auto [sinh, cosh] = taylor_expand<20>(H, y_0);

		for (auto t_ : { -0.9, -0.75, -0.5, -0.2, -0.1, 0.0, 0.1, 0.2, 0.5, 0.75, 0.9 }) {
			EXPECT_NEAR(sinh(0, 0, t_), sinh_exact(t_), std::abs(util::pow(t_, 15)));
			EXPECT_NEAR(cosh(0, 0, t_), cosh_exact(t_), std::abs(util::pow(t_, 15)));
		}
	}
}

}	// test
}	// fields