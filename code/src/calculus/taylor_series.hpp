#pragma once

#include "functions/constant.hpp"

#include "../util/util.hpp"

#include <utility>

namespace fields {

template <Int N, typename Hamiltonian, typename Phi_0>
constexpr auto integrate(Hamiltonian&& H, Phi_0&& phi_0);


namespace detail {

template <typename Hamiltonian>
class Field {
 public:
    constexpr explicit Field(Hamiltonian h) : H(std::move(h)) {
    }

    constexpr auto time_derivative() const { return H(*this); }

 private:

    Hamiltonian H;
};

template <typename Hamiltonian>
constexpr auto make_field(Hamiltonian&& H) { return Field<Hamiltonian>(std::forward<Hamiltonian>(H)); }

template <typename Hamiltonian>
constexpr auto d_dt(Field<Hamiltonian> phi) { return phi.time_derivative(); }

template <Int N, typename Hamiltonian>
constexpr auto d_dt(Field<Hamiltonian> phi) {
    static_assert(N >= 0);

    if constexpr (N == 0) {
        return phi;
    } else if constexpr (N == 1) {
        return d_dt(std::move(phi));
    } else {
        return d_dt(d_dt<N - 1>(std::move(phi)));
    }
}

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

template <Int N, typename Hamiltonian, typename Phi_0>
constexpr auto integrate(Hamiltonian&& H, Phi_0&& phi_0) {
    static_assert(N >= 0, "The 'Order' of the Taylor Series solution must be greater than zero.");

    return fields::detail::taylor_series(std::forward<Hamiltonian>(H), std::forward<Phi_0>(phi_0), std::make_integer_sequence<Int, N + 1>{});
}

}   // fields