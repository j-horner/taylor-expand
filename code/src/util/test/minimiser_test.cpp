#include "../minimiser.hpp"

#include <gtest/gtest.h>

#include <cmath>
#include <iostream>
#include <limits>

namespace fields {
namespace util {
namespace test {

class MinimiserTest : public ::testing::Test {
 protected:
    auto make_quadratic(double a) {
        return  [a] (auto x) {
                    return (x - a)*(x - a);
                };
    }

    auto make_gaussian(double a) {
        return  [a] (auto x) {
                    return -std::exp(-(x - a)*(x - a));
                };
    }

};

TEST_F(MinimiserTest, BracketMinimumTest) {
    auto a = 5.0;
    auto f = make_quadratic(a);
    auto g = make_gaussian(a);

    auto points = bracket_minimum(f);

    auto check_bracket = [&points] {
        // x[0] < x[1] < x[2]
        ASSERT_GT(points.first[1], points.first[0]);
        ASSERT_GT(points.first[2], points.first[1]);

        //  (f(x[0]) > f(x[1])) AND (f(x[1]) < f(x[2]))
        ASSERT_GT(points.second[0], points.second[1]);
        ASSERT_GT(points.second[2], points.second[1]);
    };

    check_bracket();    // check f minimum is bracketed

    points = bracket_minimum(g);

    check_bracket();    // check g minimum is bracketed
}

TEST_F(MinimiserTest, MinimiseTest) {
    const auto tol = std::sqrt(std::numeric_limits<double>::epsilon());
    auto a = 5.0;
    auto f = make_quadratic(a);
    auto g = make_gaussian(a);

    auto minimum = minimise(f, bracket_minimum(f).first).first;
    ASSERT_NEAR(minimum, a, tol*a);

    minimum = minimise(g, bracket_minimum(g).first).first;
    ASSERT_NEAR(minimum, a, tol*a);
}

}   // test
}   // util
}   // fields