#pragma once

#include "functions/constant.hpp"

#include "derivative.hpp"

namespace fields {
namespace detail {

template <typename Hamiltonian, Int N>
class Field;

}

template <Int N, typename Hamiltonian, Int M>
constexpr auto d_dx(fields::detail::Field<Hamiltonian, M> phi) ->fields::detail::Field<Hamiltonian, M + N>;


namespace detail {

///
/// @brief  A class whose time derivative returns the Hamiltonian applied to itself. e.g.
///
///             Field y{H};     static_assert(d_dt(y) == H(y));
///
/// @tparam Hamiltonian     Defines the time derivative of this class.
/// @tparam N               Tracks spatial derivatives applied to this class. 
///
template <typename Hamiltonian, Int N = 0>
class Field {
public:
	constexpr explicit Field(Hamiltonian h) : H_(h) {
	}

	///
	/// @brief  All Field<Hamiltonian, M> classes share the same Hamiltonian regardless of M. 
	///
	template <Int M>
	constexpr explicit Field(Field<Hamiltonian, M> phi) : H_(phi.H()) {
	}

	///
	/// @brief  Implements d_dt(...) (TODO: could remove this and make d_dt a friend). Takes advantage of: d_dt(d_dx<N>(y)) == d_dx<N>(d_dt(y))
	///
	// constexpr auto time_derivative() const { return operators::d_dx<N>(H_(Field<Hamiltonian>{H_})); }

	///
	/// @brief  Returns d_dx<N>(y).
	///         Required for evaluating Taylor series coefficients from initial condition y_0.
	///         x and t arguments are required for a compatible interface but are ignored.
	///
	template <typename Y, typename X_, typename T_>
	constexpr auto operator()(Y y, X_, T_) const { return d_dx<N>(y); }

	// remove and add template Field friend? where is this used?
	constexpr auto H() const { return H_; }

 private:

	Hamiltonian H_;
};

///
/// @brief  Prints out "y" or "d_dx<N>(y)".
///
template <typename Stream, typename Hamiltonian, Int N>
auto& operator<<(Stream& os, Field<Hamiltonian, N>) {
	if constexpr (N == 0) {
		os << "y";
	}
	else if constexpr (N == 1) {
		os << "d_dx(y)";
	}
	else {
		os << "d_dx<" << N << ">(y)";
	}
	return os;
}

///
/// @brief  Creates an object whose time derivative is given by the Hamiltonian
///
template <typename Hamiltonian>
constexpr auto make_field(Hamiltonian H) { return Field<Hamiltonian>{H}; }

}	// detail
}	// fields
