#pragma once

#include "shared_field.hpp"

#include <iostream>
#include <type_traits>

namespace fields {
namespace operators {

template <typename F>
class MultiplyScalar {
public:
	template <typename G>
	MultiplyScalar(G&& g, double k) : f_(std::forward<G>(g)), k_(k) {
	}

	MultiplyScalar(MultiplyScalar&& old, double k) : f_(std::move(old.f_)), k_(k*old.k_) {
	}

	template <typename T>
	auto operator()(T x) {
		return k_ * f_.func()(x);
	}

	auto scalar() const { return k_; }
	auto f() const { return f_; }

private:
	SharedField<F> f_;
	double k_;

};

}	// operators
}	// fields