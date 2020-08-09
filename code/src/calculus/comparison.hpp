#pragma once

#include "../util/util.hpp"

#include "functions/constant.hpp"

// C++ headers
#include <tuple>
#include <type_traits>

namespace fields {

template <typename... Fs>
class Addition;

template <typename T, typename U>
class Subtraction;

template <typename... Fs>
class Multiplication;

namespace util {

template <typename... Ts, typename... Us>
struct is_same<Addition<Ts...>, Addition<Us...>> : std::bool_constant<is_permutation<std::tuple<Ts...>, std::tuple<Us...>>::value> {};

template <typename... Ts, typename... Us>
struct is_same<Multiplication<Ts...>, Multiplication<Us...>> : std::bool_constant<is_permutation<std::tuple<Ts...>, std::tuple<Us...>>::value> {};

template <typename F, typename G>
struct is_same<Addition<F, Multiplication<Constant<-1>, G>>, Subtraction<F, G>> : std::true_type {};

template <typename... Fs, typename G>
struct is_same<Addition<Fs..., Multiplication<Constant<-1>, G>>, Subtraction<Addition<Fs...>, G>> : std::true_type {};

template <typename F, typename... Gs>
struct is_same<Addition<F, Multiplication<Constant<-1>, Gs...>>, Subtraction<F, Multiplication<Gs...>>> : std::true_type {};

template <typename... Fs, typename... Gs>
struct is_same<Addition<Fs..., Multiplication<Constant<-1>, Gs...>>, Subtraction<Addition<Fs...>, Multiplication<Gs...>>> : std::true_type {};

template <typename... Fs, typename... Gs>
struct is_same<Multiplication<Multiplication<Fs...>, Gs...>, Multiplication<Fs..., Gs...>> : std::true_type {
};

template <typename... Fs, typename... Gs>
struct is_same<Multiplication<Fs..., Gs...>, Multiplication<Multiplication<Fs...>, Gs...>> : std::true_type {
};

template <typename... Fs, typename... Gs>
struct is_same<Addition<Addition<Fs...>, Gs...>, Addition<Fs..., Gs...>> : std::true_type {};

template <typename... Fs, typename... Gs>
struct is_same<Addition<Fs..., Gs...>, Addition<Addition<Fs...>, Gs...>> : std::true_type {};

template <typename... Ts>
struct contains;

template <typename T, typename... Fs, typename... Gs>
struct contains<T, Addition<Addition<Fs...>, Gs...>> : public contains<T, Addition<Fs..., Gs...>> {
};

template <typename T, typename... Fs, typename... Gs>
struct contains<T, Multiplication<Multiplication<Fs...>, Gs...>> : public contains<T, Multiplication<Fs..., Gs...>> {
};

template <typename T, typename... Ts>
struct contains<T, Addition<Ts...>> {
	static_assert(sizeof...(Ts) >= 0);
	constexpr static auto index = util::tuple_index<std::tuple<Ts...>, T, is_same>::value;
};

template <typename T, typename... Ts>
struct contains<T, Multiplication<Ts...>> {
	static_assert(sizeof...(Ts) >= 0);
	constexpr static auto index = util::tuple_index<std::tuple<Ts...>, T, is_same>::value;
};

/*template <Int A, Int B, typename... Ts>
struct contains<Constant<A, B>, Multiplication<Ts...>> {
	static_assert(sizeof...(Ts) >= 0);
	constexpr static auto index = 0;		// a constant can always be factored out of a Multiplication
};*/

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
