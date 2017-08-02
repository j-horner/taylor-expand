#pragma once

#include "shared_field.hpp"

#include <iostream>
#include <type_traits>

namespace fields {
namespace operators {

template <typename F>
class MultiplyScalar : public SharedField<F> {
public:
	template <typename G>
	MultiplyScalar(G&& g, double k) : SharedField<F>(std::forward<G>(g)), k_(k) {
	}

	MultiplyScalar(MultiplyScalar&& old, double k) : SharedField<F>(std::move(old.f_)), k_(k*old.k_) {
	}

	template <typename T>
	auto operator()(T x) {
		return k_ * func()(x);
	}

	auto scalar() const { return k_; }

private:
	double k_;

};

}	// operators
}	// fields