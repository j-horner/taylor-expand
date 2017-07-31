#pragma once

#include <iostream>
#include <memory>
#include <type_traits>

namespace fields {
namespace operators {

template <typename F>
class MultiplyScalar {
public:
	MultiplyScalar(F&& f, double k) : f_(std::make_shared<F>(std::forward<F>(f))), k_(k) {
	}

	MultiplyScalar(std::shared_ptr<F> f, double k) : f_(std::move(f)), k_(k) {
	}

	MultiplyScalar(MultiplyScalar&& old, double k) : f_(std::move(old.f_)), k_(k*old.k_) {
		// std::cout << "Merged two scalar multiplications:\t" << old.k_ << "\t" << k << "--->\t" << k_ << std::endl;
	}

	template <typename T>
	auto operator()(T x) {
		// std::cout << "MultiplyScalar operator()\n" << std::endl;
		return k_ * (*f_)(x);
	}

	auto lhs() const -> decltype(auto) { return *f_;}
	auto rhs() const { return k_; }

	auto f() const { return f_; }

private:
	std::shared_ptr<F> f_;
	double k_;

};

}	// operators
}	// fields