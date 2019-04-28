#pragma once

#include "functions/constant.hpp"

#include "addition.hpp"

#include "../util/util.hpp"

// C++ headers
#include <tuple>
#include <type_traits>

namespace fields {

namespace operators {

template <typename... Fs>
class Addition;

template <typename... Fs>
class Multiplication;

}   // operators

namespace util {

template <typename... Ts, typename... Us>
struct is_same<operators::Addition<Ts...>, operators::Addition<Us...>> : std::bool_constant<is_permutation<std::tuple<Ts...>, std::tuple<Us...>>::value> {};

template <typename... Ts, typename... Us>
struct is_same<operators::Multiplication<Ts...>, operators::Multiplication<Us...>> : std::bool_constant<is_permutation<std::tuple<Ts...>, std::tuple<Us...>>::value> {};

}   // util

namespace operators {

template <typename... Fs, typename... Gs>
constexpr auto operator==(Addition<Fs...>, Addition<Gs...>) { return util::is_same<Addition<Fs...>, Addition<Gs...>>::value; }

template <typename... Fs, typename... Gs>
constexpr auto operator==(Multiplication<Fs...>, Multiplication<Gs...>) { return util::is_same<Multiplication<Fs...>, Multiplication<Gs...>>::value; }

template <typename F, typename G>
constexpr auto operator==(F lhs, G rhs) {
    static_assert(sizeof(lhs) > 0, "silence unused variable warning");
    static_assert(sizeof(rhs) > 0, "silence unused variable warning");

    if constexpr (detail::is_constant<F>::value && std::is_arithmetic_v<G>) {
        return static_cast<G>(lhs) == rhs;
    } else if constexpr (detail::is_constant<G>::value && std::is_arithmetic_v<F>) {
        return static_cast<F>(rhs) == lhs;
    } else {
        // use decltype to silence "unused warnings"
        return util::is_same<F, G>::value;
    }
}

} // operators
} // fields
