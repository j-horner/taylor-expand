#pragma once

#include "operators/math_operators.hpp"

#include "functions/constant.hpp"

#include "../util/util.hpp"

#include "field.hpp"
#include "vector.hpp"

#include <utility>

namespace fields {

// Main interface functions

///
/// @brief Provides the Nth order Taylor series solution to a differential equation "dy/dt = H(y)" with initial condition y(0) = y_0
///
/// @tparam N               The order of the taylor expansion. If N == 0, this function returns y_0.
/// @tparam Hamiltonian     The type of the differential equation, typically a polymorphic lambda.
///                         Must have templated function operator which returns an expression for the differential equation e.g.:
///
///                             constexpr auto H = [] (auto y) { return d_dx<2>(y); };    // diffusion equation
///
/// @tparam Y_0     The type of the initial condition at t = 0. Must be Constant or a function of x.
///
/// @param H        The Hamiltonian defining the equation to be solved. Typically a stateless constexpr polymorphic lambda.
/// @param y_0      The initial condition at t = 0. Must be Constant or a function of x.
///
/// @return         The Taylor series solution as a function of time.
///
template <Int N, typename Hamiltonian, typename Y_0>
constexpr auto taylor_expand(Hamiltonian H, Y_0 y_0);

///
/// @brief Provides the Nth order Taylor series solution to a differential equation "dy/dt = H(y)" with initial condition y(t_0) = y_0
///
/// @tparam N               The order of the taylor expansion. If N == 0, this function returns y_0.
/// @tparam Hamiltonian     The type of the differential equation, typically a polymorphic lambda.
///                         Must have templated function operator which returns an expression for the differential equation e.g.:
///
///                             constexpr auto H = [] (auto y) { return d_dx<2>(y); };    // diffusion equation
///
/// @tparam Y_0             The type of the initial condition at t = t_0. Must be Constant or a function of x.
/// @tparam Constant<A, B>  The type of the time which is expanded around. 
///
/// @param H        The Hamiltonian defining the equation to be solved. Typically a stateless constexpr polymorphic lambda.
/// @param y_0      The initial condition at t = t_0. Must be Constant or a function of x.
/// @param t_0      The time which is expanded around.
///
/// @return         The Taylor series solution as a function of time.
///
template <Int N, typename Hamiltonian, typename Y_0, Int A, Int B>
constexpr auto taylor_expand(Hamiltonian H, Y_0 y_0, Constant<A, B> t_0);





namespace detail {

template <typename Hamiltonian, Int... Ns>
constexpr auto make_phi_vector_impl(Hamiltonian H, std::integer_sequence<Int, Ns...>) {
	auto make_phi = [H](auto) { return Field{ H }; };
	return Vector{ make_phi(Ns)... };
}


template <Int N, typename Hamiltonian>
constexpr auto make_phi_vector(Hamiltonian H) {
	return make_phi_vector_impl(H, std::make_integer_sequence<Int, N>{});
}

template <typename Hamiltonian, typename... Phis, typename T_0, Int... Ns>
constexpr auto taylor_series(Hamiltonian H, Vector<Phis...> phi_0, T_0 t_0, std::integer_sequence<Int, Ns...>) {
	static_assert(sizeof...(Ns) > 1);
	static_assert(sizeof...(Phis) > 1);


	auto phis = make_field_vector<sizeof...(Phis)>(H);

	// the definition of a Taylor expansion
	return (... + ((d_dt<Ns>(phis)(phi_0, x, t_0) / Constant<util::factorial(Ns)>{})* ((t - t_0) ^ Constant<Ns>{})));
}


template <typename Hamiltonian, typename Phi_0, typename T_0, Int... Ns>
constexpr auto taylor_series(Hamiltonian H, Phi_0 phi_0, T_0 t_0, std::integer_sequence<Int, Ns...>) {
	static_assert(sizeof...(Ns) > 1);

	auto phi = Field{H};

	// the definition of a Taylor expansion
	return (... + ((d_dt<Ns>(phi)(phi_0, x, t_0)/Constant<util::factorial(Ns)>{})* ((t - t_0)^Constant<Ns>{})));
}

}   // detail

template <Int N, typename Hamiltonian, typename Y_0>
constexpr auto taylor_expand(Hamiltonian H, Y_0 y_0) {
	static_assert(N >= 0, "The 'Order' of the Taylor Series solution must be greater than or equal to zero.");

	if constexpr (0 == N) {
		return y_0;
	} else {
		return fields::detail::taylor_series(H, y_0, Constant<0>{}, std::make_integer_sequence<Int, N + 1>{});
	}
}

template <Int N, typename Hamiltonian, typename Y_0, Int A, Int B>
constexpr auto taylor_expand(Hamiltonian H, Y_0 y_0, Constant<A, B> t_0) {
	static_assert(N >= 0, "The 'Order' of the Taylor Series solution must be greater than or equal to zero.");

	if constexpr (0 == N) {
		return y_0;
	} else {
		return fields::detail::taylor_series(H, y_0, t_0, std::make_integer_sequence<Int, N + 1>{});
	}
}

}   // fields