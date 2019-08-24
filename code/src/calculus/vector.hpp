#pragma once

#include <tuple>

namespace fields {

template <typename... Ts>
class Vector {
 public:
	template <typename... Args>
	constexpr Vector(Args... args) : components_(args...) {
	}

	constexpr auto components() const { return components_; }

	template <std::size_t I>
	constexpr auto get() const { return std::get<I>(components_); }

	template <typename... Args>
	constexpr auto operator()(Args... args) const { return function_call_impl(component_indicies_, std::make_tuple(args...)); }

	template <typename D>
	constexpr auto derivative(D d) const { return derivative_impl(d, component_indicies_); }

	template <typename F>
	constexpr auto multiply_scalar(F f) const { return multiply_scalar_impl(f, component_indicies_); }

 private:
	 template <std::size_t... Is, typename Tuple>
	 constexpr auto function_call_impl(std::index_sequence<Is...>, Tuple args) const { return Vector<decltype((std::apply(get<Is>(), args))) ...>{ (std::apply(get<Is>(), args)) ... }; }
	 
	 template <typename D, std::size_t... Is>
	 constexpr auto derivative_impl(D d, std::index_sequence<Is...>) const { return Vector<decltype(d(get<Is>()))...>{ d(get<Is>()) ... }; }
	 
	 template <typename F, std::size_t... Is>
	 constexpr auto multiply_scalar_impl(F f, std::index_sequence<Is...>) const { return Vector<decltype((f * get<Is>()))...>{(f* get<Is>()) ... }; }

	 std::tuple<Ts...> components_;

	 constexpr static auto N = sizeof...(Ts);

	 constexpr static auto component_indicies_ = std::make_index_sequence<N>{};
};

template <typename... Args>
Vector(Args...)->Vector<Args...>;


}	// fields