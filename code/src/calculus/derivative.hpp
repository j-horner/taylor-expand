#pragma once

#include "addition.hpp"
#include "multiply_scalar.hpp"
#include "shared_field.hpp"

#include "../util/util.hpp"

namespace fields {
namespace operators {

template <typename F, int N = 1>
class Derivative {
public:

	// Derivative(const Derivative&) = default;

	template <typename G>
	explicit Derivative(G&& g) : f_(std::forward<G>(g)) {
	}

	auto operator()(double x) const {
		static_assert(util::binomial(N, 0) == 1, "Binomial(N, 0) is broken!");
		static_assert(util::binomial(N, 1) == N, "Binomial(N, 1) is broken!");

		// auto& f = f_.func();
		// return dx_2*(f(x + dx) - f(x - dx));

		constexpr auto factor = util::pow(dx_2, N);
		return factor*sum_terms<N>(x);
	}

	auto f() const { return f_; }

private:
	template <int K>
	auto sum_terms(double x) const {
		return term<K>(x) + sum_terms<K - 1>(x);
	}

	template <>
	auto sum_terms<0>(double x) const {
		return term<0>(x);
	}

	template <int K>
	auto term(double x) const {
		constexpr auto h = (N - 2 * K)*dx;
		constexpr auto coeff = util::pow(-1, K)*util::binomial(N, K);
		return coeff*f_.func()(x + h);
	}

	SharedField<F> f_;

	constexpr static auto dx = 0.1;
	constexpr static auto dx_2 = 0.5 / dx;
};


template <typename F>
auto d_dx(F&& f) {
	return Derivative<F>(std::forward<F>(f));
}

template <int N, typename F>
auto d_dx(F&& f) {
	return Derivative<F, N>(std::forward<F>(f));
}

template <typename F>
auto d_dx(SharedField<F> f) {
	return Derivative<F>(std::move(f));
}

template <int N, typename F>
auto d_dx(SharedField<F> f) {
	return Derivative<F, N>(std::move(f));	// F = lambda
}

template <typename F, int M>
auto d_dx(Derivative<F, M> dydx) {		// F = lambda, M = 1
	return d_dx<M + 1>(dydx.f());		// d_dx<2>(SharedField<lambda>)
}

template <int N, typename F, int M>
auto d_dx(Derivative<F, M> dydx) {
	return d_dx<M + N>(dydx.f());
}

template <int N, typename F, int M>
auto d_dx(SharedField<Derivative<F, M>> dydx) {
	return d_dx<M + N>(dydx.func().f());
}

template <int N, typename F, typename G>
auto d_dx(Addition<F, G> y) {
	return Derivative<F, N>(y.f()) + Derivative<G, N>(y.g());
}

template <int N, typename F, typename G>
auto d_dx(SharedField<Addition<F, G>> y) {
	return d_dx<N>(y.func().f()) + d_dx<N>(y.func().g());
	// return Derivative<F, N>(y.func().f()) + Derivative<G, N>(y.func().g());
}

template <int N, typename F>
auto d_dx(MultiplyScalar<F> y) {
	return y.scalar()*d_dx<N>(y.f());
}

template <int N, typename F>
auto d_dx(SharedField<MultiplyScalar<F>> y) {
	return y.func().scalar()*d_dx<N>(y.func().f());
}

}	// operators
}	// fields