#pragma once

#include "composite.hpp"
#include "multiply_scalar.hpp"


namespace fields {
namespace operators {

template <typename F, typename G>
class Addition : public Composite<F, G> {
public:
	template <typename Lhs, typename Rhs>
	Addition(Lhs&& lhs, Rhs&& rhs) : Composite<F, G>(std::forward<Lhs>(lhs), std::forward<Rhs>(rhs)) {
	}

	template <typename T>
	auto operator() (T x) {
		// std::cout << "Addition operator()\n" << std::endl;	
		return lhs()(x) + rhs()(x);
	}

};



}	// operators
}	// fields