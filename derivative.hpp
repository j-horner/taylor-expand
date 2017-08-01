#pragma once

#include "shared_field.hpp"

namespace fields {
namespace operators {

template <typename F>
class Derivative : public SharedField<F> {
public:
	template <typename G>
	Derivative(G&& g) : SharedField<F>(std::forward<G>(g)) {
	}

	template <typename T>
	auto operator()(T x) {
		constexpr static auto dx = 0.001;
		constexpr static auto dx_2 = 0.5 / dx;

		const auto& f = func();

		return (f(x + dx) - f(x - dx))*dx_2;
	}
};


template <typename F>
auto d_dx(const F&& f) {
	return Derivative<F>(std::forward<F>(f));
}

}	// operators
}	// fields