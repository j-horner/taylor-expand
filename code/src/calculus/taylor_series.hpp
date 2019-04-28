#pragma once

#include "functions/constant.hpp"

#include "../util/util.hpp"

#include <utility>

namespace fields {

template <Int N, typename Hamiltonian, typename Phi_0>
constexpr auto taylor_expand(Hamiltonian H, Phi_0 phi_0);


namespace detail {

template <typename Hamiltonian, Int N = 0>
class Field {
 public:
    constexpr explicit Field(Hamiltonian h) : H_(h) {
    }

    template <Int M>
    constexpr explicit Field(Field<Hamiltonian, M> phi) : H_(phi.H()) {
    }

    constexpr auto time_derivative() const { return d_dx<N>(H_(Field<Hamiltonian>{H_})); }

    template <typename Phi, typename X_, typename T_>
    constexpr auto operator()(Phi phi, X_, T_) const { return d_dx<N>(phi); }

    // remove and add template Field friend?
    constexpr auto H() const { return H_; }

 private:

    Hamiltonian H_;
};

template <typename Hamiltonian, Int N>
auto operator<<(std::ostream& os, Field<Hamiltonian, N>) -> std::ostream& {
    if constexpr (N == 0) {
        os << "y";
    } else if constexpr (N == 1) {
        os << "d_dx(y)";
    } else {
        os << "d_dx<" << N << ">(y)";
    }
    return os;
}

template <typename Hamiltonian>
constexpr auto make_field(Hamiltonian H) { return Field<Hamiltonian>{H}; }

template <typename Hamiltonian, typename Phi_0, typename T_0, Int... Ns>
constexpr auto taylor_series(Hamiltonian H, Phi_0 phi_0, T_0 t_0, std::integer_sequence<Int, Ns...>) {
    static_assert(sizeof...(Ns) > 0);

    using namespace operators;

    auto phi = make_field(H);

    return (... + ((d_dt<Ns>(phi)(phi_0, x, t_0)/Constant<util::factorial(Ns)>{})*(t^Constant<Ns>{})));
}

}   // detail

template <Int N, typename Hamiltonian, typename Y_0>
constexpr auto taylor_expand(Hamiltonian H, Y_0 y_0) {
    static_assert(N >= 0, "The 'Order' of the Taylor Series solution must be greater than zero.");

    return fields::detail::taylor_series(H, y_0, 0_c, std::make_integer_sequence<Int, N + 1>{});
}

template <Int N, typename Hamiltonian, typename Y_0, typename T_0>
constexpr auto taylor_expand(Hamiltonian H, Y_0 y_0, T_0 t_0) {
    static_assert(N >= 0, "The 'Order' of the Taylor Series solution must be greater than zero.");

    return fields::detail::taylor_series(H, y_0, t_0, std::make_integer_sequence<Int, N + 1>{});
}

}   // fields