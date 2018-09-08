#include "../power.hpp"

#include <gtest/gtest.h>

namespace fields {
namespace test {

template <typename F, typename G>
constexpr auto operator==(F, G) {
    return std::is_same_v<F, G>;
}

class PowerTest : public ::testing::Test {

};

TEST_F(PowerTest, Power_Operator_Is_Correct) {
    using namespace operators;
    using namespace literals;

    constexpr auto f = []{};

    using F = std::decay_t<decltype(f)>;

    static_assert((f^0_c) == 1_c);
    static_assert((f^1_c) == f);
    static_assert((f^2_c)== Power<F, 2>{f});
    static_assert((f^4_c)== Power<F, 4>{f});
    static_assert((f^-2_c) == Power<F, -2>{f});
    static_assert((f^-4_c) == Power<F, -4>{f});
}

}   // test
}   // fields