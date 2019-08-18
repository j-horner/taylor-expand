#include "../derivative.hpp"

#include "../comparison.hpp"
#include "../functions/linear.hpp"

#include "../addition.hpp"
#include "../subtraction.hpp"
#include "../multiplication.hpp"
#include "../division.hpp"

#include <gtest/gtest.h>

namespace fields {

// TODO: clang fails the first test if placed in fields::test namespace. WHY?
// namespace test {

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

template <int N = 1> struct dA_dx {};
template <int N = 1> struct dB_dx {};
template <int N = 1> struct dC_dx {};
template <int N = 1> struct dD_dx {};

template <int N = 1> constexpr auto d_dx(A) { return dA_dx<N>{}; }
template <int N = 1> constexpr auto d_dx(B) { return dB_dx<N>{}; }
template <int N = 1> constexpr auto d_dx(C) { return dC_dx<N>{}; }
template <int N = 1> constexpr auto d_dx(D) { return dD_dx<N>{}; }

class DerivativeTest : public ::testing::Test {
protected:
    	
	constexpr static auto a = A{};
    constexpr static auto b = B{};
    constexpr static auto c = C{};
    constexpr static auto d = D{};
};

TEST_F(DerivativeTest, Derivative_Of_Sums_Is_Correct) {
	constexpr auto da_dx = d_dx(a);
    constexpr auto db_dx = d_dx(b);
    constexpr auto dc_dx = d_dx(c);
    constexpr auto dd_dx = d_dx(d);

	static_assert(d_dx(a + b) == da_dx + db_dx);
    static_assert(d_dx(a + b + c) == da_dx + db_dx + dc_dx);
    static_assert(d_dx(a + b + c + d) == da_dx + db_dx + dc_dx + dd_dx);

    static_assert(d_dx(a - b) == da_dx - db_dx);
}

TEST_F(DerivativeTest, Derivative_Of_Products_Is_Correct) {
	using namespace literals;

	constexpr auto da_dx = d_dx(a);
	constexpr auto db_dx = d_dx(b);
	constexpr auto dc_dx = d_dx(c);
	constexpr auto dd_dx = d_dx(d);

    static_assert(d_dx(a*b) == da_dx*b + a*db_dx);
    static_assert(d_dx(a*b*c) == da_dx*b*c + a*db_dx*c + a*b*dc_dx);
    static_assert(d_dx(a*b*c*d) == da_dx*b*c*d + a*db_dx*c*d + a*b*dc_dx*d + a*b*c*dd_dx);

    static_assert(d_dx(a/b) == da_dx*(b^-1_c) + (-a)*(b^-2_c)*db_dx);
}

TEST_F(DerivativeTest, Derivative_Of_Linear_Is_Correct) {
	using namespace literals;

    {
        static_assert(d_dt(t) == 1);
        static_assert(d_dt<1>(t) == 1);
        static_assert(d_dt<2>(t) == 0);
        static_assert(d_dt<30>(t) == 0);

        static_assert(d_dx(x) == 1);
        static_assert(d_dx<1>(x) == 1);
        static_assert(d_dx<2>(x) == 0);
        static_assert(d_dx<30>(x) == 0);

        static_assert(d_dx(t) == 0);
        static_assert(d_dt(x) == 0);
        static_assert(d_dx<3>(t) == 0);
        static_assert(d_dt<7>(x) == 0);

        static_assert(d_dt<2>(x + t) == 0);
        static_assert(d_dx<4>(x - t) == 0);
    }

    {
        constexpr auto y = x;
        static_assert(d_dx(y) == 1);
    }
    {
        constexpr auto y = x + x + x + x;

        static_assert(y == 4_c*x);
        static_assert(d_dx(y) == 4);
    }
    {
        constexpr auto y = x - x;

        static_assert(y == 0);
        static_assert(d_dx(y) == 0);
    }
    {
        constexpr auto y = x + x - x + x + x + x - x - x - x + x - x - x;

        static_assert(y == 0);
        static_assert(d_dx(y) == 0);
    }
    {
        constexpr auto y = x*x;
        static_assert(d_dx(y) == 2_c*x);
    }
    {
        constexpr auto y = x/x;
        static_assert(d_dx(y) == 0);
    }
    {
        constexpr auto y = x*x*x;
        static_assert(d_dx(y) == 3_c*x*x);
    }
    {
        constexpr auto y = x*x*x*x*x*x;
        static_assert(d_dx(y) == 6_c*x*x*x*x*x);
    }
}

TEST_F(DerivativeTest, Time_Derivative_Of_Fields_Is_Correct) {
    using namespace fields::detail;
    using namespace literals;

    {
        constexpr auto H = [] (auto y) { return y; };

		constexpr auto y = Field{H};

        static_assert(d_dt(y) == H(y));

        static_assert(d_dt<0>(y) == y);
        static_assert(d_dt<1>(y) == d_dt(y));

        // only true because of the form of H
        static_assert(d_dt<1>(y) == y);
        static_assert(d_dt<10>(y) == y);
    }
    {
        constexpr auto H = [] (auto y) { return y + 5_c; };

		constexpr auto y = Field{H};

        static_assert(d_dt(y) == H(y));

        static_assert(d_dt<0>(y) == y);
        static_assert(d_dt<1>(y) == d_dt(y));

        // only true because of the form of H
        static_assert(d_dt<1>(y) == y + 5_c);
        static_assert(d_dt<10>(y) == y + 5_c);
    }
    {
        constexpr auto H = [] (auto y) { return 2_c*y; };

		constexpr auto y = Field{ H };

        static_assert(d_dt(y) == H(y));

        static_assert(d_dt<0>(y) == y);
        static_assert(d_dt<1>(y) == d_dt(y));

        // only true because of the form of H
        static_assert(d_dt<1>(y) == 2_c*y);
        static_assert(d_dt<2>(y) == 4_c*y);
        static_assert(d_dt<5>(y) == 32_c*y);
        static_assert(d_dt<10>(y) == 1024_c*y);
    }
    {
        constexpr auto H = [] (auto y) { return y*y; };

		constexpr auto y = Field{ H };

        static_assert(d_dt(y) == H(y));

        static_assert(d_dt<0>(y) == y);
        static_assert(d_dt<1>(y) == d_dt(y));

        // only true because of the form of H
        static_assert(d_dt<1>(y) == factorial(1_c)*(y^2_c));
        static_assert(d_dt<2>(y) == factorial(2_c)*(y^3_c));
        static_assert(d_dt<5>(y) == factorial(5_c)*(y^6_c));
        static_assert(d_dt<10>(y) == factorial(10_c)*(y^11_c));
        static_assert(d_dt<20>(y) == factorial(20_c)*(y^21_c));
    }
    {
        constexpr auto H = [] (auto) { return x + t; };

		constexpr auto y = Field{ H };

        static_assert(d_dt(y) == H(y));

        static_assert(d_dt<0>(y) == y);
        static_assert(d_dt<1>(y) == d_dt(y));

        // only true because of the form of H
        static_assert(d_dt(y) == x + t);
        static_assert(d_dt<2>(y) == 1);
        static_assert(d_dt<3>(y) == 0);
        static_assert(d_dt<5>(y) == 0);
        static_assert(d_dt<10>(y) == 0);
        static_assert(d_dt<20>(y) == 0);
    }
    {
        constexpr auto g = x + t*t;
        
        constexpr auto H = [g] (auto y) { return g*y; };

		constexpr auto y = Field{ H };

        static_assert(d_dt(y) == H(y));

        static_assert(d_dt<0>(y) == y);
        static_assert(d_dt<1>(y) == d_dt(y));

        static_assert(d_dt(y) == g*y);

        static_assert(d_dt<2>(y) == 2_c*t*y + (g^2_c)*y);
        static_assert(d_dt<3>(y) == 2_c*y + 6_c*t*g*y + (g^3_c)*y);
        static_assert(d_dt<4>(y) == (g^4_c)*y + 8_c*g*y + 12_c*(g^2_c)*t*y + 12_c*t*t*y);
    }
    {
		using fields::d_dx;
		constexpr auto H = [] (auto y) { return d_dx<2>(y); };

		constexpr auto y = Field{ H };

        static_assert(d_dt(y) == H(y));

        static_assert(d_dt<0>(y) == y);
        static_assert(d_dt<1>(y) == d_dt(y));

        static_assert(d_dt<1>(y) == d_dx<2>(y));
        static_assert(d_dt<2>(y) == d_dx<4>(y));
        static_assert(d_dt<5>(y) == d_dx<10>(y));
        static_assert(d_dt<10>(y) == d_dx<20>(y));
        static_assert(d_dt<20>(y) == d_dx<40>(y));
        static_assert(d_dt<100>(y) == d_dx<200>(y));
    }

}

// }   // test
}   // fields