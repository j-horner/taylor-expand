#include "../util.hpp"

#include <gtest/gtest.h>

namespace fields {
namespace util {
namespace test {

class UtilTest : public ::testing::Test {

};

TEST_F(UtilTest, Pow_Is_Correct) {
    static_assert(util::pow(2, 3) == 8);
}

TEST_F(UtilTest, Factorial_Is_Correct) {
    static_assert(util::factorial(4) == 24);
}

TEST_F(UtilTest, Binomial_Is_Correct) {
    static_assert(util::binomial(4, 2) == 6);
}

}   // test
}   // util
}   // fields
