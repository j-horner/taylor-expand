#pragma once

#include "multiplication.hpp"

#include "../util/util.hpp"

// C++ headers
#include <type_traits>

namespace fields {

template <typename F, typename G>
class Subtraction {
public:
	static_assert(fields::detail::is_multiple<F, G>::value == false, "Subtraction of multiples should cancel out.");
	
	template <typename... Args>
    constexpr auto operator()(Args... args) const { return lhs(args...) - rhs(args...); }

    F lhs;
    G rhs;
};

template <typename Stream, typename F, typename G>
auto& operator<<(Stream& os, Subtraction<F, G> y) {
    os << "(" << y.lhs << " - " << y.rhs << ")";
    return os;
}

template <typename... Ts>
class Addition;

namespace detail {

template <typename T, typename U>
struct is_subtraction : std::false_type {};

template <typename T, typename F>
struct is_subtraction<T, Subtraction<F, T>> : std::true_type {};

template <typename T, typename F>
struct is_subtraction<T, Subtraction<T, F>> : std::true_type {};

template <typename T, typename F>
struct is_subtraction<Subtraction<F, T>, T> : std::true_type {};

template <typename T, typename F>
struct is_subtraction<Subtraction<T, F>, T> : std::true_type {};

template <typename... Ts>
struct contains_subtraction;

template <typename T, typename... Ts>
struct contains_subtraction<T, Addition<Ts...>> {
	static_assert(sizeof...(Ts) >= 0);
	constexpr static auto index = util::tuple_index<std::tuple<Ts...>, T, fields::detail::is_subtraction>::value;
};

}	// detail

} // fields
