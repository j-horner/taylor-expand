#include "../linear.hpp"

#include <gtest/gtest.h>

#include "../../operators.hpp"

namespace fields {
namespace test {

class LinearTest : public ::testing::Test {

};

TEST_F(LinearTest, DerivativeIsCorrect) {
    using namespace operators;
    
    {
        constexpr auto y = x;
        static_assert(y(10) == 10, "x(10) != 10");
        static_assert(d_dx(y) == 1, "dx/dx != 1");
    }
    {
        constexpr auto y = x + x;
        static_assert(y(1) == 2, "2x(1) != 2");
        static_assert(d_dx(y) == 2, "d(2x)/dx != 2");
    }
}

}
}