#pragma once

#include "../addition.hpp"
#include "../subtraction.hpp"
#include "../multiplication.hpp"
#include "../division.hpp"


namespace fields {

// Definition of number 2
template <typename F>
constexpr auto operator+(F rhs, F) {
	return Constant<2>{}*rhs;
}

// Addiions with common factors
template <typename F, typename G>
constexpr auto operator+(Addition<G, F> y, F f) {
	return y.template get<0>() + Constant<2>{}*f;
}

template <typename F, typename G>
constexpr auto operator+(F f, Addition<G, F> y) {
	return y.template get<0>() + Constant<2>{}*f;
}

template <typename F, typename... Gs>
constexpr auto operator+(Addition<F, Gs...> y, F f) {
	return y.template sub_sum<1, sizeof...(Gs) + 1>() + Constant<2>{}*f;
}

template <typename F, typename... Gs>
constexpr auto operator+(F f, Addition<F, Gs...> y) {
	return y.template sub_sum<1, sizeof...(Gs) + 1>() + Constant<2>{}*f;
}

// Additions with unrelated factors
template <typename... Fs, typename G>
constexpr auto operator+(G g, Addition<Fs...> f) {
	return f + g;
}

template <typename G, typename... Fs>
constexpr auto operator+(Addition<Fs...> f, G g) {
	static_assert(sizeof(g) > 0, "silence unused variable warning");

	if constexpr (std::is_same_v<G, Constant<0>>) {
		return f;
	}
	else {
		constexpr auto match_found = (util::is_same<Fs, G>::value || ...);

		if constexpr (false == match_found) {
			constexpr auto multiple_found = (fields::detail::is_multiple<Fs, G>::value || ...);

			if constexpr (false == multiple_found) {
				return Addition<Fs..., G>(f, g);
			}
			else {
				return f.template get<0>() + (f.template sub_sum<1, sizeof...(Fs)>() + g);
			}
		}
		else {
			return f.template get<0>() + (f.template sub_sum<1, sizeof...(Fs)>() + g);
		}
	}
}

template <typename... Fs, typename... Gs>
constexpr auto operator+(Addition<Fs...> lhs, Addition<Gs...> rhs) {
	if constexpr (util::is_same<Addition<Fs...>, Addition<Gs...>>::value) {
		return Constant<2>{}*lhs;
	}
	else {
		return (lhs + rhs.template get<0>()) + rhs.template sub_sum<1, sizeof...(Gs)>();
	}
}

// Subtractions with common factors
template <typename F, typename G>
constexpr auto operator+(Subtraction<G, F> y, F) { return y.lhs; }

template <typename F, typename G>
constexpr auto operator+(F f, Subtraction<G, F> y) { return y.lhs; }

template <typename F, typename G>
constexpr auto operator+(Subtraction<F, G> y, F f) {
	using namespace literals;
	return 2_c * f - y.rhs;
}

template <typename F, typename G>
constexpr auto operator+(F f, Subtraction<F, G> y) {
	using namespace literals;
	return 2_c * f - y.rhs;
}

template <typename F, typename G, typename H>
constexpr auto operator+(Addition<G, F> lhs, Subtraction<H, F> rhs) { return lhs.template get<0>() + rhs.lhs; }

template <typename F, typename G, typename H>
constexpr auto operator+(Subtraction<H, F> lhs, Addition<G, F> rhs) { return lhs.lhs + rhs.template get<0>(); }

template <typename F, typename G, typename... Gs>
constexpr auto operator+(Addition<F, Gs...> lhs, Subtraction<G, F> rhs) { return lhs.template sub_sum<1, sizeof...(Gs) + 1>() + rhs.lhs; }

template <typename F, typename G, typename... Gs>
constexpr auto operator+(Subtraction<G, F> lhs, Addition<F, Gs...> rhs) { return lhs.lhs + rhs.template sub_sum<1, sizeof...(Gs) + 1>(); }

template <typename F, typename G, typename H>
constexpr auto operator+(Subtraction<G, F> lhs, Subtraction<F, H> rhs) { return lhs.lhs - rhs.rhs; }

template <typename F, typename G, typename H>
constexpr auto operator+(Subtraction<F, G> lhs, Subtraction<H, F> rhs) { return rhs.lhs - lhs.rhs; }

// Subtractions with unrelated factors
template <typename F, typename G, typename H>
constexpr auto operator+(Subtraction<F, G> lhs, H rhs) { return (lhs.lhs + rhs) - lhs.rhs; }

template <typename F, typename G, typename H>
constexpr auto operator+(H lhs, Subtraction<F, G> rhs) { return (lhs + rhs.lhs) - rhs.rhs; }

template <typename A, typename B, typename C, typename D>
constexpr auto operator+(Subtraction<A, B> lhs, Subtraction<C, D> rhs) { return (lhs.lhs + rhs.lhs) - (lhs.rhs + rhs.rhs); }

// Multiplication with common factors
template <typename F, typename G>
constexpr auto operator+(Multiplication<G, F> y, F f) {
	using namespace literals;
	return (y.template get<0>() + 1_c) * f;
}

template <typename F, typename G>
constexpr auto operator+(F f, Multiplication<G, F> y) {
	using namespace literals;
	return (1_c + y.template get<0>()) * f;
}

template <typename F, typename... Gs>
constexpr auto operator+(Multiplication<F, Gs...> y, F f) {
	using namespace literals;
	return f * (y.template sub_product<1, sizeof...(Gs) + 1>() + 1_c);
}

template <typename F, typename... Gs>
constexpr auto operator+(F f, Multiplication<F, Gs...> y) {
	using namespace literals;
	return f * (1_c + y.template sub_product<1, sizeof...(Gs) + 1>());
}

template <typename F, typename... Gs>
constexpr auto operator+(Multiplication<Gs...> lhs, Multiplication<F, Gs...> rhs) {
	using namespace literals;
	return (1_c + rhs.template get<0>()) * lhs;
}

template <typename F, typename... Gs>
constexpr auto operator+(Multiplication<F, Gs...> lhs, Multiplication<Gs...> rhs) {
	using namespace literals;
	return (lhs.template get<0>() + 1_c) * rhs;
}

/*template <typename F, typename G, typename... Gs>
constexpr auto operator+(Multiplication<F, Gs...> lhs, Multiplication<G, Gs...> rhs) {
	return (lhs.template get<0>() + rhs.template get<0>()) * lhs.template sub_product<1, sizeof...(Gs) + 1>();
}*/

template <typename F, typename... Gs>
constexpr auto operator+(Multiplication<F, Gs...> lhs, Multiplication<F, Gs...> rhs) {
	return (Constant<2>{}*lhs.template get<0>())* rhs.template sub_product<1, sizeof...(Gs) + 1>();
}

template <typename... Fs, typename... Gs>
constexpr auto operator+(Multiplication<Fs...> lhs, Multiplication<Gs...> rhs) -> std::enable_if_t<util::is_same<Multiplication<Fs...>, Multiplication<Gs...>>::value, Multiplication<Constant<2, 1>, Fs...>> {
	return Constant<2>{}*lhs;
}

/*template <Int A, Int B, Int C, Int D, typename... Gs>
constexpr auto operator+(Multiplication<Constant<A, B>, Gs...> lhs, Multiplication<Constant<C, D>, Gs...> rhs) {
	return (lhs.template get<0>() + rhs.template get<0>()) * lhs.template sub_product<1, sizeof...(Gs) + 1>();
}*/

/*template <Int A, Int B, Int C, Int D, typename... Fs, typename... Gs>
constexpr auto operator+(Multiplication<Constant<A, B>, Fs...> lhs, Multiplication<Constant<C, D>, Gs...>)->std::enable_if_t < util::is_same<Multiplication<Fs...>, Multiplication<Gs...>>::value, Multiplication<decltype(Constant<A, B>{} +Constant<C, D>{}), Fs... >> {
	return (Constant<A, B>{} + Constant<C, D>{})* lhs.template sub_product<1, sizeof...(Fs) + 1>();
}*/

template <typename F, typename G, typename... Fs, typename... Gs>
constexpr auto operator+(Multiplication<F, Fs...> lhs, Multiplication<G, Gs...> rhs)->std::enable_if_t < util::is_same<Multiplication<Fs...>, Multiplication<Gs...>>::value, Multiplication<decltype(F{} + G{}), Fs... >> {
	return (lhs.template get<0>() + rhs.template get<0>())* lhs.template sub_product<1, sizeof...(Fs) + 1>();
}

// Division with common factors
template <typename F, typename G>
constexpr auto operator+(Division<F, G> y, F f) {
	using namespace literals;
	return f * (1_c + y.rhs) / y.rhs;
}

template <typename F, typename G>
constexpr auto operator+(F f, Division<F, G> y) {
	using namespace literals;
	return f * (y.rhs + 1_c) / y.rhs;
}

// Division with unrelated factors
// template <typename F, typename G, typename H>
// constexpr auto operator+(Division<F, G> lhs, H rhs) { return (lhs.lhs + lhs.rhs*rhs)/lhs.rhs; }

// template <typename F, typename G, typename H>
// constexpr auto operator+(H lhs, Division<F, G> rhs) { return (lhs*rhs.rhs + rhs.lhs)/rhs.rhs; }

template <typename A, typename B, typename C, typename D>
constexpr auto operator+(Division<A, B> lhs, Division<C, D> rhs) { return (lhs.lhs * rhs.rhs + lhs.rhs * rhs.lhs) / (lhs.rhs * rhs.rhs); }

// Main Addition
template <typename F, typename G>
constexpr auto operator+(F lhs, G rhs) {
	static_assert(sizeof(lhs) > 0, "silence unused variable warning");
	static_assert(sizeof(rhs) > 0, "silence unused variable warning");

	if constexpr (std::is_same_v<F, fields::Constant<0>>) {
		return rhs;
	}
	else if constexpr (std::is_same_v<G, fields::Constant<0>>) {
		return lhs;
	}
	else {
		return Addition<F, G>(lhs, rhs);
	}
}

}	// fields