#pragma once

#include "composite.hpp"
#include "addition.hpp"

namespace fields {
namespace operators {

/*template <typename F>
class MultiplyScalar;

template <typename F, typename G>
class MultiplyBase : public Composite<F, G> {
public:
	template <typename Lhs, typename Rhs>
	MultiplyBase(Lhs&& l, Rhs&& r) : Composite<F, G>(std::forward<Lhs>(l), std::forward<Rhs>(r)) {
	}

	template <typename T>
	auto operator()(T x) {
		return lhs()(x)*rhs()(x);
	}
};

template <typename F, typename G>
class Multiply : public MultiplyBase<F, G> {
public:
	template <typename Lhs, typename Rhs>
	Multiply(Lhs&& l, Rhs&& r) : MultiplyBase<F, G>(std::forward<Lhs>(l), std::forward<Rhs>(r)) {
	}
};

template <typename F>
class Multiply<F, F> : public MultiplyBase<F, F> {
public:
	Multiply(SharedField<F> f, SharedField<F> g) : MultiplyBase<F, F>(std::move(f), std::move(g)) {
	}

	template <typename T>
	auto operator() (T x) {
		if (&lhs() == &rhs()) {
			auto val = lhs()(x);		// underlying functions are the same, call once then square
			return val*val;
		} else {
			return MultiplyBase<F, F>::operator()(x);	// otherwise call default f*g
		}
	}
};*/

template <typename F, typename G>
class Multiplication {
public:
    template <typename T>
    constexpr auto operator()(T x) const { return lhs(x)*rhs(x); }

    F lhs;
    G rhs;
};

template <typename F, typename G>
class Division {
public:
    template <typename T>
    constexpr auto operator()(T x) const { return lhs(x)/rhs(x); }

    F lhs;
    G rhs;
};

template <typename F, typename G>
constexpr auto operator*(F lhs, G rhs) { return Multiplication<F, G>{lhs, rhs}; }

template <typename F, typename G>
constexpr auto operator/(F lhs, G rhs) { return Division<F, G>{lhs, rhs}; }

template <typename A, typename B, typename C, typename D>
constexpr auto operator==(Multiplication<A, B>, Multiplication<C, D>) { return (std::is_same_v<A, C> && std::is_same_v<B, D>) || (std::is_same_v<A, D> && std::is_same_v<B, C>); }

template <typename F, typename G>
constexpr auto d_dx(Multiplication<F, G> y) { return d_dx(y.lhs)*y.rhs + y.lhs*d_dx(y.rhs); }

template <typename F, typename G>
constexpr auto d_dx(Division<F, G> y) { return d_dx(y.lhs)/y.rhs - y.lhs*d_dx(y.rhs)/(y.rhs*y.rhs); }

}	// operators

}	// fields
