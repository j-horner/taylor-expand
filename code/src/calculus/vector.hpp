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
	constexpr auto operator()(Args... args) const { return function_call_impl(component_indicies_{}, std::make_tuple(args...)); }

	template <typename D>
	constexpr auto derivative(D d) const { return derivative_impl(d, component_indicies_{}); }

	template <typename F>
	constexpr auto multiply_scalar(F f) const { return multiply_scalar_impl(f, component_indicies_{}); }

	template <typename... Us>
	constexpr auto addition(Vector<Us...> u) const { return addition_impl(u, component_indicies_{}); }

	template <typename Stream>
	auto print(Stream& os) const -> void { print_impl(os, component_indicies_{}); }

 private:
	 template <std::size_t... Is, typename Tuple>
	 constexpr auto function_call_impl(std::index_sequence<Is...>, Tuple args) const { return Vector<decltype((std::apply(get<Is>(), args))) ...>{ (std::apply(get<Is>(), args)) ... }; }
	 
	 template <typename D, std::size_t... Is>
	 constexpr auto derivative_impl(D d, std::index_sequence<Is...>) const { return Vector<decltype(d(get<Is>()))...>{ d(get<Is>()) ... }; }
	 
	 template <typename F, std::size_t... Is>
	 constexpr auto multiply_scalar_impl(F f, std::index_sequence<Is...>) const { return Vector<decltype((f * get<Is>()))...>{(f* get<Is>()) ... }; }

	 template <typename... Us, std::size_t... Is>
	 constexpr auto addition_impl(Vector<Us...> u, std::index_sequence<Is...>) const { return Vector<decltype(get<Is>() + u.template get<Is>())...>{(get<Is>() + u.template get<Is>())...}; }

	 template <typename Stream, typename T>
	 auto print(Stream& os, T last) const -> void {
		 os << last;
	 }
	 
	 template <typename Stream, typename T, typename... Rest>
	 auto print(Stream& os, T first, Rest... rest) const -> void {
		 os << first << " , ";
		 print(os, rest...);
	 }

	 template <typename Stream, std::size_t... Is>
	 constexpr auto print_impl(Stream& os, std::index_sequence<Is...>) const -> void {
		 print(os, get<Is>()...);
	 }

	 std::tuple<Ts...> components_;

	 using component_indicies_ = std::make_index_sequence<sizeof...(Ts)>;
};

template <typename... Args>
Vector(Args...) -> Vector<Args...>;

template <typename Stream, typename... Fs>
auto& operator<<(Stream& os, Vector<Fs...> y) {
	os << "(";
	y.print(os);
	os << ")";

	return os;
}


}	// fields

namespace std {

template <typename... Ts>
struct tuple_size<fields::Vector<Ts...>> {
	constexpr static auto value = sizeof...(Ts);
};

template <std::size_t I, typename... Ts>
struct tuple_element<I, fields::Vector<Ts...>> : tuple_element<I, tuple<Ts...>> {

};

}	// std