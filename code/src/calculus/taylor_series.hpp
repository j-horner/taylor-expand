#pragma once

#include "functions/constant.hpp"

#include "../util/util.hpp"

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
    constexpr auto time_derivative() const { return d_dx<N>(H_(Field<Hamiltonian>{H_})); }

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
auto operator<<(Stream& os, Field<Hamiltonian, N>) -> Stream& {
    if constexpr (N == 0) {
        os << "y";
    } else if constexpr (N == 1) {
        os << "d_dx(y)";
    } else {
        os << "d_dx<" << N << ">(y)";
    }
    return os;
}

///
/// @brief  Creates an object whose time derivative is given by the Hamiltonian
///
template <typename Hamiltonian>
constexpr auto make_field(Hamiltonian H) { return Field<Hamiltonian>{H}; }

template <typename Hamiltonian, typename Phi_0, typename T_0, Int... Ns>
constexpr auto taylor_series(Hamiltonian H, Phi_0 phi_0, T_0 t_0, std::integer_sequence<Int, Ns...>) {
    static_assert(sizeof...(Ns) > 0);

    using namespace operators;

    auto phi = make_field(H);

    // the definition of a Taylor expansion
    return (... + ((d_dt<Ns>(phi)(phi_0, x, t_0)/Constant<util::factorial(Ns)>{})*((t - t_0)^Constant<Ns>{})));
}

}   // detail

template <Int N, typename Hamiltonian, typename Y_0>
constexpr auto taylor_expand(Hamiltonian H, Y_0 y_0) {
    static_assert(N >= 0, "The 'Order' of the Taylor Series solution must be greater than or equal to zero.");

    return fields::detail::taylor_series(H, y_0, 0_c, std::make_integer_sequence<Int, N + 1>{});
}

template <Int N, typename Hamiltonian, typename Y_0, Int A, Int B>
constexpr auto taylor_expand(Hamiltonian H, Y_0 y_0, Constant<A, B> t_0) {
    static_assert(N >= 0, "The 'Order' of the Taylor Series solution must be greater than or equal to zero.");

    return fields::detail::taylor_series(H, y_0, t_0, std::make_integer_sequence<Int, N + 1>{});
}

}   // fields