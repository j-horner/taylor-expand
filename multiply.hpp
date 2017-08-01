#pragma once

#include "composite.hpp"

namespace fields {
namespace operators {

template <typename F>
class MultiplyScalar;

template <typename F, typename G>
class MultiplyBase : public Composite<F, G> {
public:
	MultiplyBase(F&& lhs, G&& rhs) : Composite<F, G>(std::forward<F>(lhs), std::forward<G>(rhs)) {
	}

	MultiplyBase(SharedField<F>&& f, SharedField<G>&& g) : Composite<F, G>(std::move(f), std::move(g)) {
	}

	template <typename T>
	auto operator()(T x) {
		return lhs()(x)*rhs()(x);
	}
};

template <typename F, typename G>
class Multiply : public MultiplyBase<F, G> {
public:
	Multiply(F&& lhs, G&& rhs) : MultiplyBase<F, G>(std::forward<F>(lhs), std::forward<G>(rhs)) {
	}

	Multiply(SharedField<F>&& f, SharedField<G>&& g) : MultiplyBase<F, G>(std::move(f), std::move(g)) {
	}
};

/*template <typename F>
class Multiply<F, F> : public MultiplyBase<F, F> {
public:
	Multiply(SharedField<F> f, SharedField<F> g) : MultiplyBase<F, F>(std::move(f), std::move(g)) {
	}

	template <typename T>
	auto operator() (T x) {
		if (&lhs() == &rhs()) {
			const auto& f = lhs();
			auto val = f(x);		// underlying functions are the same, call once then square
			return val*val;
		} else {
			return MultiplyBase<F, F>::operator()(x);	// otherwise call default f*g
		}
	}
};*/


}	// operators

}	// fields
