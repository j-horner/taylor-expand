#include "../../taylor_series.hpp"

#include "../../comparison.hpp"

#include <gtest/gtest.h>

#include <cmath>

namespace fields {
namespace test {

class ArcSinhTest : public ::testing::Test {
protected:
};

TEST_F(ArcSinhTest, Arc_Sinh_Is_Correct) {
	using namespace literals;

	constexpr auto H = [](auto y, auto dy_dt) { return Vector{ dy_dt, -t*(dy_dt^3_c) }; };

	constexpr auto y_0 = Vector{ 0_c, 1_c };

	constexpr auto y_exact = [](double t_) { return std::asinh(t_); };
	{
		constexpr auto y0 = taylor_expand<0>(H, y_0);
		constexpr auto y1 = taylor_expand<1>(H, y_0).get<0>();
		constexpr auto y2 = taylor_expand<2>(H, y_0).get<0>();
		constexpr auto y3 = taylor_expand<3>(H, y_0).get<0>();
		constexpr auto y5 = taylor_expand<5>(H, y_0).get<0>();


		static_assert(y0 == y_0);
		static_assert(y1 == t);
		static_assert(y2 == t);
		static_assert(y3 == t + (-1_c/6_c)*(t^3_c));
		static_assert(y5 == t + (-1_c/6_c)*(t^3_c) + (3_c/40_c)*(t^5_c));
	}

	{
		// N = 10 --> MSVC Debug: fatal error C1060: compiler is out of heap space
		// N = 10 --> MSVC Release: fatal error C1060: compiler is out of heap space
		// N = 9 --> clang: LLVM error : out of memory
		constexpr auto y = taylor_expand<8>(H, y_0).get<0>();

		for (auto t_ : { -0.9, -0.75, -0.5, -0.2, -0.1, 0.0, 0.1, 0.2, 0.5, 0.75, 0.9 }) {
			EXPECT_NEAR(y(0, 0, t_), y_exact(t_), std::abs(util::pow(t_, 10)));
		}
	}
}

}	// test
}	// fields