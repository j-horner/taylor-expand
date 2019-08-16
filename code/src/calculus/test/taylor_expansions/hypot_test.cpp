#include "../../taylor_series.hpp"

#include "../../comparison.hpp"

#include <gtest/gtest.h>

#include <cmath>

namespace fields {
namespace test {

class HypotTest : public ::testing::Test {
protected:
};

TEST_F(HypotTest, Hypot_Is_Correct) {
	using namespace literals;

	constexpr auto H = [] (auto y) { return t*(y^(-1_c)); };

	constexpr auto y_0 = x;

	constexpr auto y_exact = [](double x_, double t_) { return std::hypot(x_, t_); };

	{
		constexpr auto y0 = taylor_expand<0>(H, y_0);
		constexpr auto y1 = taylor_expand<1>(H, y_0);
		constexpr auto y2 = taylor_expand<2>(H, y_0);
		constexpr auto y3 = taylor_expand<3>(H, y_0);
		constexpr auto y4 = taylor_expand<4>(H, y_0);
		
		constexpr auto z = (x^(-2_c))*(t^2_c);
		
		static_assert(y0 == x);
	
		static_assert(y1 == x);

		static_assert(y2 == x + (1_c/2_c)*z*x);
		static_assert(y3 == x + (1_c/2_c)*z*x);
		static_assert(y4 == x + (1_c/2_c)*z*x + (-1_c / 8_c)*z*z*x);

		// MSVC debug:		N = 16 --> expression did not evaluate to a constant - note: failure was caused by evaluation exceeding step limit of 1048576 (/constexpr:steps<NUMBER>)
		// MSVC release:	N = 17 --> expression did not evaluate to a constant - note: failure was caused by evaluation exceeding step limit of 1048576 (/constexpr:steps<NUMBER>)
		// clang-cl:		N = 15 --> error : constexpr variable 'y' must be initialized by a constant expression - note: constexpr evaluation hit maximum step limit; possible infinite loop?
		constexpr auto y = taylor_expand<14>(H, y_0);

		for (auto x_ : {0.1, 1.0 , 2.0, 5.0, 50.0}) {

			const auto limit = std::abs(x_) * 0.95;

			const auto dt = 2.0*limit/5;

			for (auto t_ = -limit; t_ <= limit; t_ += 0.1) {
				EXPECT_NEAR(y(0, x_, t_), y_exact(x_, t_), std::abs(util::pow(t_/x_, 5))) << "t = " << t_ << "\tx = " << x_ << std::endl;;
			}
		}
	}
}

}   // test
}   // fields