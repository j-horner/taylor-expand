#include "../../taylor_series.hpp"

#include "../../comparison.hpp"

#include <gtest/gtest.h>

#include <cmath>

namespace fields {
namespace test {

class SinCosTest : public ::testing::Test {
protected:
};

TEST_F(SinCosTest, Sin_Cos_Is_Correct) {
	using namespace literals;

	constexpr auto H = [](auto sin, auto cos) { return Vector{cos, -sin}; };

	constexpr auto y_0 = Vector{0_c, 1_c};

	constexpr auto sin_exact = [](double t_) { return std::sin(t_); };
	constexpr auto cos_exact = [](double t_) { return std::cos(t_); };

	{
		constexpr auto y0 = taylor_expand<0>(H, y_0);
		static_assert(y0 == y_0);
		
		// structured bindings cannot be constexpr...
		auto [sin, cos] = taylor_expand<6>(H, y_0);

		// ...therefore we have to use std::is_same directly
		static_assert(std::is_same_v<decltype(sin), decltype(t + (-1_c / 6_c) * (t ^ 3_c) + (1_c / 120_c) * (t ^ 5_c))>);
		static_assert(std::is_same_v<decltype(cos), decltype(1_c + (-1_c / 2_c) * (t ^ 2_c) + (1_c / 24_c) * (t ^ 4_c) + (-1_c/720_c)*(t^6_c))>);
	}

	{
		// N = 21 --> integer overflow!
		auto [sin, cos] = taylor_expand<20>(H, y_0);
				
		for (auto t_ : { -0.9, -0.75, -0.5, -0.2, -0.1, 0.0, 0.1, 0.2, 0.5, 0.75, 0.9 }) {
			EXPECT_NEAR(sin(0, 0, t_), sin_exact(t_), std::abs(util::pow(t_, 15)));
			EXPECT_NEAR(cos(0, 0, t_), cos_exact(t_), std::abs(util::pow(t_, 15)));
		}
	}
}

}	// test
}	// fields