#pragma once

#include "functions/constant.hpp"

#include "addition.hpp"

// C++ headers
#include <type_traits>

namespace fields {
namespace operators {

template <typename F, typename G>
constexpr auto operator==(F lhs, G rhs) {
    constexpr auto default_comparison = std::is_same_v<std::decay_t<decltype(lhs)>, std::decay_t<decltype(rhs)>>;

    if constexpr (detail::is_constant<F>::value && std::is_arithmetic_v<G>) {
        return static_cast<G>(lhs) == rhs;
    } else if constexpr (detail::is_constant<G>::value && std::is_arithmetic_v<F>) {
        return static_cast<F>(rhs) == lhs;
    } else {
        return default_comparison;
    }
}


// https://stackoverflow.com/questions/40117987/how-do-i-compare-tuples-for-equivalent-types-disregarding-type-order#40118456
namespace detail {

template <typename T, typename Tuple>
struct type_counter;

template <typename T, typename ... Ts>
struct type_counter<T, std::tuple<Ts...>> : std::integral_constant<std::size_t, (... + std::is_same<T, Ts>::value)> {};

template <typename Tuple1, typename Tuple2, std::size_t... Is>
constexpr bool equivalent_types(std::index_sequence<Is...>) {
    return (... && (type_counter<std::tuple_element_t<Is, Tuple1>, Tuple1>::value == type_counter<std::tuple_element_t<Is, Tuple1>, Tuple2>::value));
}

template <typename Tuple1, typename Tuple2>
constexpr bool equivalent_types() {
    constexpr auto s1 = std::tuple_size<Tuple1>::value;
    constexpr auto s2 = std::tuple_size<Tuple2>::value;

    return (s1 == s2) && equivalent_types<Tuple1, Tuple2>(std::make_index_sequence<std::min(s1, s2)>());
}

}   //detail

template <typename... Fs>
class Addition;

template <typename... Fs>
class Multiplication;

template <typename... Fs, typename... Gs>
constexpr auto operator==(Addition<Fs...>, Addition<Gs...>) {
    if constexpr (sizeof...(Fs) != sizeof...(Gs)) {
        return false;
    } else {
        return detail::equivalent_types<std::tuple<Fs...>, std::tuple<Gs...>>();
    }
}

template <typename... Fs, typename... Gs>
constexpr auto operator==(Multiplication<Fs...>, Multiplication<Gs...>) {
    if constexpr (sizeof...(Fs) != sizeof...(Gs)) {
        return false;
    } else {
        return detail::equivalent_types<std::tuple<Fs...>, std::tuple<Gs...>>();
    }
}

} // operators
} // fields
