#pragma once

#include "shared_field.hpp"

namespace fields {
namespace operators {

template <typename F>
class Derivative {
public:
	template <typename G>
	Derivative(G&& g) : f_(std::forward<G>(g)) {
	}

	template <typename T>
	auto operator()(T x) {
		constexpr static auto dx = 0.001;
		constexpr static auto dx_2 = 0.5 / dx;

		const auto& f = f_.f();

		return (f(x + dx) - f(x - dx))*dx_2;
	}

private:
	SharedField<F> f_;
};


template <typename F>
auto d_dx(const F&& f) {
	return Derivative<F>(std::forward<F>(f));
}

}
}	// fields