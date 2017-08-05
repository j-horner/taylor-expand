#pragma once

#include "derivative.hpp"
#include "shared_field.hpp"

namespace fields {
namespace util {

template <typename F>
auto find_roots(F&& f) {
	using namespace operators;
	
	auto df_dx = d_dx(f);
	
	const auto find_root = [&f, &df_dx] (auto x) {
	
		auto x_1 = x - f(x) / df_dx(x);

		for (auto i = 0; i < 30; ++i) {
			std::cout << i << "\t" << x_1 << "\t" << x << "\t" << f(x_1) << "\t" << f(x) << std::endl;

			const auto dx = (x_1 - x) / x_1;

			// check for convergence in x
			/*if (dx*dx < 1.0e-12) {
				break;
			}*/

			x = x_1;

			auto f_x = f(x);

			x_1 = x - f_x / df_dx(x);

			// check for convergence in f, (could be that f -> 0 as x -> inf)
			if (std::abs(f_x) < 1.0e-12) {
				break;
			}
		}

		return x_1;
	};

	return std::array<double, 5>{find_root(-10.0), find_root(-5.0), find_root(0.0), find_root(5.0), find_root(10.0)};

}


}	// util
}	// fields