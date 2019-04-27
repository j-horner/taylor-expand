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
TEST_F(UtilTest, Count) {
    static_assert(util::count<int>(std::tuple<>{}) == 0);
    static_assert(util::count<int>(std::tuple<int>{}) == 1);
    static_assert(util::count<int>(std::tuple<int, int>{}) == 2);
    static_assert(util::count<int>(std::tuple<int, int, int, int>{}) == 4);
    static_assert(util::count<int>(std::tuple<int, int, int, int, int, int, int, int, int, int, int, int>{}) == 12);


    static_assert(util::count<char>(std::tuple<int, char, int, int, char, int, int, int, char, char, int, int>{}) == 4);
}

TEST_F(UtilTest, IsPermutation) {
    static_assert(util::is_permutation(std::tuple<>{}, std::tuple<>{}));

    static_assert(util::is_permutation(std::tuple<int>{}, std::tuple<int>{}));
    static_assert(util::is_permutation(std::tuple<int, int>{}, std::tuple<int, int>{}));
    static_assert(util::is_permutation(std::tuple<int, int, int, int>{}, std::tuple<int, int, int, int>{}));
    static_assert(util::is_permutation(std::tuple<int, char, double>{}, std::tuple<double, int, char>{}));
    static_assert(util::is_permutation(std::tuple<int, char, unsigned int, double>{}, std::tuple<unsigned int, double, int, char>{}));
    static_assert(util::is_permutation(std::tuple<float, int, float, char, float, double>{}, std::tuple<double, int, char, float, float, float>{}));
    static_assert(util::is_permutation(std::tuple<bool, bool, bool, bool, int, char, double>{}, std::tuple<bool, double, bool, bool, int, char, bool>{}));

    static_assert(false == util::is_permutation(std::tuple<>{}, std::tuple<int>{}));
    static_assert(false == util::is_permutation(std::tuple<int, int>{}, std::tuple<int>{}));
    static_assert(false == util::is_permutation(std::tuple<int, int, int>{}, std::tuple<int, int, int, int>{}));
    static_assert(false == util::is_permutation(std::tuple<int,  double>{}, std::tuple<double, int, char>{}));
    static_assert(false == util::is_permutation(std::tuple<int, char, unsigned int, double>{}, std::tuple<unsigned int, int, char>{}));
    static_assert(false == util::is_permutation(std::tuple<float, int, float, char, float, double>{}, std::tuple<double, int, char, float, float>{}));
    static_assert(false == util::is_permutation(std::tuple<bool, bool, bool, int, char, double>{}, std::tuple<bool, double, bool, bool, int, char, bool>{}));

}

}   // test
}   // util
}   // fields
