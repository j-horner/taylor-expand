#pragma once

#include "../functions/constant.hpp"

#include "../addition.hpp"
#include "../subtraction.hpp"

namespace fields {

// -f = (-1)*f
template <typename F>
constexpr auto operator-(F f) {
	return Constant<-1>{}*f;
}

// Definition of 0
template <typename F>
constexpr auto operator-(F, F) {
	return Constant<0>{};
}

// Addition with common factors
template <typename F, typename G>
constexpr auto operator-(Addition<G, F> y, F) { return y.template get<0>(); }

template <typename F, typename G>
constexpr auto operator-(F, Addition<G, F> y) { return -y.template get<0>(); }

template <typename F, typename... Gs>
constexpr auto operator-(Addition<F, Gs...> y, F) { return y.template sub_sum<1, sizeof...(Gs) + 1>(); }

template <typename F, typename... Gs>
constexpr auto operator-(F, Addition<F, Gs...> y) { return -y.template sub_sum<1, sizeof...(Gs) + 1>(); }

template <typename F, typename... Gs>
constexpr auto operator-(Addition<F, Gs...> lhs, Addition<Gs...> rhs) { return lhs.template get<0>(); }

template <typename F, typename... Gs>
constexpr auto operator-(Addition<Gs...> lhs, Addition<F, Gs...> rhs) { return -rhs.template get<0>(); }

template <typename F, typename G, typename... Gs>
constexpr auto operator-(Addition<G, F> lhs, Addition<F, Gs...> rhs) { return lhs.template get<0>() - rhs.template sub_sum<1, sizeof...(Gs) + 1>(); }

template <typename F, typename G, typename... Gs>
constexpr auto operator-(Addition<F, Gs...> lhs, Addition<G, F> rhs) { return lhs.template sub_sum<1, sizeof...(Gs) + 1>() - rhs.template get<0>(); }

template <typename F, typename G, typename H>
constexpr auto operator-(Addition<G, F> lhs, Addition<H, F> rhs) { return lhs.template get<0>() - rhs.template get<0>(); }

// Subtractions with common factors
template <typename F, typename G>
constexpr auto operator-(Subtraction<G, F> y, F f) {
	using namespace literals;
	return y.lhs - 2_c * f;
}

template <typename F, typename G>
constexpr auto operator-(F f, Subtraction<G, F> y) {
	using namespace literals;
	return 2_c * f - y.lhs;
}

template <typename F, typename G>
constexpr auto operator-(Subtraction<F, G> y, F) { return -y.rhs; }

template <typename F, typename G>
constexpr auto operator-(F, Subtraction<F, G> y) { return y.rhs; }

// Subtractions with unrelated factors
template <typename F, typename G, typename H>
constexpr auto operator-(Subtraction<F, G> lhs, H rhs) { return lhs.lhs - (lhs.rhs + rhs); }

template <typename F, typename G, typename H>
constexpr auto operator-(H lhs, Subtraction<F, G> rhs) { return (lhs + rhs.rhs) - rhs.lhs; }

template <typename A, typename B, typename C, typename D>
constexpr auto operator-(Subtraction<A, B> lhs, Subtraction<C, D> rhs) { return (lhs.lhs + rhs.rhs) - (lhs.rhs + rhs.lhs); }

// Mulitplications with common factors
template <typename F, typename G>
constexpr auto operator-(Multiplication<G, F> y, F f) {
	using namespace literals;
	return (y.template get<0>() - 1_c) * f;
}

template <typename F, typename G>
constexpr auto operator-(F f, Multiplication<G, F> y) {
	using namespace literals;
	return (1_c - y.template get<0>()) * f;
}

template <typename F, typename... Gs>
constexpr auto operator-(Multiplication<F, Gs...> y, F f) {
	using namespace literals;
	return f * (y.template sub_product<1, sizeof...(Gs) + 1>() - 1_c);
}

template <typename F, typename... Gs>
constexpr auto operator-(F f, Multiplication<F, Gs...> y) {
	using namespace literals;
	return f * (1_c - y.template sub_product<1, sizeof...(Gs) + 1>());
}

template <typename F, typename... Gs>
constexpr auto operator-(Multiplication<Gs...> lhs, Multiplication<F, Gs...> rhs) {
	using namespace literals;
	return (1_c - rhs.template get<0>()) * lhs;
}

template <typename F, typename... Gs>
constexpr auto operator-(Multiplication<F, Gs...> lhs, Multiplication<Gs...> rhs) {
	using namespace literals;
	return (lhs.template get<0>() - 1_c) * rhs;
}

// Division with common factors
template <typename F, typename G>
constexpr auto operator-(Division<F, G> y, F f) {
	using namespace literals;
	return f * (1_c - y.rhs) / y.rhs;
}

template <typename F, typename G>
constexpr auto operator-(F f, Division<F, G> y) {
	using namespace literals;
	return f * (y.rhs - 1_c) / y.rhs;
}

// Division with unrelated factors
template <typename F, typename G, typename H>
constexpr auto operator-(Division<F, G> lhs, H rhs) { return (lhs.lhs - lhs.rhs * rhs) / lhs.rhs; }

template <typename F, typename G, typename H>
constexpr auto operator-(H lhs, Division<F, G> rhs) { return (lhs * rhs.rhs - rhs.lhs) / rhs.rhs; }

template <typename A, typename B, typename C, typename D>
constexpr auto operator-(Division<A, B> lhs, Division<C, D> rhs) { return (lhs.lhs * rhs.rhs - lhs.rhs * rhs.lhs) / (lhs.rhs * rhs.rhs); }

// Main Subtraction definition
template <typename F, typename G>
constexpr auto operator-(F lhs, G rhs) {
	static_assert(sizeof(lhs) > 0, "silence unused variable warning");
	static_assert(sizeof(rhs) > 0, "silence unused variable warning");

	if constexpr (std::is_same_v<F, Constant<0>>) {
		return -rhs;
	}
	else if constexpr (std::is_same_v<G, Constant<0>>) {
		return lhs;
	}
	else {
		return Subtraction<F, G>{lhs, rhs};
	}
}

}	// fields