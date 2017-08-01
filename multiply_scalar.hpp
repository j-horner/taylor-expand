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
	MultiplyScalar(G&& f, double k) : SharedField<F>(std::forward<G>(f)), k_(k) {
	}

	/*MultiplyScalar(std::shared_ptr<F> f, double k) : f_(std::move(f)), k_(k) {
	}*/

	MultiplyScalar(MultiplyScalar&& old, double k) : SharedField<F>(std::move(old.f_)), k_(k*old.k_) {
		// std::cout << "Merged two scalar multiplications:\t" << old.k_ << "\t" << k << "--->\t" << k_ << std::endl;
	}

	template <typename T>
	auto operator()(T x) {
		// std::cout << "MultiplyScalar operator()\n" << std::endl;
		return k_ * func()(x);
	}

	auto scalar() const { return k_; }

	/*auto lhs() const -> decltype(auto) { return *f_;}
	auto rhs() const { return k_; }

	auto f() const { return f_; }*/

private:
	double k_;

};

}	// operators
}	// fields