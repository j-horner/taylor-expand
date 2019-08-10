
#include "../multiplication.hpp"
#include "../division.hpp"

namespace fields {

// -------------------------------------------------------------------------------------------------
//                                      / operations
// -------------------------------------------------------------------------------------------------

// Multiplication with common factors
template <typename F, typename G>
constexpr auto operator/(Multiplication<G, F> m, F) { return m.template get<0>(); }

template <typename F, typename G>
constexpr auto operator/(F, Multiplication<G, F> m) { return Constant<1, 1>{} / m.template get<0>(); }

template <typename F, typename... Gs>
constexpr auto operator/(Multiplication<F, Gs...> m, F) { return m.template sub_product<1, sizeof...(Gs) + 1>(); }

template <typename F, typename... Gs>
constexpr auto operator/(F, Multiplication<F, Gs...> m) { return Constant<1, 1>{} / m.template sub_product<1, sizeof...(Gs) + 1>(); }

template <typename F, typename... Gs>
constexpr auto operator/(Multiplication<F, Gs...> lhs, Multiplication<Gs...>) { return lhs.template get<0>(); }

template <typename F, typename... Gs>
constexpr auto operator/(Multiplication<Gs...>, Multiplication<F, Gs...> rhs) { return Constant<1>{}/rhs.template get<0>(); }

template <typename F, typename G, typename... Gs>
constexpr auto operator/(Multiplication<G, F> lhs, Multiplication<F, Gs...> rhs) { return lhs.template get<0>() / rhs.template sub_product<1, sizeof...(Gs) + 1>(); }

template <typename F, typename G, typename... Gs>
constexpr auto operator/(Multiplication<F, Gs...> lhs, Multiplication<G, F> rhs) { return lhs.template sub_product<1, sizeof...(Gs) + 1>() / rhs.template get<0>(); }

template <typename F, typename G, typename H>
constexpr auto operator/(Multiplication<G, F> lhs, Multiplication<H, F> rhs) { return lhs.template get<0>() / rhs.template get<0>(); }

// Division with Multiplication
template <typename F, typename G, typename... Gs>
constexpr auto operator/(Multiplication<Gs...> lhs, Division<F, G> rhs) { return lhs * rhs.rhs / rhs.lhs; }

template <typename F, typename G, typename... Gs>
constexpr auto operator/(Division<F, G> lhs, Multiplication<Gs...> rhs) { return lhs.lhs / (lhs.rhs * rhs); }

// Division with common factors
template <typename F, typename G, typename H>
constexpr auto operator/(Division<F, G> lhs, Division<F, H> rhs) { return rhs.rhs / lhs.rhs; }

template <typename F, typename G, typename H>
constexpr auto operator/(Division<G, F> lhs, Division<H, F> rhs) { return lhs.lhs / rhs.lhs; }

// Division with unrelated factors
template <typename A, typename B, typename C, typename D>
constexpr auto operator/(Division<A, B> lhs, Division<C, D> rhs) { return (lhs.lhs * rhs.rhs) / (lhs.rhs * rhs.lhs); }

// division by constant is equivalent to multiplying by inverse
template <typename F, Int A, Int B>
constexpr auto operator/(F f, Constant<A, B> c) {
	return (Constant<1>{}/c)*f;
}

template <typename F, typename G>
constexpr auto operator/(F lhs, G rhs) {
	static_assert(false == std::is_same_v<decltype(rhs), Constant<0>>, "Cannot divide by 0!");

	static_assert(sizeof(lhs) > 0, "silence unused variable warning");
	static_assert(sizeof(rhs) > 0, "silence unused variable warning");

	if constexpr (std::is_same_v<decltype(lhs), Constant<0>>) {
		return Constant<0>{};
	}
	else {
		return Division<F, G>{lhs, rhs};
	}
}

}	// fields