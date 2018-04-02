#include "../addition.hpp"

#include <gtest/gtest.h>

namespace fields {
namespace test {

struct A {};
struct B {};
struct C {};
struct D {};

struct dA_dx {};
struct dB_dx {};
struct dC_dx {};
struct dD_dx {};

auto d_dx(A) -> dA_dx { return {}; }
auto d_dx(B) -> dB_dx { return {}; }
auto d_dx(C) -> dC_dx { return {}; }
auto d_dx(D) -> dD_dx { return {}; }

class AdditionTest : public ::testing::Test {
};

TEST_F(AdditionTest, Addition_Is_Correct) {
    using namespace operators;

    constexpr auto a = [] (auto) { return 3; };
    constexpr auto b = [] (auto x) { return 2*x; };
    constexpr auto c = [] (auto x) { return 4*x; };
    constexpr auto d = [] (auto x) { return x*x; };

    constexpr auto y = a + b + c + d;   // x^2 + 6x + 3

    static_assert(y(2) == 19, "(x^2 + 6x + 3)(2) != 19");

    static_assert(std::is_same_v<std::decay_t<decltype(a)>, std::decay_t<decltype(y.get<0>())>>);
    static_assert(std::is_same_v<std::decay_t<decltype(d)>, std::decay_t<decltype(y.get<3>())>>);

    static_assert(std::is_same_v<std::decay_t<decltype(b + c)>, std::decay_t<decltype(y.sub_sum<1, 3>())>>);

    constexpr auto f = a + b;
    constexpr auto g = c + d;

    static_assert(std::is_same_v<std::decay_t<decltype(f + g)>, std::decay_t<decltype(y)>>, "(a*b)*(c*d) != a*b*c*d");
}

TEST_F(AdditionTest, Derivative_Is_Correct) {
    using namespace operators;

    constexpr auto a = A{};
    constexpr auto b = B{};
    constexpr auto c = C{};
    constexpr auto d = D{};

    constexpr auto da_dx = dA_dx{};
    constexpr auto db_dx = dB_dx{};
    constexpr auto dc_dx = dC_dx{};
    constexpr auto dd_dx = dD_dx{};

    static_assert(std::is_same_v<decltype(d_dx(a + b)), decltype(da_dx + db_dx)>, "d_dx(a + b) != da_dx + db_dx");
    static_assert(std::is_same_v<decltype(d_dx(a + b + c)), decltype(da_dx + db_dx + dc_dx)>, "d_dx(a + b + c) != da_dx + db_dx + dc_dx");
    static_assert(std::is_same_v<decltype(d_dx(a + b + c + d)), decltype(da_dx + db_dx + dc_dx + dd_dx)>, "d_dx(a + b + c + d) != da_dx + db_dx + dc_dx + dd_dx");
}


}   // test
}   // fields