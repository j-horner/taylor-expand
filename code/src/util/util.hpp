#pragma once

// C++ headers
#include <tuple>
#include <type_traits>

namespace fields {
namespace util {

template <typename Int>
constexpr auto factorial(Int n) noexcept -> Int {
    return (n == Int{0}) ? Int{1} : n*factorial(n - 1);
}

template <typename Int>
constexpr auto binomial(Int n, Int k) noexcept {
    return factorial(n) / (factorial(k)*factorial(n - k));
}

template <typename T, typename Int>
constexpr auto pow(T x, Int n) noexcept {
    static_assert(std::is_integral_v<Int>, "n must be an integral type.");
    static_assert(std::is_arithmetic_v<T>, "This function is intended to be used with arithmetic types.");

    auto y = T{1};
    for (auto i = 0; i < n; ++i) {
        y *= x;
    }
    return y;
}

template <typename T, typename... Ts>
constexpr auto count(std::tuple<Ts...> t) noexcept -> int;


namespace detail {

template <typename T, std::size_t... Idx, typename... Ts>
constexpr auto count(std::index_sequence<Idx...>, std::tuple<Ts...>) noexcept {
    return (static_cast<int>(std::is_same_v<T, std::decay_t<std::tuple_element_t<Idx, std::tuple<Ts...>>>>) + ...);
}

}   // detail

template <typename T, typename... Ts>
constexpr auto count(std::tuple<Ts...> t) noexcept -> int {
    if constexpr (sizeof...(Ts) == 0) {
        return 0;
    } else {
        return detail::count<T>(std::make_index_sequence<sizeof...(Ts)>{}, t);
    }
}


template <typename... Ts, typename... Us>
constexpr auto is_permutation(std::tuple<Ts...> t1, std::tuple<Us...> t2) noexcept {
    if constexpr (sizeof...(Ts) > sizeof...(Us)) {
        return ((count<Ts>(t1) == count<Ts>(t2)) && ...);
    } else {
        return ((count<Us>(t1) == count<Us>(t2)) && ...);
    }
}

}   // util
}   // fields
