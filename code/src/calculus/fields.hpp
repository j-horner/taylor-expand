#pragma once

#include "../util/function_traits.hpp"
#include "operators.hpp"

#include <array>
#include <functional>
#include <iostream>
#include <list>
#include <memory>
#include <unordered_map>

#include <cmath>

namespace fields {

template <typename T>
class Field {
 public:
	 template <typename F>
	 explicit Field(F&& f) : f_(std::forward<F>(f)) {
	 }

	template <typename X>
	auto operator()(X&& x) const {
		const auto idx = static_cast<int>(x*dx_1);

		const auto location = memory_.find(idx);

		if (memory_.end() != location) {
			return location->second;
		} else {
			const auto val = f_(std::forward<X>(x));

			memory_.emplace(idx, val);

			return val;
		}
	 }

 private:
	std::function<T> f_;

	mutable std::unordered_map<int, double> memory_;

	constexpr static auto dx_1 = 1.0e7;		// inverse of the dynamic lattice resolution
};


template <typename T>
class FieldList {
 public:
	 template <typename... Args>
	 auto operator()(Args&&... args) const {
		 return front().func()(std::forward<Args>(args)...);
	 }

	 auto front() const -> decltype(auto) {
		 return field_.front();
	 }

	 template <typename... Args>
	 auto emplace_front(Args&&... args) {
		 field_.emplace_front(SharedField<Field<T>>(Field<T>{std::forward<Args>(args)...}));
	 }

 private:
	 template <typename G>
	 using SharedField = operators::SharedField<G>;

	 std::list<SharedField<Field<T>>> field_;
};

namespace {
using namespace operators;

template <typename F>
auto wrap_function(F&& f) {
	return SharedField<F>(std::forward<F>(f));
}

template <typename F>
auto wrap_function(SharedField<F> f) {
	return f;
}
}

template <typename Hamiltonian,
		  typename Psi,
		  typename Real>
auto integrate(Hamiltonian&& H_, Psi&& psi_init, Real t_0, Real t_f) {
	using traits = util::function_traits<std::decay_t<Psi>>;

	using Ret = traits::result_type;
	using Arg = traits::arg<0>::type;

	auto psi = FieldList<Ret(Arg)>{};

	auto dt = Real{ 0.025 };
	constexpr auto sixth = Real{ 1.0 / 6.0 };

	auto t = t_0;

	auto H = [&H_] (auto&& psi, double t) {
		return H_(wrap_function(std::forward<decltype(psi)>(psi)), t);
	};

	psi.emplace_front(std::forward<Psi>(psi_init));

	constexpr auto tol = 1.0e-6;

	auto RKCK_step = [&H] (auto& psi_0, auto t, auto dt) {
		using namespace operators;

		// RKCK coefficients	https://en.wikipedia.org/wiki/Cash%E2%80%93Karp_method
		constexpr static auto c = std::array<Real, 5>{0.2, 0.3, 1.0, 0.875};

		constexpr static auto b = std::array<Real, 6>{(37.0 / 378.0), 0.0, (250.0 / 621.0), (125.0 / 594.0), 0.0, (512.0 / 1771.0)};

		constexpr static auto e = std::array<Real, 6>{	std::get<0>(b) - (2825.0 / 27648.0),
			std::get<1>(b) - 0.0,
			std::get<2>(b) - (18575.0 / 48384.0),
			std::get<3>(b) - (13525.0 / 55296.0),
			std::get<4>(b) - (277.0 / 14336.0),
			std::get<5>(b) - 0.25};

		constexpr static auto a = std::array<std::array<Real, 5>, 5> { { { 0.2, 0.0, 0.0, 0.0, 0.0 },
		{ (3.0 / 40.0), (9.0 / 40.0), 0.0, 0.0, 0.0 },
		{ 0.3, -0.9, 1.2, 0.0, 0.0 },
		{ (-11.0 / 54.0), 2.5, (-70.0 / 27.0), (35.0 / 27.0), 0.0 },
		{ (1631.0 / 55296.0), (175.0 / 512.0), (575.0 / 13824.0), (44275.0 / 110592.0), (253.0 / 4096.0) }}};

		// RKCK
		auto K1 = H(psi_0, t)*dt;
		auto K2 = H(psi_0 + a[0][0] * K1, t + c[0] * dt)*dt;
		auto K3 = H(psi_0 + a[1][0] * K1 + a[1][1] * K2, t + c[1] * dt)*dt;
		auto K4 = H(psi_0 + a[2][0] * K1 + a[2][1] * K2 + a[2][2] * K3, t + c[2] * dt)*dt;
		auto K5 = H(psi_0 + a[3][0] * K1 + a[3][1] * K2 + a[3][2] * K3 + a[3][3] * K4, t + c[3] * dt)*dt;
		auto K6 = H(psi_0 + a[4][0] * K1 + a[4][1] * K2 + a[4][2] * K3 + a[4][3] * K4 + a[4][4] * K5, t + c[4] * dt)*dt;

		auto psi_1 = psi_0 + b[0] * K1 + b[1] * K2 + b[2] * K3 + b[3] * K4 + b[4] * K5 + b[5] * K6;

		auto error = e[0] * K1 + e[1] * K2 + e[2] * K3 + e[3] * K4 + e[4] * K5 + e[5] * K6;

		return std::make_pair(std::move(psi_1), std::move(error));
	};

	// fixed step size loop
	/*while (t < t_f) {
		using namespace operators;

		const auto& psi_0 = psi.front();

		// some compile time sanity checks
		static_assert(std::is_lvalue_reference_v<decltype(psi_0)>, "Psi is NOT an lvalue reference!");

		static_assert(std::is_reference_v<decltype(H(psi_0, t))> == false, "H(psi) returns reference when it should always create a new object!");
		static_assert(std::is_reference_v<decltype(H(psi_0, t)*dt)> == false, "H(psi)*dt returns reference when it should always create a new object!");
		static_assert(std::is_reference_v<decltype(psi_0 + H(psi_0, t)*dt)> == false, "psi + H(psi)*dt returns reference when it should always create a new object!");

		static_assert(std::is_same_v<decltype(psi_0 + psi_0*dt), decltype(psi_0*dt)>, "{psi + psi*dt} and {psi*dt} are NOT the same type!");


		// euler
		// auto psi_1 = psi_0 + H_(psi_0, t)*dt;

		// RK2
		// auto K1		= H_(psi_0, t)*dt;

		// auto psi_1	= psi_0 + H(psi_0 + 0.5*K1, t + 0.5*dt)*dt;

		// RK4
		// auto K1 = H(psi_0, t)*dt;
		// auto K2 = H(psi_0 + 0.5*K1, t + 0.5*dt)*dt;
		// auto K3 = H(psi_0 + 0.5*K2, t + 0.5*dt)*dt;
		// auto K4 = H(psi_0 + K3, t + dt)*dt;

		// auto psi_1 = psi_0 + (K1 + 2.0*K2 + 2.0*K3 + K4)*sixth;

		// RKCK
		auto step_pair = RKCK_step(psi_0, t, dt);

		auto psi_1 = std::move(step_pair.first);

		psi.emplace_front(std::move(psi_1));

		t += dt;
	}*/

	auto step_successful = false;

	auto count = 0;

	// adaptive step size
	while (t < t_f) {
		const auto& psi_0 = psi.front();

		// try steps until success
		while (true) {
			// RKCK
			auto step_pair = RKCK_step(psi_0, t, dt);

			auto psi_1 = std::move(step_pair.first);
			auto error = std::move(step_pair.second);


			auto relative_error = [&psi_0, &error, &H, t, dt](auto x) {
				return std::abs(error(x)) / (std::abs(psi_0(x)) + std::abs((H(psi_0, t)*dt)(x)) + 1.0e-30);
			};

			const auto roots = util::find_roots(d_dx(relative_error));

			auto err_max = 0.0;
			auto x_max = 0.0;

			for (const auto& root : roots) {
				const auto val = relative_error(root);
				if (val > err_max) {
					err_max = val;
					x_max = root;
				}
			}

			std::cout << "Maximum relative error is: " << err_max << " at x = " << x_max << std::endl;

			err_max /= tol;

			if (err_max < 1.0) {
				std::cout << "Success: " << t << "\t" << dt << std::endl;

				// step successful

				/*for (auto x = -20.0; x <= 20.0; x += 0.1) {
					std::cout << x << ",\t" << psi_0(x) << ",\t" << psi_1(x) << ",\t" << relative_error(x) << std::endl;
				}*/

				psi.emplace_front(std::move(psi_1));
				t += dt;

				if (err_max > 1.89e-4) {
					std::cout << "Success: " << t << "\t" << dt;
					dt = 0.9*dt*std::pow(err_max, -0.2);
					std::cout << "\t" << std::endl;
				}
				else {
					std::cout << "Success: " << t << "\t" << dt;
					dt *= 5.0;
					std::cout << "\t" << std::endl;
				}

				break;
			} else {
				auto dt_temp = 0.9*dt*std::pow(err_max, -0.25);

				std::cout << "Fail: " << t << "\t" << dt << "\t" << dt_temp << std::endl;

				dt = ((dt >= 0.0) ? std::max(dt_temp, 0.1*dt) : std::min(dt_temp, 0.1*dt));
			}

		}

		if (count > 5) break;
	}


	return psi;
}


}	// fields