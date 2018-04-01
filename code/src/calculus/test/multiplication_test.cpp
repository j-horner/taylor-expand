#include "../multiply.hpp"

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

class MultiplicationTest : public ::testing::Test {
};

TEST_F(MultiplicationTest, Mutliplication_Is_Correct) {
    using namespace operators;

    constexpr auto a = [] (auto) { return 3; };
    constexpr auto b = [] (auto x) { return 2*x; };
    constexpr auto c = [] (auto x) { return 4*x; };
    constexpr auto d = [] (auto x) { return x*x; };

    constexpr auto y = a*b*c*d; 

    static_assert(y(2) == 384, "3*2x*4x*x*x(2) != 384");

    static_assert(false, "Need to think about order here, it's getting swapped!");

    a = 2;
    y.get<0>() = 2;

    static_assert(std::is_same_v<std::decay_t<decltype(a)>, std::decay_t<decltype(y.get<0>())>>);
    static_assert(std::is_same_v<std::decay_t<decltype(d)>, std::decay_t<decltype(y.get<3>())>>);
}

TEST_F(MultiplicationTest, Derivative_Is_Correct) {
    using namespace operators;

    constexpr auto a = A{};
    constexpr auto b = B{};
    constexpr auto c = C{};
    constexpr auto d = D{};

    constexpr auto da_dx = dA_dx{};
    constexpr auto db_dx = dB_dx{};
    constexpr auto dc_dx = dC_dx{};
    constexpr auto dd_dx = dD_dx{};

    d_dx(a*b*c) = 5;

    static_assert(std::is_same_v<decltype(d_dx(a*b)), decltype(da_dx*b + a*db_dx)>, "d_dx(a*b) != da_dx*b + a*db_dx");
    // static_assert(std::is_same_v<decltype(d_dx(a*b*c)), decltype(da_dx*b*c + a*db_dx*c + a*b*dc_dx)>, "d_dx(a*b*c) != da_dx*b*c + a*db_dx*c + a*b*dc_dx");
    // static_assert(std::is_same_v<decltype(d_dx(a*b)), decltype(da_dx*b + a*db_dx)>, "d_dx(a*b) != da_dx*b + a*db_dx");


}


}   // test
}   // fields