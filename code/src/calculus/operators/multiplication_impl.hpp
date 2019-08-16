#pragma once

#include "../multiplication.hpp"
#include "../division.hpp"

#include "../functions/power.hpp"

namespace fields {

// Constant - any multiplications with Constant on the right will be moved so it's on the left, also ensure all constants are moved to the front through multiplications

// TODO - Multiplication<F, Constant> should never happen...
template <Int A, Int B, Int C, Int D, typename F>
constexpr auto operator*(Constant<A, B> c, Multiplication<F, Constant<C, D>> y) { return (c * y.template get<1>()) * y.template get<0>(); }

template <typename F, Int A, Int B, typename... Fs>
constexpr auto operator*(F f, Multiplication<Constant<A, B>, Fs...> y) {
	if constexpr (fields::detail::is_constant<F>::value) {
		return (f * y.template get<0>()) * y.template sub_product<1, sizeof...(Fs) + 1>();
	}
	else {
		return y.template get<0>() * (f * y.template sub_product<1, sizeof...(Fs) + 1>());
	}
}

template <Int A, Int B, typename... Fs>
constexpr auto operator*(Constant<A, B> f, Multiplication<Constant<A, B>, Fs...> y) {
	return (f * f) * y.template sub_product<1, sizeof...(Fs) + 1>();
}

template <typename... Fs, Int A, Int B, typename... Gs>
constexpr auto operator*(Multiplication<Fs...> lhs, Multiplication<Constant<A, B>, Gs...> rhs) { return rhs.template get<0>() * (lhs * rhs.template sub_product<1, sizeof...(Gs) + 1>()); }

// Multiplications with unrelated factors
// Need to remove the case where G is constant as all constants are moved to the left-hand-side
template <typename G, typename... Fs>
constexpr auto operator*(Multiplication<Fs...> f, G g) {
	if constexpr (fields::detail::is_constant<G>::value) {
		return g * f;
	}
	else {
		constexpr auto match_found = (std::is_same_v<Fs, G> || ...);

		if constexpr (false == match_found) {
			constexpr auto power_found = (fields::detail::is_power<Fs, G>::value || ...);

			if constexpr (false == power_found) {
				return Multiplication<Fs..., G>(f, g);
			}
			else {
				return f.template get<0>() * (f.template sub_product<1, sizeof...(Fs)>() * g);
			}
		}
		else {
			return f.template get<0>() * (f.template sub_product<1, sizeof...(Fs)>() * g);
		}
	}
}

template <typename... Fs, typename G>
constexpr auto operator*(G g, Multiplication<Fs...> f) {
	static_assert(sizeof(g) > 0, "silence unused variable warning");
	static_assert(sizeof(f) > 0, "silence unused variable warning");

	if constexpr (std::is_same_v<G, Constant<0>>) {
		return Constant<0>{};
	}
	else if constexpr (std::is_same_v<G, Constant<1>>) {
		return f;
	}
	else {
		constexpr auto match_found = (util::is_same<Fs, G>::value || ...);

		if constexpr (false == match_found) {
			constexpr auto power_found = (fields::detail::is_power<Fs, G>::value || ...);

			if constexpr (false == power_found) {
				return Multiplication<G, Fs...>(g, f);
			}
			else {
				return f.template get<0>() * (g * f.template sub_product<1, sizeof...(Fs)>());
			}
		}
		else {
			return f.template get<0>() * (g * f.template sub_product<1, sizeof...(Fs)>());
		}
	}
}

template <typename... Fs>
constexpr auto operator*(Multiplication<Fs...> lhs, Multiplication<Fs...>) {
	return Power<Multiplication<Fs...>, 2>{lhs};
}

template <typename... Fs, typename... Gs>
constexpr auto operator*(Multiplication<Fs...> lhs, Multiplication<Gs...> rhs) {
	if constexpr (util::is_same<Multiplication<Fs...>, Multiplication<Gs...>>::value) {
		return Power<Multiplication<Fs...>, 2>{lhs};
	}
	else {
		return (lhs * rhs.template get<0>()) * rhs.template sub_product<1, sizeof...(Gs)>();
	}
}

// Division with common factors
template <typename F, typename G, typename H>
constexpr auto operator*(Multiplication<G, F> m, Division<H, F> d) { return m.template get<0>() * d.lhs; }

template <typename F, typename G, typename H>
constexpr auto operator*(Division<H, F> d, Multiplication<G, F> m) { return d.lhs * m.template get<0>(); }

template <typename F, typename... Gs, typename H>
constexpr auto operator*(Multiplication<F, Gs...> m, Division<H, F> d) { return m.template sub_product<1, sizeof...(Gs) + 1>() * d.lhs; }

template <typename F, typename... Gs, typename H>
constexpr auto operator*(Division<H, F> d, Multiplication<F, Gs...> m) { return d.lhs * m.template sub_product<1, sizeof...(Gs) + 1>(); }

template <typename A, typename B, typename C>
constexpr auto operator*(Division<A, B> lhs, Division<B, C> rhs) { return lhs.lhs / rhs.rhs; }

template <typename A, typename B, typename C>
constexpr auto operator*(Division<A, B> lhs, Division<C, A> rhs) { return rhs.lhs / lhs.rhs; }

// Division with unrelated factors
template <typename A, typename B, typename C, typename D>
constexpr auto operator*(Division<A, B> lhs, Division<C, D> rhs) { return (lhs.lhs * rhs.lhs) / (lhs.rhs * rhs.rhs); }

template <typename... Fs, typename A, typename B>
constexpr auto operator*(Multiplication<Fs...> lhs, Division<A, B> rhs) { return (lhs * rhs.lhs) / rhs.rhs; }

template <typename F, typename G, typename H>
constexpr auto operator*(F f, Division<G, H> d) { return (f * d.lhs) / d.rhs; }

template <typename F, typename G, typename H>
constexpr auto operator*(Division<G, H> d, F f) { return (d.lhs * f) / d.rhs; }

template <typename F>
constexpr auto operator*(F lhs, F) {
	if constexpr (std::is_same_v<std::decay_t<decltype(lhs)>, Constant<0>>) {
		return Constant<0>{};
	}
	else {
		return Power<F, 2>{lhs};
	}
}

template <typename F, Int N>
constexpr auto operator*(Power<F, N> lhs, F) {
	if constexpr (N == -1) {
		using namespace literals;
		return 1_c;
	}
	else {
		return Power<F, N + 1>{lhs};
	}
}

template <typename F, Int N>
constexpr auto operator*(F, Power<F, N> rhs) {
	static_assert(sizeof(rhs) > 0, "silence unused variable warning");

	if constexpr (N == -1) {
		return Constant<1>{};
	}
	else {
		return Power<F, N + 1>{rhs};
	}
}

template <typename F, Int N>
constexpr auto operator*(Power<F, N> lhs, Power<F, N>) {
	return Power<F, 2*N>{lhs};
}

template <typename F, Int N, Int M>
constexpr auto operator*(Power<F, N> lhs, Power<F, M>) -> std::enable_if_t<(N != M) && (N + M != 0), Power<F, N + M>> {
	return Power<F, N + M>{lhs};
}



template <typename F, typename G>
constexpr auto operator*(Multiplication<F, G> lhs, G rhs) {
	if constexpr (std::is_same_v<std::decay_t<decltype(rhs)>, Constant<0>>) {
		return Constant<0>{};
	}
	else {
		return lhs.template get<0>() * Power<G, 2>{rhs};
	}
}

template <typename F, typename... Gs>
constexpr auto operator*(F lhs, Multiplication<F, Gs...> rhs) {
	return Power<F, 2>{lhs}*rhs.template sub_product<1, sizeof...(Gs) + 1>();
}

template <typename F, typename G, Int N>
constexpr auto operator*(Multiplication<F, G> lhs, Power<G, N> rhs) {
	if constexpr (-1 == N) {
		return lhs.template get<0>();
	}
	else {
		return lhs.template get<0>() * Power<G, N + 1>{rhs};
	}
}

template <typename F, typename... Gs, Int N>
constexpr auto operator*(Power<F, N> lhs, Multiplication<F, Gs...> rhs) {
	return Power<F, N + 1>{lhs}*rhs.template sub_product<1, sizeof...(Gs) + 1>();
}

template <typename F, typename G, Int N>
constexpr auto operator*(Multiplication<F, Power<G, N>> lhs, G rhs) {
	if constexpr (N == -1) {
		return lhs.template get<0>();
	}
	else {
		return lhs.template get<0>() * Power<G, N + 1>{rhs};
	}
}

template <typename F, typename... Gs, Int N>
constexpr auto operator*(F lhs, Multiplication<Power<F, N>, Gs...> rhs) {
	return Power<F, N + 1>{lhs}*rhs.template sub_product<1, sizeof...(Gs) + 1>();
}

template <typename F, typename G, Int N, Int M>
constexpr auto operator*(Multiplication<F, Power<G, N>> lhs, Power<G, M> rhs) {
	if constexpr (N + M == 1) {
		return lhs.template get<0>() * rhs.f();
	}
	else {
		return lhs.template get<0>() * Power<G, N + M>{rhs};
	}
}

template <typename F, typename... Gs, Int N, Int M>
constexpr auto operator*(Power<F, N> lhs, Multiplication<Power<F, M>, Gs...> rhs) {
	return Power<F, N + M>{lhs}*rhs.template sub_product<1, sizeof...(Gs) + 1>();
}

template <typename F, typename G, Int N>
constexpr auto operator*(Multiplication<F, Power<G, N>> lhs, Power<G, N> rhs) {
	return lhs.template get<0>() * Power<G, 2 * N>{rhs};
}

template <typename F, typename... Gs, Int N>
constexpr auto operator*(Power<F, N> lhs, Multiplication<Power<F, N>, Gs...> rhs) {
	return Power<F, 2 * N>{lhs}*rhs.template sub_product<1, sizeof...(Gs) + 1>();
}

template <Int A, Int B, Int C, Int D, Int N, typename... Fs>
constexpr auto operator*(Constant<A, B> lhs, Power<Multiplication<Constant<C, D>, Fs...>, N> rhs) {
	constexpr auto n = Constant<N>{};
	return (lhs * (rhs.f().template get<0>() ^ n)) * (rhs.f().template sub_product<1, sizeof...(Fs) + 1>() ^ n);
}

// Main Multiplication
template <typename F, typename G>
constexpr auto operator*(F lhs, G rhs) {
	static_assert(sizeof(lhs) > 0, "silence unused variable warning");
	static_assert(sizeof(rhs) > 0, "silence unused variable warning");

	if constexpr (std::is_same_v<decltype(lhs), Constant<0>> || std::is_same_v<decltype(rhs), Constant<0>>) {
		return Constant<0>{};
	}
	else if constexpr (std::is_same_v<decltype(lhs), Constant<1>>) {
		return rhs;
	}
	else if constexpr (std::is_same_v<decltype(rhs), Constant<1>>) {
		return lhs;
	}
	else if constexpr (::fields::detail::is_constant<G>::value) {
		return rhs * lhs;
	}
	else {
		return Multiplication<F, G>(lhs, rhs);
	}
}

}	// fields