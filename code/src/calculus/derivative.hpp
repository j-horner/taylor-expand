#pragma once

#include "functions/constant.hpp"
#include "functions/linear.hpp"
#include "functions/power.hpp"

#include "addition.hpp"
#include "subtraction.hpp"
#include "multiplication.hpp"
#include "division.hpp"
#include "taylor_series.hpp"

// C headers
#include <cstdint>

namespace fields {

using Int = std::intmax_t;


namespace detail {

class X;
class T;

template <typename H, Int N>
class Field;

} // detail

template <typename F, Int N>
class Power;

namespace operators {

// all constants have 0 derivative
template <Int D = 1, Int A, Int B>
constexpr auto d_dx(Constant<A, B>) {
    static_assert(D >= 0);
    using namespace literals;
    if constexpr (D == 0) {
        return Constant<A, B>{};
    } else {
        return 0_c;
    }
}

template <Int D = 1, Int A, Int B>
constexpr auto d_dt(Constant<A, B>) {
    static_assert(D >= 0);
    using namespace literals;
    if constexpr (D == 0) {
        return Constant<A, B>{};
    } else {
        return 0_c;
    }
}

template <Int D = 1>
constexpr auto d_dx(fields::detail::X) {
    static_assert(D >= 0);

    using namespace literals;
    if constexpr (D == 0) {
        return x;
    } else if constexpr (D == 1) {
        return 1_c;
    } else {
        return 0_c;
    }
}

template <Int D = 1>
constexpr auto d_dx(fields::detail::T) {
    static_assert(D >= 0);

    using namespace literals;
    if constexpr (D == 0) {
        return t;
    } else {
        return 0_c;
    }
}

template <Int D = 1>
constexpr auto d_dt(fields::detail::X) {
    static_assert(D >= 0);

    using namespace literals;
    if constexpr (D == 0) {
        return x;
    } else {
        return 0_c;
    }
}

template <Int D = 1>
constexpr auto d_dt(fields::detail::T) {
    static_assert(D >= 0);

    using namespace literals;
    if constexpr (D == 0) {
        return t;
    } else if constexpr (D == 1) {
        return 1_c;
    } else {
        return 0_c;
    }
}

template <Int D = 1, typename F, Int N>
constexpr auto d_dx(Power<F, N> y) {
    static_assert(D >= 0);
    if constexpr (D == 0) {
        return y;
    } else if constexpr (D == 1) {
        if constexpr (N == 2) {
            return Constant<N>{}*y.f()*d_dx(y.f());
        } else {
            return Constant<N>{}*Power<F, N - 1>{y}*d_dx(y.f());
        }
    } else {
        return d_dx<D - 1>(d_dx(y));
    }
}

template <Int D = 1, typename F, Int N>
constexpr auto d_dt(Power<F, N> y) {
    static_assert(D >= 0);
    if constexpr (D == 0) {
        return y;
    } else if constexpr (D == 1) {
        if constexpr (N == 2) {
            return Constant<N>{}*y.f()*d_dt(y.f());
        } else {
            return Constant<N>{}*Power<F, N - 1>{y}*d_dt(y.f());
        }
    } else {
        return d_dt<D - 1>(d_dt(y));
    }
}


// Derivative operators
template <Int D = 1, typename... Fs>
constexpr auto d_dx(Addition<Fs...> y) { return y.derivative([] (auto f) { return d_dx<D>(f); }); }

template <Int D = 1, typename... Fs>
constexpr auto d_dt(Addition<Fs...> y) { return y.derivative([] (auto f) { return d_dt<D>(f); }); }

template <Int D = 1, typename F, typename G>
constexpr auto d_dx(Subtraction<F, G> y) { return d_dx<D>(y.lhs) - d_dx<D>(y.rhs); }

template <Int D = 1, typename F, typename G>
constexpr auto d_dt(Subtraction<F, G> y) { return d_dt<D>(y.lhs) - d_dt<D>(y.rhs); }


// Derivative operators
template <Int D = 1, typename... Fs>
constexpr auto d_dx(Multiplication<Fs...> y) {
    if constexpr (D == 0) {
        return y;
    } else {
        return d_dx<D - 1>(y.derivative([] (auto f) { return d_dx(f); }));
    }
}

template <Int D = 1, typename... Fs>
constexpr auto d_dt(Multiplication<Fs...> y) {
    if constexpr (D == 0) {
        return y;
    } else {
        return d_dt<D - 1>(y.derivative([] (auto f) { return d_dt(f); }));
    }
}

template <Int D = 1, typename F, typename G>
constexpr auto d_dx(Division<F, G> y) {
    static_assert(D >= 0);
    if constexpr (D == 0) {
        return y;
    } else {
        return d_dx<D - 1>(d_dx(y.lhs*Power<G, -1>{y.rhs}));
        // return d_dx<D - 1>((d_dx(y.lhs)*y.rhs - y.lhs*d_dx(y.rhs))/(y.rhs*y.rhs));
    }
}

template <Int D = 1, typename F, typename G>
constexpr auto d_dt(Division<F, G> y) {
    static_assert(D >= 0);
    if constexpr (D == 0) {
        return y;
    } else {
        return d_dt<D - 1>(d_dt(y.lhs*Power<G, -1>{y.rhs}));
        // return d_dt<D - 1>((d_dt(y.lhs)*y.rhs - y.lhs*d_dt(y.rhs))/(y.rhs*y.rhs));
    }
}

template <Int D = 1, typename F>
constexpr auto d_dx(F f) {
    if constexpr (D == 0) {
        return f;
    } else {
        static_assert(false, "The derivative of this type is not known.");
    }
}

template <Int D = 1, typename F>
constexpr auto d_dt(F f) {
    if constexpr (D == 0) {
        return f;
    } else {
        static_assert(false, "The derivative of this type is not known.");
    }
}

template <Int N = 1, typename Hamiltonian, Int M>
constexpr auto d_dx(fields::detail::Field<Hamiltonian, M> phi) {
    static_assert(N >= 0);
    return fields::detail::Field<Hamiltonian, M + N>(phi);
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


} // operators
} // fields
