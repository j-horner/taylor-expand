#include "../util.hpp"

#include <gtest/gtest.h>

namespace fields {
namespace util {
namespace test {

class UtilTest : public ::testing::Test {

};

TEST_F(UtilTest, PowTestCompileTime) {
    static_assert(util::pow(2, 3) == 8, "Pow is broken!");
}

TEST_F(UtilTest, FactorialTest) {
    static_assert(util::factorial(4) == 24, "Factorial is broken!");
}

TEST_F(UtilTest, BinomialTest) {
    static_assert(util::binomial(4, 2) == 6, "Binomial(4, 2) is broken!");
}

}   // test
}   // util
}   // fields
