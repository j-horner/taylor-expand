#include "../power.hpp"

#include "../../comparison.hpp"

#include <gtest/gtest.h>

namespace fields {
namespace test {

class PowerTest : public ::testing::Test {
};

TEST_F(PowerTest, Power_Operator_Is_Correct) {
    using namespace literals;

    struct F {};
    constexpr auto f = F{};

    static_assert((f^0_c) == 1_c);

    static_assert((f^1_c) == f);

    static_assert((f^2_c) == Power<F, 2>{f});
    static_assert((f^4_c) == Power<F, 4>{f});
    static_assert((f^-2_c) == Power<F, -2>{f});
    static_assert((f^-4_c) == Power<F, -4>{f});

    static_assert(((f^3_c)^2_c) == Power<F, 6>{f});

    static_assert((3_c^3_c) == 27);
    static_assert(((2_c/3_c)^2_c) == (4_c/9_c));
}

}   // test
}   // fields