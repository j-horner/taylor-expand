#include "fields.hpp"

#include <iostream>

#include <cmath>

auto main() -> int {
	using namespace fields;
	
	auto psi_0 =	[a = 1.0, b = 2.0, c = 3.0](double x) {
						// std::cout << "Psi_0: " << x << std::endl;
						return a*x*x + b*x + c;
					};	
	
	auto H = [] (auto psi, auto /*t*/) {
		// std::cout << "----- Calling H -----" << std::endl;
		using namespace fields::operators;

		return psi*psi;
	};

	std::cout << "psi_0:\t" << psi_0(0.0) << std::endl;

	for (auto t = 0.0; t < 0.333; t += 0.01) {
		// auto t = 0.2;
		auto psi = integrate(H, psi_0, 0.0, t);

		std::cout << "t: " << t << "\tpsi:\t" << psi(0.0) << "\t" << psi_0(0.0) / (1.0 - t*psi_0(0.0)) << std::endl;
	}

	/*for (auto x = -1.0; x < 1.0; x += 0.1) {
		std::cout << "psi(" << x << ")\t= " << psi_0(x) << "\t--->\t" << psi(x) << "\t" << psi_0(x) / (1.0 - psi_0(x)) << std::endl;
	}*/
		







	return 0;
}