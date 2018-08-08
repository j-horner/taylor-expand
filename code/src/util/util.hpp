#pragma once

// C++ headers
#include <type_traits>

namespace fields {
namespace util {

template <typename Int>
constexpr auto factorial(Int n) -> Int {
    return (n == Int{0}) ? Int{1} : n*factorial(n - 1);
}

template <typename Int>
constexpr auto binomial(Int n, Int k) {
    return factorial(n) / (factorial(k)*factorial(n - k));
}

template <typename T, typename Int>
constexpr auto pow(T x, Int n) {
    static_assert(std::is_integral_v<Int>, "n must be an integral type.");

    auto y = T{1};
    for (auto i = 0; i < n; ++i) {
        y *= x;
    }
    return y;
}

}   // util
}   // fields
