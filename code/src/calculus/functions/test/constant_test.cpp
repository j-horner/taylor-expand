#include "../constant.hpp"

#include <gtest/gtest.h>

#include <limits>
#include <type_traits>

namespace fields {
namespace test {

// Class to test the integration of various equations
class ConstantTest : public ::testing::Test {
 protected:
};

TEST_F(ConstantTest, LiteralTest) {
    using namespace literals;
    using namespace operators;


    constexpr auto a = 123_c;
    constexpr auto b = 99999_c;
    constexpr auto c = 9223372036854775807_c;
    constexpr auto d = -9223372036854775807_c;
    constexpr auto e = 0_c;

    static_assert(std::is_same_v<std::decay_t<decltype(a)>, Constant<123>>, "a is not Constant<123>");
    static_assert(std::is_same_v<std::decay_t<decltype(b)>, Constant<99999>>, "b is not Constant<99999>");
    static_assert(std::is_same_v<std::decay_t<decltype(c)>, Constant<9223372036854775807>>, "c is not Constant<9223372036854775807>");
    static_assert(std::is_same_v<std::decay_t<decltype(d)>, Constant<-9223372036854775807>>, "d is not Constant<-9223372036854775807>");
    static_assert(std::is_same_v<std::decay_t<decltype(e)>, Constant<0>>, "e is not Constant<0>");
}

}   // test
}   // fields