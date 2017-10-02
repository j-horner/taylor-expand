#include "../function_traits.hpp"

#include <gtest/gtest.h>

#include <type_traits>

namespace fields {
namespace util {
namespace test {

class FuntionTraitsTest : public ::testing::Test {
};

TEST_F(FuntionTraitsTest, LambdaTests) {
    auto f = [] {};
    using F = decltype(f);
    static_assert(std::is_same_v<void, function_traits<F>::result_type>, "Empty lambda does not return void!");
    static_assert(0 == function_traits<F>::n_args, "Empty lambda does not have zero arguments!");

    auto g = [f] (double, int) {
        return 1.0f;
    };
    using G = decltype(g);
    static_assert(std::is_same_v<float, function_traits<G>::result_type>, "Lambda does not return float!");
    static_assert(std::is_same_v<double, function_traits<G>::arg<0>::type>, "Lambda first arg is not double!");
    static_assert(std::is_same_v<int, function_traits<G>::arg<1>::type>, "Lambda second arg is not int!");
    static_assert(2 == function_traits<G>::n_args, "Lambda does not have zero arguments!");
}

}   // test
}   // util
}   // fields