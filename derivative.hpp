#pragma once

#include "shared_field.hpp"

namespace fields {
namespace operators {

template <typename F, int N = 1>
class Derivative : public SharedField<F> {
public:
	template <typename G>
	Derivative(G&& g) : SharedField<F>(std::forward<G>(g)) {
	}

	template <typename T>
	auto operator()(T x) {
		constexpr static auto dx = 0.05;
		constexpr static auto dx_2 = 0.5 / dx;

		auto& f = func();

		return (f(x + dx) - f(x - dx))*dx_2;
	}
};


template <typename F>
auto d_dx(F&& f) {
	return Derivative<F>(std::forward<F>(f));
}

template <typename F>
auto d_dx(SharedField<F> f) {
	return Derivative<F>(std::move(f));
}


}	// operators
}	// fields