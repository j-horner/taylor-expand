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


template <typename T1, typename T2>
struct is_same : std::is_same<T1, T2> {};


namespace detail {


template <typename... Ts>
struct count : std::integral_constant<int, 0> {};

template <typename T, std::size_t... Idx, typename... Ts>
struct count<T, std::index_sequence<Idx...>, std::tuple<Ts...>> : std::integral_constant<int, (static_cast<int>(util::is_same<T, std::decay_t<std::tuple_element_t<Idx, std::tuple<Ts...>>>>::value) + ...)>{};

}   // detail


template <typename... Ts>
struct count : std::integral_constant<int, 0> {};

template <typename T>
struct count<T, std::tuple<>> : std::integral_constant<int, 0> {};

template <typename T, typename... Ts>
struct count<T, std::tuple<Ts...>> : std::integral_constant<int, ((sizeof...(Ts) == 0) ? 0
                                                                                       : detail::count<T, std::make_index_sequence<sizeof...(Ts)>, std::tuple<Ts...>>::value)> {};

template <typename T1, typename T2>
struct is_permutation : std::false_type {};


template <typename... Ts, typename... Us>
struct is_permutation<std::tuple<Ts...>, std::tuple<Us...>> : std::bool_constant<(sizeof...(Ts) > sizeof...(Us)) ? ((count<Ts, std::tuple<Ts...>>::value == count<Ts, std::tuple<Us...>>::value) && ...)
                                                                                                                : ((count<Us, std::tuple<Ts...>>::value == count<Us, std::tuple<Us...>>::value) && ...)> {};


}   // util
}   // fields
