#include "../addition.hpp"

#include <gtest/gtest.h>

namespace fields {
namespace test {

struct A {
    template <typename T> constexpr auto operator()(T) const { return 3; }
};
struct B {
    template <typename T> constexpr auto operator()(T x) const { return 2*x; }
};
struct C {
    template <typename T> constexpr auto operator()(T x) const { return 4*x; }
};
struct D {
    template <typename T> constexpr auto operator()(T x) const { return x*x; }
};

struct dA_dx {};
struct dB_dx {};
struct dC_dx {};
struct dD_dx {};

constexpr auto d_dx(A) -> dA_dx { return {}; }
constexpr auto d_dx(B) -> dB_dx { return {}; }
constexpr auto d_dx(C) -> dC_dx { return {}; }
constexpr auto d_dx(D) -> dD_dx { return {}; }

class AdditionTest : public ::testing::Test {
 protected:
    constexpr static auto a = A{};
    constexpr static auto b = B{};
    constexpr static auto c = C{};
    constexpr static auto d = D{};
};

TEST_F(AdditionTest, Addition_Is_Correct) {
    using namespace operators;
    using namespace literals;

    {
        constexpr auto y = a + b + c + d;   // x^2 + 6x + 3

        static_assert(y(2) == 19, "(x^2 + 6x + 3)(2) != 19");

        static_assert(std::is_same_v<std::decay_t<decltype(a)>, std::decay_t<decltype(y.get<0>())>>);
        static_assert(std::is_same_v<std::decay_t<decltype(d)>, std::decay_t<decltype(y.get<3>())>>);

        static_assert(b + c == y.sub_sum<1, 3>());

        static_assert((a + b) + c == a + (b + c), "+ is not associative");


        constexpr auto f = a + b;
        constexpr auto g = c + d;

        static_assert(f + g == y, "(a + b) + (c + d) != a + b + c + d");

    }

    static_assert(a + a == 2_c*a);
    static_assert(b*a + a == (b + 1_c)*a);
    static_assert(a + b*a == (1_c + b)*a);
    static_assert(a*b + a == a*(b + 1_c));
    static_assert(a + a*b == a*(1_c + b));
    static_assert(a*b*c + a == a*(b*c + 1_c));
    static_assert(a + a*b*c == a*(1_c + b*c));
    static_assert(b*c + a*b*c == (1_c + a)*b*c);
    static_assert(a*b*c + b*c == (a + 1_c)*b*c);
}

TEST_F(AdditionTest, Subtraction_Is_Correct) {
    using namespace operators;
    using namespace literals;

    static_assert((a - a) == 0);
    static_assert(b*a - a == (b - 1_c)*a);
    static_assert(a - b*a == (1_c - b)*a);
}

TEST_F(AdditionTest, Derivative_Is_Correct) {
    using namespace operators;

    constexpr auto da_dx = dA_dx{};
    constexpr auto db_dx = dB_dx{};
    constexpr auto dc_dx = dC_dx{};
    constexpr auto dd_dx = dD_dx{};

    static_assert(d_dx(a + b) == da_dx + db_dx, "d_dx(a + b) != da_dx + db_dx");
    static_assert(d_dx(a + b + c) == da_dx + db_dx + dc_dx, "d_dx(a + b + c) != da_dx + db_dx + dc_dx");
    static_assert(d_dx(a + b + c + d) == da_dx + db_dx + dc_dx + dd_dx, "d_dx(a + b + c + d) != da_dx + db_dx + dc_dx + dd_dx");
}


}   // test
}   // fields