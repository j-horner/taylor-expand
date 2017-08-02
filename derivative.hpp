#pragma once

#include "shared_field.hpp"
#include "util.hpp"

namespace fields {
namespace operators {

template <typename F, int N = 1>
class Derivative : public SharedField<F> {
public:
	template <typename G>
	Derivative(G&& g) : SharedField<F>(std::forward<G>(g)) {
	}

	auto operator()(double x) {
		static_assert(util::pow(2, 3) == 8, "Pow is broken!");
		static_assert(util::factorial(4) == 24, "Factorial is broken!");
		static_assert(util::binomial(4, 2) == 6, "Binomial(4, 2) is broken!");
		static_assert(util::binomial(N, 0) == 1, "Binomial(N, 0) is broken!");
		static_assert(util::binomial(N, 1) == N, "Binomial(N, 1) is broken!");

		// auto& f = func();
		// return dx_2*(f(x + dx) - f(x - dx));
	
		constexpr auto factor = util::pow(dx_2, N);
		return factor*sum_terms<N>(x);
	}
private:
	template <int K>
	auto sum_terms(double x) {
		return term<K>(x) + sum_terms<K - 1>(x);
	}

	template <>
	auto sum_terms<0>(double x) {
		return term<0>(x);
	}

	template <int K>
	auto term(double x) {
		constexpr auto h = (N - 2 * K)*dx;
		constexpr auto coeff = util::pow(-1, K)*util::binomial(N, K);
		return coeff*func()(x + h);
	}

	constexpr static auto dx = 0.05;
	constexpr static auto dx_2 = 0.5 / dx;
};


template <typename F>
auto d_dx(F&& f) {
	return Derivative<F>(std::forward<F>(f));
}

template <typename F>
auto d_dx(SharedField<F> f) {
	return Derivative<F>(std::move(f));
}


}	// operators
}	// fields