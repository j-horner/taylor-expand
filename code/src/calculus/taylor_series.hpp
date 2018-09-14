#pragma once

#include "functions/constant.hpp"

#include "../util/util.hpp"

#include <utility>

namespace fields {

template <Int N, typename Hamiltonian, typename Phi_0>
constexpr auto integrate(Hamiltonian&& H, Phi_0&& phi_0);


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

    // remove and add template Field friend?
    constexpr auto H() const { return H_; }

 private:

    Hamiltonian H_;
};

template <typename Hamiltonian>
constexpr auto make_field(Hamiltonian H) { return Field<Hamiltonian>{H}; }

template <typename Hamiltonian, typename Phi_0, Int... Ns>
constexpr auto taylor_series(Hamiltonian&& H, Phi_0&&, std::integer_sequence<Int, Ns...>) {
    static_assert(sizeof...(Ns) > 0);

    using namespace operators;
    using namespace literals;

    auto phi = make_field(std::forward<Hamiltonian>(H));

    return ((1_c/Constant<util::factorial(Ns)>{}) + ...);
    // return ((d_dt<Ns>(phi)/Constant<util::factorial(Ns)>{})(std::forward<Phi_0>(phi_0)) + ...);
}

}   // detail

namespace operators {

template <Int N = 1, typename Hamiltonian, Int M>
constexpr auto d_dx(fields::detail::Field<Hamiltonian, M> phi) {
    static_assert(N >= 0);
    return Field<Hamiltonian, M + N>(phi);
}

template <Int N = 1, typename Hamiltonian, Int M>
constexpr auto d_dt(fields::detail::Field<Hamiltonian, M> phi) {
    static_assert(N >= 0);

    if constexpr (N == 0) {
        return phi;
    } else if constexpr (N == 1) {
        return phi.time_derivative();
    } else {
        return d_dt(d_dt<N - 1>(phi));
    }
}

}   // operators

template <Int N, typename Hamiltonian, typename Phi_0>
constexpr auto integrate(Hamiltonian&& H, Phi_0&& phi_0) {
    static_assert(N >= 0, "The 'Order' of the Taylor Series solution must be greater than zero.");

    return fields::detail::taylor_series(std::forward<Hamiltonian>(H), std::forward<Phi_0>(phi_0), std::make_integer_sequence<Int, N + 1>{});
}

}   // fields