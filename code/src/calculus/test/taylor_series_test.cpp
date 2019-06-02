#include "../taylor_series.hpp"

#include "../multiplication.hpp"
#include "../comparison.hpp"
#include "../functions/linear.hpp"
#include "../functions/power.hpp"


#include <gtest/gtest.h>

#include <iostream>
#include <type_traits>
#include <vector>

#include <cmath>

namespace fields {
namespace test {

// Class to test the integration of various equations
class TaylorSeriesTest : public ::testing::Test {
 protected:
};


TEST_F(TaylorSeriesTest, Series_Expansion_Is_Correct) {
    using namespace operators;
    using namespace literals;

    {
        constexpr auto H = [] (auto) { return 0_c; };

        constexpr auto y_0 = [] {};
        {
                constexpr auto y0 = taylor_expand<0>(H, y_0);
                constexpr auto y1 = taylor_expand<1>(H, y_0);
                constexpr auto y2 = taylor_expand<2>(H, y_0);
                constexpr auto y3 = taylor_expand<3>(H, y_0);
                static_assert(y0 == y_0);
                static_assert(y1 == y0);
                static_assert(y2 == y0);
                static_assert(y3 == y0);
        }
    }
    {
        constexpr auto H = [] (auto) { return 3_c; };

        constexpr auto y_0 = [] {};
        {
                constexpr auto y0 = taylor_expand<0>(H, y_0);
                constexpr auto y1 = taylor_expand<1>(H, y_0);
                constexpr auto y2 = taylor_expand<2>(H, y_0);
                constexpr auto y3 = taylor_expand<3>(H, y_0);
                static_assert(y0 == y_0);
                static_assert(y1 == y_0 + 3_c*t);
                static_assert(y2 == y1);
                static_assert(y3 == y1);
        }

    }
    {
        constexpr auto H = [] (auto) { return 7_c*x - 4_c; };

        constexpr auto y_0 = [] {};
        {
                constexpr auto y0 = taylor_expand<0>(H, y_0);
                constexpr auto y1 = taylor_expand<1>(H, y_0);
                constexpr auto y2 = taylor_expand<2>(H, y_0);
                constexpr auto y3 = taylor_expand<3>(H, y_0);
                static_assert(y0 == y_0);
                static_assert(y1 == y_0 + (7_c*x - 4_c)*t);
                static_assert(y2 == y1);
                static_assert(y3 == y1);
        }
    }
    {
        constexpr auto H = [] (auto) { return t - ((x*t)^2_c)/2_c; };

        constexpr auto y_0 = [] {};
        {
                constexpr auto y0 = taylor_expand<0>(H, y_0);
                constexpr auto y1 = taylor_expand<1>(H, y_0);
                constexpr auto y2 = taylor_expand<2>(H, y_0);
                constexpr auto y3 = taylor_expand<3>(H, y_0);

                static_assert(y0 == y_0);
                static_assert(y1 == y0);
                static_assert(y2 == y1 + (t*t)/2_c);
                static_assert(y3 == y0 + (t*t)/2_c + (-1_c*x*x*(t^3_c))/6_c);
        }
    }
}

TEST_F(TaylorSeriesTest, Lerp_Is_Correct) {
    using namespace operators;
    using namespace literals;

    constexpr auto a = 4.3_c;
    constexpr auto b = 7.9_c;

    constexpr auto H = [a, b] (auto) { return b - a; };

    constexpr auto y_0 = a;

    constexpr auto y_exact = [a, b] (double t_) { return a + (b - a)*t_; };     // TODO : use std::lerp(a, b, t_); in C++20

    {
        constexpr auto y0 = taylor_expand<0>(H, y_0);
        constexpr auto y1 = taylor_expand<1>(H, y_0);
        constexpr auto y2 = taylor_expand<2>(H, y_0);
        constexpr auto y3 = taylor_expand<3>(H, y_0);
        constexpr auto y = taylor_expand<20>(H, y_0);

        static_assert(y0 == a);
        static_assert(y1 == a + (b - a)*t);
        static_assert(y2 == a + (b - a)*t);
        static_assert(y3 == a + (b - a)*t);
        static_assert(y == a + (b - a)*t);

        for (auto t_ : {-0.9, -0.75, -0.5, -0.2, -0.1, 0.0, 0.1, 0.2, 0.5, 0.75, 0.9}) {
            EXPECT_NEAR(y(0, 0, t_), y_exact(t_), std::abs(util::pow(t_, 15)));
        }
    }

}

TEST_F(TaylorSeriesTest, Exponential_Is_Correct) {
    using namespace operators;
    using namespace literals;

    constexpr auto H = [] (auto y) { return y; };

    constexpr auto y_0 = 1_c;

    constexpr auto y_exact = [] (double t_) { return std::exp(t_); };

    {
        constexpr auto y0 = taylor_expand<0>(H, y_0);
        constexpr auto y1 = taylor_expand<1>(H, y_0);
        constexpr auto y2 = taylor_expand<2>(H, y_0);
        constexpr auto y3 = taylor_expand<3>(H, y_0);
        static_assert(y0 == 1);
        static_assert(y1 == 1_c + t);
        static_assert(y2 == 1_c + t + (t^2_c)/2_c);
        static_assert(y3 == 1_c + t + (t^2_c)/2_c + (t^3_c)/6_c);

        constexpr auto y = taylor_expand<20>(H, y_0);

        for (auto t_ : {-0.9, -0.75, -0.5, -0.2, -0.1, 0.0, 0.1, 0.2, 0.5, 0.75, 0.9}) {
            EXPECT_NEAR(y(0, 0, t_), y_exact(t_), std::abs(util::pow(t_, 20)));
        }
    }
}

TEST_F(TaylorSeriesTest, Geometric_Series_Is_Correct) {
    using namespace operators;
    using namespace literals;

    constexpr auto H = [] (auto y) { return y*y; };

    constexpr auto y_0 = 1_c;

    constexpr auto y_exact = [] (double t_) { return 1.0/(1.0 - t_); };

    {
        constexpr auto y0 = taylor_expand<0>(H, y_0);
        constexpr auto y1 = taylor_expand<1>(H, y_0);
        constexpr auto y2 = taylor_expand<2>(H, y_0);
        constexpr auto y3 = taylor_expand<3>(H, y_0);

        static_assert(y0 == 1_c);
        static_assert(y1 == 1_c + t);
        static_assert(y2 == 1_c + t + t*t);
        static_assert(y3 == 1_c + t + t*t + t*t*t);

        constexpr auto y = taylor_expand<20>(H, y_0);

        for (auto t_ : {-0.9, -0.75, -0.5, -0.2, -0.1, 0.0, 0.1, 0.2, 0.5}) {
            EXPECT_NEAR(y(0, 0, t_), y_exact(t_), std::abs(util::pow(t_, 20)));
        }
    }
}

TEST_F(TaylorSeriesTest, Exponential_2_Is_Correct) {
    using namespace operators;
    using namespace literals;

    // constexpr static auto ln2 = 0.69314718055994530941723212145818_c;
    constexpr static auto ln2 = 0.6931_c;

    constexpr auto H = [] (auto y) { return ln2*y; };

    constexpr auto y_0 = 1_c;

    constexpr auto y_exact = [] (double t_) { return std::exp2(t_); };

    {
        constexpr auto y0 = taylor_expand<0>(H, y_0);
        constexpr auto y1 = taylor_expand<1>(H, y_0);
        constexpr auto y2 = taylor_expand<2>(H, y_0);
        constexpr auto y3 = taylor_expand<3>(H, y_0);

        constexpr auto tau = ln2*t;

        static_assert(y0 == 1);
        static_assert(y1 == 1_c + tau);
        static_assert(y2 == 1_c + tau + (tau^2_c)/2_c);
        static_assert(y3 == 1_c + tau + (tau^2_c)/2_c + (tau^3_c)/6_c);

        constexpr auto y = taylor_expand<4>(H, y_0);

        for (auto t_ : {-0.9, -0.75, -0.5, -0.2, -0.1, 0.0, 0.1, 0.2, 0.5, 0.75, 0.9}) {
            EXPECT_NEAR(y(0, 0, t_), y_exact(t_), std::abs(util::pow(t_, 4)));
        }
    }
}

TEST_F(TaylorSeriesTest, Exponential_Minus_One_Is_Correct) {
    using namespace operators;
    using namespace literals;

    constexpr auto H = [] (auto y) { return 1_c + y; };

    constexpr auto y_0 = 0_c;

    constexpr auto y_exact = [] (double t_) { return std::expm1(t_); };

    {
        constexpr auto y0 = taylor_expand<0>(H, y_0);
        constexpr auto y1 = taylor_expand<1>(H, y_0);
        constexpr auto y2 = taylor_expand<2>(H, y_0);
        constexpr auto y3 = taylor_expand<3>(H, y_0);
        static_assert(y0 == 0);
        static_assert(y1 == t);
        static_assert(y2 == t + (t^2_c)/2_c);
        static_assert(y3 == t + (t^2_c)/2_c + (t^3_c)/6_c);

        constexpr auto y = taylor_expand<20>(H, y_0);

        for (auto t_ : {-0.9, -0.75, -0.5, -0.2, -0.1, 0.0, 0.1, 0.2, 0.5, 0.75, 0.9}) {
            EXPECT_NEAR(y(0, 0, t_), y_exact(t_), std::abs(util::pow(t_, 20)));
        }
    }
}

TEST_F(TaylorSeriesTest, Logarithm_Is_Correct) {
    using namespace operators;
    using namespace literals;

    constexpr auto H = [] (auto) { return 1_c/t; };

    constexpr auto y_0 = 0_c;
    constexpr auto t_0 = 1_c;

    constexpr auto y_exact = [] (double t_) { return std::log(t_); };

    {
        constexpr auto y0 = taylor_expand<0>(H, y_0, t_0);
        constexpr auto y1 = taylor_expand<1>(H, y_0, t_0);
        constexpr auto y2 = taylor_expand<2>(H, y_0, t_0);
        constexpr auto y3 = taylor_expand<3>(H, y_0, t_0);

        {
            constexpr auto t_ = t - t_0;

            static_assert(y0 == 0);
            static_assert(y1 == t_);
            static_assert(y2 == t_ + (-1_c*(t_^2_c))/2_c);
            static_assert(y3 == t_ + (-1_c*(t_^2_c))/2_c + (t_^3_c)/3_c);
        }

        constexpr auto y = taylor_expand<20>(H, y_0, t_0);

        for (auto t_ : {-0.9, -0.75, -0.5, -0.2, -0.1, 0.0, 0.1, 0.2, 0.5, 0.75, 0.9}) {
            const auto tau = 1.0 + t_;
            EXPECT_NEAR(y(0, 0, tau), y_exact(tau), std::abs(util::pow(t_, 15)));
        }
    }
}

TEST_F(TaylorSeriesTest, Logarithm_10_Is_Correct) {
    using namespace operators;
    using namespace literals;

    constexpr auto ln10 = 2.3026_c; // 2.3025850929940456840179914546844_c

    constexpr auto H = [ln10] (auto) { return 1_c/(ln10*t); };

    constexpr auto y_0 = 0_c;
    constexpr auto t_0 = 1_c;

    constexpr auto y_exact = [] (double t_) { return std::log10(t_); };

    {
        constexpr auto y0 = taylor_expand<0>(H, y_0, t_0);
        constexpr auto y1 = taylor_expand<1>(H, y_0, t_0);
        constexpr auto y2 = taylor_expand<2>(H, y_0, t_0);
        constexpr auto y3 = taylor_expand<3>(H, y_0, t_0);

        {
            constexpr auto t_ = (t - t_0);

            static_assert(y0 == 0);
            static_assert(y1 == t_/ln10);
            static_assert(y2 == t_/ln10 + ((-1_c*(t_^2_c))/2_c)/ln10);
            static_assert(y3 == t_/ln10 + ((-1_c*(t_^2_c))/2_c)/ln10 + ((t_^3_c)/3_c)/ln10);
        }

        constexpr auto y = taylor_expand<15>(H, y_0, t_0);

        for (auto t_ : {-0.9, -0.75, -0.5, -0.2, -0.1, 0.0, 0.1, 0.2, 0.5, 0.75, 0.9}) {
            const auto tau = 1.0 + t_;
            EXPECT_NEAR(y(0, 0, tau), y_exact(tau), std::abs(util::pow(t_, 6)));
        }
    }
}

TEST_F(TaylorSeriesTest, Logarithm_1_Plus_T_Is_Correct) {
    using namespace operators;
    using namespace literals;

    constexpr auto H = [] (auto) { return 1_c/(1_c + t); };

    constexpr auto y_0 = 0_c;

    constexpr auto y_exact = [] (double t_) { return std::log(1.0 + t_); };

    {
        constexpr auto y0 = taylor_expand<0>(H, y_0);
        constexpr auto y1 = taylor_expand<1>(H, y_0);
        constexpr auto y2 = taylor_expand<2>(H, y_0);
        constexpr auto y3 = taylor_expand<3>(H, y_0);

        static_assert(y0 == 0);
        static_assert(y1 == t);
        static_assert(y2 == t + (-1_c*t*t)/2_c);
        static_assert(y3 == t + (-1_c*t*t)/2_c + (t^3_c)/3_c);

        constexpr auto y = taylor_expand<20>(H, y_0);

        for (auto t_ : {-0.9, -0.75, -0.5, -0.2, -0.1, 0.0, 0.1, 0.2, 0.5, 0.75, 0.9}) {
            EXPECT_NEAR(y(0, 0, t_), y_exact(t_), std::abs(util::pow(t_, 15)));
        }
    }
}

TEST_F(TaylorSeriesTest, Pow_Is_correct) {
    using namespace operators;
    using namespace literals;

    constexpr auto H = [] (auto y) { return x*y/t; };

    constexpr auto y_0 = 1_c;
    constexpr auto t_0 = 1_c;

    constexpr auto y_exact = [] (double t_, double x_) { return std::pow(t_, x_); };

    {
        constexpr auto y0 = taylor_expand<0>(H, y_0, t_0);
        constexpr auto y1 = taylor_expand<1>(H, y_0, t_0);
        constexpr auto y2 = taylor_expand<2>(H, y_0, t_0);
        constexpr auto y3 = taylor_expand<3>(H, y_0, t_0);
      
        {
            constexpr auto t_ = t - t_0;

            std::cout << y0 << std::endl;
            std::cout << y1 << std::endl;
            std::cout << y2 << std::endl;
            std::cout << y3 << "\t" << 1_c + x*t_ + (1_c/2_c)*(x*x + (-1_c*x))*(t_^2_c) + (1_c/6_c)*((x^3_c) + (-2_c*x^2_c) + x + (-1_c)*(x*x + (-1_c*x))*(t_^3_c)) << std::endl;

            static_assert(y0 == 1);
            static_assert(y1 == 1_c + x*t_);
            static_assert(y2 == 1_c + x*t_ + (1_c/2_c)*(x*x + (-1_c*x))*(t_^2_c));
            static_assert(y3 == 1_c + x*t_ + (1_c/2_c)*(x*x + (-1_c*x))*(t_^2_c) + (1_c/6_c)*((x^3_c) + (-2_c*x^2_c) + x + (-1_c)*(x*x + (-1_c*x))*(t_^3_c)));
        }
        /*constexpr auto y = taylor_expand<20>(H, y_0, t_0);

        for (auto t_ : {-0.9, -0.75, -0.5, -0.2, -0.1, 0.0, 0.1, 0.2, 0.5, 0.75, 0.9}) {
            EXPECT_NEAR(y(0, 0, t_), y_exact(t_), std::abs(util::pow(t_, 20)));
        }*/
    }
}

TEST_F(TaylorSeriesTest, Square_Root_Is_Correct) {
    using namespace operators;
    using namespace literals;

    constexpr auto H = [] (auto y) { return 0.5_c/y; };

    constexpr auto y_0 = 1_c;

    constexpr auto y_exact = [] (double t_) { return std::sqrt(1.0 + t_); };

    {
        constexpr auto y0 = taylor_expand<0>(H, y_0);
        constexpr auto y1 = taylor_expand<1>(H, y_0);
        constexpr auto y2 = taylor_expand<2>(H, y_0);
        constexpr auto y3 = taylor_expand<3>(H, y_0);

        static_assert(y0 == 1);
        static_assert(y1 == 1_c + t/2_c);
        static_assert(y2 == 1_c + t/2_c + (-1_c*t*t)/8_c);
        static_assert(y3 == 1_c + t/2_c + (-1_c*t*t)/8_c + (t^3_c)/16_c);

        constexpr auto y = taylor_expand<18>(H, y_0);

        for (auto t_ : {-0.9, -0.75, -0.5, -0.2, -0.1, 0.0, 0.1, 0.2, 0.5, 0.75, 0.9}) {
            EXPECT_NEAR(y(0, 0, t_), y_exact(t_), std::abs(util::pow(t_, 20)));
        }
    }
}

TEST_F(TaylorSeriesTest, Inverse_Square_Root_Is_Correct) {
    using namespace operators;
    using namespace literals;

    constexpr auto H = [] (auto y) { return -0.5_c*(y^3_c); };

    constexpr auto y_0 = 1_c;

    constexpr auto y_exact = [] (double t_) { return 1.0/std::sqrt(1.0 + t_); };

    {
        constexpr auto y0 = taylor_expand<0>(H, y_0);
        constexpr auto y1 = taylor_expand<1>(H, y_0);
        constexpr auto y2 = taylor_expand<2>(H, y_0);
        constexpr auto y3 = taylor_expand<3>(H, y_0);


        static_assert(y0 == 1);
        static_assert(y1 == 1_c + (-1_c/2_c)*t);
        static_assert(y2 == 1_c + (-1_c/2_c)*t + (3_c/8_c)*t*t);
        static_assert(y3 == 1_c + (-1_c/2_c)*t + (3_c/8_c)*t*t + (-5_c/16_c)*(t^3_c));

        constexpr auto y = taylor_expand<17>(H, y_0);

        for (auto t_ : {-0.9, -0.75, -0.5, -0.2, -0.1, 0.0, 0.1, 0.2, 0.5, 0.75, 0.9}) {
            EXPECT_NEAR(y(0, 0, t_), y_exact(t_), std::abs(util::pow(t_, 15)));
        }
    }
}

TEST_F(TaylorSeriesTest, Tan_Is_Correct) {
    using namespace operators;
    using namespace literals;

    constexpr auto H = [] (auto y) { return 1_c + y*y; };

    constexpr auto y_0 = 0_c;

    constexpr auto y_exact = [] (double t_) { return std::tan(t_); };

    {
        constexpr auto y0 = taylor_expand<0>(H, y_0);
        constexpr auto y1 = taylor_expand<1>(H, y_0);
        constexpr auto y2 = taylor_expand<2>(H, y_0);
        constexpr auto y3 = taylor_expand<3>(H, y_0);
        constexpr auto y5 = taylor_expand<5>(H, y_0);
        constexpr auto y7 = taylor_expand<7>(H, y_0);

        static_assert(y0 == 0);
        static_assert(y1 == t);
        static_assert(y2 == t);
        static_assert(y3 == t + (t^3_c)/3_c);
        static_assert(y5 == t + (t^3_c)/3_c + (2_c/15_c)*(t^5_c));
        static_assert(y7 == t + (t^3_c)/3_c + (2_c/15_c)*(t^5_c) + (17_c/315_c)*(t^7_c));

        constexpr auto y = taylor_expand<9>(H, y_0);

        for (auto t_ : {-0.9, -0.75, -0.5, -0.2, -0.1, 0.0, 0.1, 0.2, 0.5, 0.75, 0.9}) {
            EXPECT_NEAR(y(0, 0, t_), y_exact(t_), std::abs(util::pow(t_, 5)));
        }
    }
}

}   // test
}   // fields