#pragma once

#include "function_traits.hpp"
#include "operators.hpp"

#include <array>
#include <functional>
#include <iostream>
#include <list>
#include <memory>
#include <unordered_map>

namespace fields {

template <typename T>
class Field {
 public:
	 template <typename F>
	 Field(F&& f) : f_(std::forward<F>(f)) {
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
		 field_.emplace_front(SharedField<Field<T>>(std::forward<Args>(args)...));
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

	const auto dt = Real{ 0.025 };
	const auto dt_2 = Real{ 0.5*dt };
	constexpr auto sixth = Real{ 1.0 / 6.0 };

	auto t = t_0;

	auto H = [&H_] (auto&& psi, double t) {
		return H_(wrap_function(std::forward<decltype(psi)>(psi)), t);
	};

	psi.emplace_front(std::forward<Psi>(psi_init));


	// RKCK coefficients	https://en.wikipedia.org/wiki/Cash%E2%80%93Karp_method
	constexpr auto c = std::array<Real, 6>{0.0, 0.2, 0.3, 1.0, 7.0/8.0};

	constexpr auto b = std::array<Real, 6>{(37.0 / 378.0), 0.0, (250.0 / 621.0), (125.0 / 594.0), 0.0, (512.0 / 1771.0)};

	constexpr auto e = std::array<Real, 6>{	std::get<0>(b) - (2825.0/27648.0),
											std::get<1>(b) - 0.0,
											std::get<2>(b) - (18575.0/48384.0),
											std::get<3>(b) - (13525.0/55296.0),
											std::get<4>(b) -(277.0/14336.0),
											std::get<5>(b) - 0.25};
	
	constexpr auto a = std::array<std::array<Real, 5>, 5> { std::array<Real, 5>{0.2, 0.0, 0.0, 0.0, 0.0},
															std::array<Real, 5>{ (3.0 / 40.0), (9.0 / 40.0), 0.0, 0.0, 0.0 },
															std::array<Real, 5>{ 0.3, -0.9, 1.2, 0.0, 0.0 },
															std::array<Real, 5>{ (-11.0 / 54.0), 2.5, (-70.0 / 27.0), (35.0 / 27.0), 0.0 },
															std::array<Real, 5>{ (1631.0 / 55296.0), (175.0 / 512.0), (575.0 / 13824.0), (44275.0 / 110592.0), (253.0 / 4096.0) }};



	while (t < t_f) {
		using namespace operators;
		
		const auto& psi_0 = psi.front();
	
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
		auto K1 = H(psi_0, t)*dt;
		auto K2 = H(psi_0 + 0.5*K1, t + 0.5*dt)*dt;
		auto K3 = H(psi_0 + 0.5*K2, t + 0.5*dt)*dt;
		auto K4 = H(psi_0 + K3, t + dt)*dt;

		auto psi_1 = psi_0 + (K1 + 2.0*K2 + 2.0*K3 + K4)*sixth;

		// RK5







		psi.emplace_front(std::move(psi_1));
		
		t += dt;
	}

	return psi;
}


}	// fields
