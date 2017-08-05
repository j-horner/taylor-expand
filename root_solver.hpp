#pragma once

#include "derivative.hpp"
#include "shared_field.hpp"

namespace fields {
namespace util {

template <typename F>
auto find_root(F&& f) {
	using namespace operators;
	
	// return 0.0;

	auto df_dx = d_dx(make_shared_field(f));

	auto x_0 = 0.0;
	
	auto x_1 = x_0 - f(x_0)/df_dx(x_0);

	for (auto i = 0; i < 10; ++i) {

		const auto dx = (x_1 - x_0)/x_1;

		if (dx*dx < 1.0e-2) {
			break;
		}

		x_0 = x_1;

		x_1 = x_0 - f(x_0)/df_dx(x_0);

		std::cout << i << "\t" << x_1 << "\t" << f(x_0) << std::endl;
	}

	return x_1;

}


}	// util
}	// fields