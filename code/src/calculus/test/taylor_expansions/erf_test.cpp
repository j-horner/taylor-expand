#include "../../taylor_series.hpp"

#include "../../comparison.hpp"

#include <gtest/gtest.h>

#include <cmath>

namespace fields {
namespace test {

class ErfTest : public ::testing::Test {
protected:
};

TEST_F(ErfTest, Erf_And_Erfc_Is_Correct) {
	using namespace literals;

	constexpr auto H = [](auto y, auto dy_dt) { return Vector{dy_dt, -2_c*t*dy_dt}; };
		
	constexpr auto two_over_sqrt_pi = 1.1283791670955126_c;

	{
		constexpr auto y_exact = [](double t_) { return std::erf(t_); };
		
		constexpr auto y_0 = Vector{ 0_c, two_over_sqrt_pi };

	
		// N = 10 --> MSVC Debug: error C2131: expression did not evaluate to a constant : note: failure was caused by evaluation exceeding step limit of 1048576 (/constexpr:steps<NUMBER>)
		// N = 10 --> MSVC Release: fatal error C1060: compiler is out of heap space
		// N = 10 --> clang -  LLVM error : out of memory
		constexpr auto y = taylor_expand<9>(H, y_0).get<0>();

		for (auto t_ : { -0.9, -0.75, -0.5, -0.2, -0.1, 0.0, 0.1, 0.2, 0.5, 0.75, 0.9 }) {
			EXPECT_NEAR(y(0, 0, t_), y_exact(t_), std::abs(util::pow(t_, 10)));
		}
	}

	{
		constexpr auto y_exact = [](double t_) { return std::erfc(t_); };

		constexpr auto y_0 = Vector{ 1_c, -two_over_sqrt_pi };


		// N = 10 --> MSVC Debug: error C2131: expression did not evaluate to a constant : note: failure was caused by evaluation exceeding step limit of 1048576 (/constexpr:steps<NUMBER>)
		// N = 10 --> MSVC Release: fatal error C1060: compiler is out of heap space
		// N = 10 --> clang -  LLVM error : out of memory
		constexpr auto y = taylor_expand<9>(H, y_0).get<0>();

		for (auto t_ : { -0.9, -0.75, -0.5, -0.2, -0.1, 0.0, 0.1, 0.2, 0.5, 0.75, 0.9 }) {
			EXPECT_NEAR(y(0, 0, t_), y_exact(t_), std::abs(util::pow(t_, 10)));
		}
	}

}

}	// test
}	// fields