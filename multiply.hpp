#pragma once

#include "composite.hpp"

namespace fields {
namespace operators {

template <typename F>
class MultiplyScalar;

template <typename F, typename G>
class MultiplyBase : public Composite<F, G> {
public:
	template <typename Lhs, typename Rhs>
	MultiplyBase(Lhs&& lhs, Rhs&& rhs) : Composite<F, G>(std::forward<Lhs>(lhs), std::forward<Rhs>(rhs)) {
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
	Multiply(Lhs&& lhs, Rhs&& rhs) : MultiplyBase<F, G>(std::forward<Lhs>(lhs), std::forward<Rhs>(rhs)) {
	}
};

template <typename F>
class Multiply<F, F> : public MultiplyBase<F, F> {
public:
	Multiply(std::shared_ptr<F> f, std::shared_ptr<F> g) : MultiplyBase<F, F>(std::move(f), std::move(g)) {
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
};


}	// operators

}	// fields
