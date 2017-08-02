#pragma once

#include "function_traits.hpp"
#include "operators.hpp"

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
	 Field(F&& f) : f_(std::forward<F>(f)), id_(count_) {
		 ++count_;
	 }

	template <typename X>
	auto operator()(X&& x) const {
		++called_;
		 
		const auto idx = static_cast<int>(x*dx_1);

		const auto location = memory_.find(idx);

		if (memory_.end() != location) {
			// std::cout << "Found result, returning...\t" << id_ << "\t" << called_ << "\t" << x << "\t" << idx << std::endl;
			return location->second;
		} else {
			// std::cout << "Not found, adding result to memory\t" << id_ << "\t" << called_ << "\t" << x << "\t" << idx << std::endl;
			
			const auto val = f_(std::forward<X>(x));

			memory_.emplace(idx, val);

			return val;
		}
	 }

 private:
	// using Result = std::function<T>::result_type;

	std::function<T> f_;

	int id_ = 0;
	
	mutable int called_ = 0;
	mutable std::unordered_map<int, double> memory_;

	static int count_;

	constexpr static auto dx_1 = 1.0e6;		// inverse of the dynamic lattice resolution
};

template <typename T>
int Field<T>::count_ = 0;

template <typename T>
class FieldList {
 public:
	 template <typename... Args>
	 auto operator()(Args&&... args) const {
		 // std::cout << __FUNCTION__ << "\ttotal Field objects: " << field_.size() << std::endl;
		 return front().func()(std::forward<Args>(args)...);
	 }

	 auto front() const -> decltype(auto) {
		 return field_.front();
	 }

	 template <typename... Args>
	 auto emplace_front(Args&&... args) {
		 // std::cout << __FUNCTION__ << std::endl;
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
	using namespace operators;

	using traits = util::function_traits<std::decay_t<Psi>>;

	using Ret = traits::result_type;

	using Arg = traits::arg<0>::type;

	auto psi = FieldList<Ret(Arg)>{};

	const auto dt = Real{ 0.01 };
	const auto dt_2 = Real{ 0.5*dt };
	constexpr auto sixth = Real{ 1.0 / 6.0 };

	auto t = t_0;

	auto count = 0;

	auto H = [&H_] (auto&& psi, double t) {
		return H_(wrap_function(std::forward<decltype(psi)>(psi)), t);
		// using F = decltype(psi);
		// return H_(SharedField<F>(std::forward<F>(psi)), t);
	};

	psi.emplace_front(std::forward<Psi>(psi_init));

	while (t < t_f) {
		const auto& psi_0 = psi.front();
	
		static_assert(std::is_lvalue_reference_v<decltype(psi_0)>, "Psi is NOT an lvalue reference!");

		static_assert(std::is_reference_v<decltype(H(psi_0, t))> == false, "H(psi) returns reference when it should always create a new object!");
		static_assert(std::is_reference_v<decltype(H(psi_0, t)*dt)> == false, "H(psi)*dt returns reference when it should always create a new object!");
		static_assert(std::is_reference_v<decltype(psi_0 + H(psi_0, t)*dt)> == false, "psi + H(psi)*dt returns reference when it should always create a new object!");

		static_assert(std::is_same_v<decltype(psi_0 + psi_0*dt), decltype(psi_0*dt)>, "{psi + psi*dt} and {psi*dt} are NOT the same type!");


		// std::cout << "Constructed next function." << std::endl;

		// euler
		// auto psi_1 = psi_0 + H_(psi_0, t)*dt;

		// RK2	
		// auto K1 = H_(psi_0, t)*dt;	
		
		// auto psi_1 = psi_0 + H(psi_0 + 0.5*K1, t + 0.5*dt)*dt;
		
		// RK4
		auto K1 = H(psi_0, t)*dt;

		auto K2 = H(psi_0 + 0.5*K1, t + 0.5*dt)*dt;

		auto K3 = H(psi_0 + 0.5*K2, t + 0.5*dt)*dt;

		auto K4 = H(psi_0 + K3, t + dt)*dt;

		auto psi_1 = psi_0 + (K1 + 2.0*K2 + 2.0*K3 + K4)*sixth;


		psi.emplace_front(std::move(psi_1));
		

		// std::cout << "Function moved into list" << std::endl;

		// std::cout << t << "\t----------" << std::endl;

		t += dt;

		++count;

		// if (count > 0) break;
	}

	return psi;
}


}	// fields
