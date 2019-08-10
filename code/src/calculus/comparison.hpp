#pragma once

#include "../util/util.hpp"

// C++ headers
#include <tuple>
#include <type_traits>

namespace fields {

template <typename... Fs>
class Addition;

template <typename... Fs>
class Multiplication;

namespace util {

template <typename... Ts, typename... Us>
struct is_same<Addition<Ts...>, Addition<Us...>> : std::bool_constant<is_permutation<std::tuple<Ts...>, std::tuple<Us...>>::value> {};

template <typename... Ts, typename... Us>
struct is_same<Multiplication<Ts...>, Multiplication<Us...>> : std::bool_constant<is_permutation<std::tuple<Ts...>, std::tuple<Us...>>::value> {};

}   // util

template <typename... Fs, typename... Gs>
constexpr auto operator==(Addition<Fs...>, Addition<Gs...>) { return util::is_same<Addition<Fs...>, Addition<Gs...>>::value; }

template <typename... Fs, typename... Gs>
constexpr auto operator==(Multiplication<Fs...>, Multiplication<Gs...>) { return util::is_same<Multiplication<Fs...>, Multiplication<Gs...>>::value; }

template <typename F, typename G>
constexpr auto operator==(F lhs, G rhs) {
    static_assert(sizeof(lhs) > 0, "silence unused variable warning");
    static_assert(sizeof(rhs) > 0, "silence unused variable warning");

	return util::is_same<F, G>::value;
}

} // fields
