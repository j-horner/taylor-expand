
#include "../addition.hpp"
#include "../subtraction.hpp"
#include "../multiplication.hpp"
#include "../vector.hpp"

#include "../functions/power.hpp"

namespace fields {

// Main Addition
template <typename F, typename G>
constexpr auto operator+(F lhs, G rhs) {
	static_assert(sizeof(lhs) > 0, "silence unused variable warning");
	static_assert(sizeof(rhs) > 0, "silence unused variable warning");

	if constexpr (std::is_same_v<F, fields::Constant<0>>) {
		return rhs;
	} else if constexpr (std::is_same_v<G, fields::Constant<0>>) {
		return lhs;
	} else {
		return Addition<F, G>(lhs, rhs);
	}
}


// Main Subtraction definition
template <typename F, typename G>
constexpr auto operator-(F lhs, G rhs) {
	static_assert(sizeof(lhs) > 0, "silence unused variable warning");
	static_assert(sizeof(rhs) > 0, "silence unused variable warning");

	if constexpr (std::is_same_v<F, Constant<0>>) {
		return -rhs;
	} else if constexpr (std::is_same_v<G, Constant<0>>) {
		return lhs;
	} else {
		return Subtraction<F, G>{lhs, rhs};
	}
}


// Main Multiplication
template <typename F, typename G>
constexpr auto operator*(F lhs, G rhs) {
	static_assert(sizeof(lhs) > 0, "silence unused variable warning");
	static_assert(sizeof(rhs) > 0, "silence unused variable warning");

	if constexpr (std::is_same_v<decltype(lhs), Constant<0>> || std::is_same_v<decltype(rhs), Constant<0>>) {
		return Constant<0>{};
	} else if constexpr (std::is_same_v<decltype(lhs), Constant<1>>) {
		return rhs;
	} else if constexpr (std::is_same_v<decltype(rhs), Constant<1>>) {
		return lhs;
	} else if constexpr (::fields::detail::is_constant<G>::value) {
		return rhs * lhs;
	} else {
		return Multiplication<F, G>(lhs, rhs);
	}
}


template <typename F, typename G>
constexpr auto operator/(F lhs, G rhs) {
	static_assert(false == std::is_same_v<decltype(rhs), Constant<0>>, "Cannot divide by 0!");

	static_assert(sizeof(lhs) > 0, "silence unused variable warning");
	static_assert(sizeof(rhs) > 0, "silence unused variable warning");

	if constexpr (std::is_same_v<decltype(lhs), Constant<0>>) {
		return Constant<0>{};
	} else {
		return lhs*Power<G, -1>{rhs};
	}
}





// -------------------------------------------------------------------------------------------------
//
//                                      + operations
//
// -------------------------------------------------------------------------------------------------



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
	} else {
		constexpr auto match_found = (util::is_same<Fs, G>::value || ...);

		if constexpr (false == match_found) {
			constexpr auto multiple_found = (fields::detail::is_multiple<Fs, G>::value || ...);

			if constexpr (false == multiple_found) {
				return Addition<Fs..., G>(f, g);
			} else {
				return f.template get<0>() + (f.template sub_sum<1, sizeof...(Fs)>() + g);
			}
		} else {
			return f.template get<0>() + (f.template sub_sum<1, sizeof...(Fs)>() + g);
		}
	}
}

template <typename... Fs, typename... Gs>
constexpr auto operator+(Addition<Fs...> lhs, Addition<Gs...> rhs) {
	if constexpr (util::is_same<Addition<Fs...>, Addition<Gs...>>::value) {
		return Constant<2>{}*lhs;
	} else {
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

template <typename F, typename... Gs>
constexpr auto operator+(Multiplication<F, Gs...> lhs, Multiplication<F, Gs...> rhs) {
	return (Constant<2>{}*lhs.template get<0>())* rhs.template sub_product<1, sizeof...(Gs) + 1>();
}

template <typename... Fs, typename... Gs>
constexpr auto operator+(Multiplication<Fs...> lhs, Multiplication<Gs...> rhs) -> std::enable_if_t<util::is_same<Multiplication<Fs...>, Multiplication<Gs...>>::value, Multiplication<Constant<2, 1>, Fs...>> {
	return Constant<2>{}*lhs;
}

template <typename F, typename G, typename... Fs, typename... Gs>
constexpr auto operator+(Multiplication<F, Fs...> lhs, Multiplication<G, Gs...> rhs)->std::enable_if_t < util::is_same<Multiplication<Fs...>, Multiplication<Gs...>>::value, Multiplication<decltype(F{} + G{}), Fs... >> {
	return (lhs.template get<0>() + rhs.template get<0>())* lhs.template sub_product<1, sizeof...(Fs) + 1>();
}

template <typename... Ts, typename... Us>
constexpr auto operator+(Vector<Ts...> v, Vector<Us...> u) {
	static_assert(sizeof...(Ts) == sizeof...(Us));
	return v.addition(u);
}








// -------------------------------------------------------------------------------------------------
//
//                                      - operations
//
// -------------------------------------------------------------------------------------------------




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







// -------------------------------------------------------------------------------------------------
//
//                                      * operations
//
// -------------------------------------------------------------------------------------------------

// Constant - any multiplications with Constant on the right will be moved so it's on the left, also ensure all constants are moved to the front through multiplications

// TODO - Multiplication<F, Constant> should never happen...
template <Int A, Int B, Int C, Int D, typename F>
constexpr auto operator*(Constant<A, B> c, Multiplication<F, Constant<C, D>> y) { return (c * y.template get<1>()) * y.template get<0>(); }

template <typename F, Int A, Int B, typename... Fs>
constexpr auto operator*(F f, Multiplication<Constant<A, B>, Fs...> y) {
	if constexpr (fields::detail::is_constant<F>::value) {
		return (f * y.template get<0>()) * y.template sub_product<1, sizeof...(Fs) + 1>();
	} else {
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
	} else {
		constexpr auto match_found = (std::is_same_v<Fs, G> || ...);

		if constexpr (false == match_found) {
			constexpr auto power_found = (fields::detail::is_power<Fs, G>::value || ...);

			if constexpr (false == power_found) {
				return Multiplication<Fs..., G>(f, g);
			} else {
				return f.template get<0>() * (f.template sub_product<1, sizeof...(Fs)>() * g);
			}
		} else {
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
	} else if constexpr (std::is_same_v<G, Constant<1>>) {
		return f;
	} else {
		constexpr auto match_found = (util::is_same<Fs, G>::value || ...);

		if constexpr (false == match_found) {
			constexpr auto power_found = (fields::detail::is_power<Fs, G>::value || ...);

			if constexpr (false == power_found) {
				return Multiplication<G, Fs...>(g, f);
			} else {
				return f.template get<0>() * (g * f.template sub_product<1, sizeof...(Fs)>());
			}
		} else {
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
	} else {
		return (lhs * rhs.template get<0>()) * rhs.template sub_product<1, sizeof...(Gs)>();
	}
}

template <typename F>
constexpr auto operator*(F lhs, F) {
	if constexpr (std::is_same_v<std::decay_t<decltype(lhs)>, Constant<0>>) {
		return Constant<0>{};
	} else {
		return Power<F, 2>{lhs};
	}
}

template <typename F, Int N>
constexpr auto operator*(Power<F, N> lhs, F) {
	if constexpr (N == -1) {
		using namespace literals;
		return 1_c;
	} else {
		return Power<F, N + 1>{lhs};
	}
}

template <typename F, Int N>
constexpr auto operator*(F, Power<F, N> rhs) {
	static_assert(sizeof(rhs) > 0, "silence unused variable warning");

	if constexpr (N == -1) {
		return Constant<1>{};
	} else {
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
	} else {
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
	} else {
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
	} else {
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
	} else {
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

template <typename F, typename... Fs>
constexpr auto operator*(F f, Vector<Fs...> v) {
	return v.multiply_scalar(f);
}

template <typename F, typename... Fs>
constexpr auto operator*(Vector<Fs...> v, F f) {
	return v.multiply_scalar(f);
}












// -------------------------------------------------------------------------------------------------
//
//                                      / operations
//
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

// division by constant is equivalent to multiplying by inverse
template <typename F, Int A, Int B>
constexpr auto operator/(F f, Constant<A, B> c) {
	return (Constant<1>{}/c)* f;
}




}	// fields