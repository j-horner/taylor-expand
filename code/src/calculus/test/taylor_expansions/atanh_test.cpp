#include "../../taylor_series.hpp"

#include "../../comparison.hpp"

#include <gtest/gtest.h>

#include <cmath>

namespace fields {
namespace test {

class ArcTanhTest : public ::testing::Test {
protected:
};

TEST_F(ArcTanhTest, Arc_Tanh_Is_Correct) {
	using namespace literals;

	constexpr auto H = [] (auto) { return 1_c/(1_c - t*t); };

	constexpr auto y_0 = 0_c;

	constexpr auto y_exact = [](double t_) { return std::atanh(t_); };
	{
		constexpr auto y0 = taylor_expand<0>(H, y_0);
		constexpr auto y1 = taylor_expand<1>(H, y_0);
		constexpr auto y2 = taylor_expand<2>(H, y_0);
		constexpr auto y3 = taylor_expand<3>(H, y_0);
		constexpr auto y5 = taylor_expand<5>(H, y_0);


		static_assert(y0 == 0);
		static_assert(y1 == t);
		static_assert(y2 == t);
		static_assert(y3 == t + (1_c / 3_c) * (t ^ 3_c));
		static_assert(y5 == t + (1_c / 3_c) * (t ^ 3_c) + (1_c / 5_c) * (t ^ 5_c));
	}

	{
		// N = 17 --> MSVC Debug: error C2131: expression did not evaluate to a constant : note: failure was caused by evaluation exceeding step limit of 1048576 (/constexpr:steps<NUMBER>)
		// N = 18 --> MSVC Release: integer overflow!
		// N = 16 --> clang - error : constexpr variable 'y' must be initialized by a constant expression : note: constexpr evaluation hit maximum step limit; possible infinite loop?
		constexpr auto y = taylor_expand<15>(H, y_0);

		for (auto t_ : { -0.9, -0.75, -0.5, -0.2, -0.1, 0.0, 0.1, 0.2, 0.5, 0.75, 0.9 }) {
			EXPECT_NEAR(y(0, 0, t_), y_exact(t_), std::abs(util::pow(t_, 10)));
		}
	}
}

}	// test
}	// fields