#pragma once

#include "composite.hpp"
#include "multiply_scalar.hpp"


namespace fields {
namespace operators {

template <typename F, typename G>
class Addition : public Composite<F, G> {
public:
	Addition(F&& lhs, G&& rhs) : Composite<F, G>(std::forward<F>(lhs), std::forward<G>(rhs)) {
	}

	Addition(SharedField<F>&& lhs, G&& rhs) : Composite<F, G>(std::move(lhs), std::forward<G>(rhs)) {
	}

	Addition(F&& lhs, SharedField<G>&& rhs) : Composite<F, G>(std::forward<F>(lhs), std::move(rhs)) {
	}

	Addition(SharedField<F>&& f, SharedField<G>&& g) : Composite<F, G>(std::move(f), std::move(g)) {
	}

	template <typename T>
	auto operator() (T x) {
		// std::cout << "Addition operator()\n" << std::endl;	
		return lhs()(x) + rhs()(x);
	}

};



}	// operators
}	// fields