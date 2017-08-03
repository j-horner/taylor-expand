#include "fields.hpp"

#include <iostream>
#include <chrono>

#include <cmath>

auto main() -> int {
	using namespace fields;
	
	/*auto psi_0 =	[a = 1.0, b = 2.0, c = 3.0](double x) {
						return a*x*x + b*x + c;
					};*/

	auto psi_0 =	[](double x) {
						return std::exp(x);
					};

	auto H = [] (auto psi, auto /*t*/) {
		using namespace fields::operators;

		// return psi;
		// return psi*psi;

		// return d_dx(d_dx(psi));

		return d_dx<2>(psi);
	};

	std::cout << "psi_0:\t" << psi_0(0.0) << std::endl;

	/*for (auto t = 0.0; t < 1.0; t += 0.01) {
		// auto t = 0.2;
		auto psi = integrate(H, psi_0, 0.0, t);

		auto x = 0.0;

		// std::cout << "t: " << t << "\tpsi:\t" << psi(x) << "\t" << psi_0(x) / (1.0 - t*psi_0(x)) << std::endl;
		std::cout << "t: " << t << "\tpsi:\t" << psi(x) << "\t" << psi_0(x)*std::exp(t) << std::endl;
	}*/

	auto t = 1.0;

	auto now = [] {return std::chrono::high_resolution_clock::now(); };

	auto start = now();

	auto psi = integrate(H, psi_0, 0.0, t);

	auto end = now();

	std::cout << "Time integration completed in: " << std::chrono::duration<double>(end - start).count() << "s" << std::endl;

	for (auto x = -50.0; x < 50.0; x += 0.1) {
		auto eval_start = now();
		
		auto psi_x = psi(x);

		auto eval_end = now();

		// std::cout << "psi(" << x << ")\t= " << psi_0(x) << "\t--->\t" << psi(x) << "\t" << psi_0(x) / (1.0 - psi_0(x)) << std::endl;
		std::cout << "psi(" << x << ")\t\t= " << psi_0(x) << "\t--->\t" << psi_x << "\t" << psi_0(x)*std::exp(t) << "\t" << std::chrono::duration<double>(eval_end - eval_start).count() << "s" << std::endl;
	}
	

	return 0;
}