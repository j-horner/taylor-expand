#pragma once

#include <functional>
#include <utility>

namespace fields {
namespace util {

// For generic types, directly use the result of the signature of its 'operator()'
template <typename T>
struct function_traits : public function_traits<decltype(&T::operator())> {};

// we specialize for pointers to member function
template <typename ClassType, typename ReturnType, typename... Args>
struct function_traits<ReturnType(ClassType::*)(Args...) const> {
	// arity is the number of arguments.
	constexpr static auto n_args = sizeof...(Args);

	using result_type = ReturnType;

	template <size_t i>
	struct arg {
		// the i-th argument is equivalent to the i-th tuple element of a tuple
		// composed of those arguments.
		// typedef typename std::tuple_element<i, std::tuple<Args...>>::type type;
		using type = typename std::tuple_element<i, std::tuple<Args...>>::type;
	};
};

}	// util
}	// fields

