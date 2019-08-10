#include "../constant.hpp"

#include "../../comparison.hpp"

#include <gtest/gtest.h>

#include <limits>
#include <type_traits>

namespace fields {
namespace test {

// Class to test the integration of various equations
class ConstantTest : public ::testing::Test {
 protected:
};

TEST_F(ConstantTest, Literal_Operator_Is_Correct) {
    using namespace literals;

    constexpr auto a = 123_c;
    constexpr auto b = 99999_c;
    constexpr auto c = 9223372036854775807_c;
    constexpr auto d = -9223372036854775807_c;
    constexpr auto e = 0_c;

    constexpr auto f = a*4_c/2_c;

    static_assert(std::is_same_v<std::decay_t<decltype(a)>, Constant<123>>);
    static_assert(std::is_same_v<std::decay_t<decltype(b)>, Constant<99999>>);
    static_assert(std::is_same_v<std::decay_t<decltype(c)>, Constant<9223372036854775807>>);
    static_assert(std::is_same_v<std::decay_t<decltype(d)>, Constant<-9223372036854775807>>);
    static_assert(std::is_same_v<std::decay_t<decltype(e)>, Constant<0>>);
    static_assert(std::is_same_v<std::decay_t<decltype(f)>, Constant<246>>);

    constexpr auto g = 0.1_c;
    constexpr auto h = 0.001100_c;
    constexpr auto i = 1.25_c;

    static_assert(std::is_same_v<std::decay_t<decltype(g)>, Constant<1, 10>>);
    static_assert(std::is_same_v<std::decay_t<decltype(h)>, Constant<11, 10000>>);
    static_assert(std::is_same_v<std::decay_t<decltype(i)>, Constant<5, 4>>);


    static_assert(0_c + 0_c + 0_c + 0_c == 0_c);
    static_assert(0_c * 0_c * 0_c * 0_c == 0_c);

    static_assert(10_c * 0_c == 0_c);
    static_assert(52_c * 1_c == 52_c);

    static_assert((3_c * 10_c + 46_c - 5_c / 6_c) / (2_c ^ 4_c) == 451_c/96_c);     // 451/96 = 11*41/96 = 11*41/(3*32)

    static_assert(5_c == 5);
    static_assert(0.5 == 0.5_c);

    static_assert(4_c + 11 == 15);
    static_assert(2_c - 7 == -5);
    static_assert(23_c*3 == 69);
    static_assert(32_c/4 == 8);

}

}   // test
}   // fields
