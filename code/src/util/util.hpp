#ifndef UTIL_UTIL_HPP_
#define UTIL_UTIL_HPP_

namespace fields {
namespace util {

constexpr auto factorial(int n) -> int {
    return (n == 0) ? 1 : n*factorial(n - 1);
}

constexpr auto binomial(int n, int k) -> int {
    return factorial(n) / (factorial(k)*factorial(n - k));
}

template <typename T>
constexpr auto pow(T x, int n) -> T {
    auto y = T{1};
    for (auto i = 0; i < n; ++i) {
        y *= x;
    }
    return y;
}

}   // util
}   // fields

#endif  // UTIL_UTIL_HPP_