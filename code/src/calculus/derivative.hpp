#pragma once

#include "functions/constant.hpp"
#include "functions/linear.hpp"
#include "functions/power.hpp"

#include "addition.hpp"

#include "field.hpp"
#include "vector.hpp"
#include "field_vector.hpp"

namespace fields {

namespace detail {

template <typename Hamiltonian, Int N>
class Field;

template <typename Hamiltonian>
constexpr auto make_field(Hamiltonian H);

}


template <Int N = 1, typename Hamiltonian, Int M>
constexpr auto d_dx(fields::detail::Field<Hamiltonian, M> phi) -> fields::detail::Field<Hamiltonian, M + N>;

template <Int N = 1, typename Hamiltonian, Int M>
constexpr auto d_dt(fields::detail::Field<Hamiltonian, M> phi);

// all constants have 0 derivative
template <Int D = 1, Int A, Int B>
constexpr auto d_dx(Constant<A, B>) {
    static_assert(D >= 0);
    if constexpr (D == 0) {
        return Constant<A, B>{};
    } else {
		return Constant<0>{};
    }
}

template <Int D = 1, Int A, Int B>
constexpr auto d_dt(Constant<A, B>) {
    static_assert(D >= 0);
    if constexpr (D == 0) {
        return Constant<A, B>{};
    } else {
        return Constant<0>{};
    }
}

template <Int D = 1>
constexpr auto d_dx(X) {
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
constexpr auto d_dx(T) {
    static_assert(D >= 0);

    if constexpr (D == 0) {
        return t;
    } else {
		return Constant<0>{};
    }
}

template <Int D = 1>
constexpr auto d_dt(X) {
    static_assert(D >= 0);

    if constexpr (D == 0) {
        return x;
    } else {
		return Constant<0>{};
    }
}

template <Int D = 1>
constexpr auto d_dt(T) {
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

template <Int N, typename Hamiltonian, Int M>
constexpr auto d_dx(fields::detail::Field<Hamiltonian, M> phi) -> fields::detail::Field<Hamiltonian, M + N> {
	static_assert(N >= 0);
	return fields::detail::Field<Hamiltonian, M + N>(phi);
}

template <Int N, typename Hamiltonian, Int M>
constexpr auto d_dt(fields::detail::Field<Hamiltonian, M> phi) {
	static_assert(N >= 0);

	if constexpr (N == 0) {
		return phi;
	}
	else if constexpr (N == 1) {
		auto H = phi.H();

		return d_dx<M>(H(fields::detail::Field{H}));
	}
	else {
		return d_dt(d_dt<N - 1>(phi));
	}
}


// Derivative operators
template <Int D = 1, typename... Fs>
constexpr auto d_dx(Addition<Fs...> y) { return y.derivative([](auto f) { return d_dx<D>(f); }); }

template <Int D = 1, typename... Fs>
constexpr auto d_dt(Addition<Fs...> y) { return y.derivative([](auto f) { return d_dt<D>(f); }); }


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

// vectors
template <Int D = 1, typename... Fs>
constexpr auto d_dx(fields::Vector<Fs...> y) { return y.derivative([](auto f) { return d_dx<D>(f); }); }

template <Int D = 1, typename... Fs>
constexpr auto d_dt(fields::Vector<Fs...> y) { return y.derivative([](auto f) { return d_dt<D>(f); }); }

template <Int N = 1, typename Hamiltonian, std::size_t D>
constexpr auto d_dt(FieldVector<Hamiltonian, D> y) {
	static_assert(N >= 0);

	if constexpr (N == 0) {
		return y;
	}
	else if constexpr (N == 1) {
		return y.vector_of_time_derivatives();
	}
	else {
		return d_dt<N - 1>(d_dt(y));
	}
}

template <Int N = 1, typename Hamiltonian, std::size_t Idx, std::size_t Dim, Int SpatialDerivative>
constexpr auto d_dt(FieldVectorComponent<Hamiltonian, Idx, Dim, SpatialDerivative> p) {
	static_assert(N >= 0);

	if constexpr (N == 0) {
		return p;
	}
	else if constexpr (N == 1) {
		return d_dx<SpatialDerivative>(p.equations.vector_of_time_derivatives().template get<Idx>());
	}
	else {
		return d_dt<N - 1>(d_dt(p));
	}
}

template <Int N = 1, typename Hamiltonian, std::size_t Idx, std::size_t Dim, Int SpatialDerivative>
constexpr auto d_dx(FieldVectorComponent<Hamiltonian, Idx, Dim, SpatialDerivative> p) {
	static_assert(N >= 0);
	return FieldVectorComponent<Hamiltonian, Idx, Dim, SpatialDerivative + N>{p.equations};
}

template <Int D = 1, typename F>
constexpr auto d_dx(F f) {
	static_assert(D == 0, "The derivative of unknown functions is only known for D == 0");
	return f;
}

template <Int D = 1, typename F>
constexpr auto d_dt(F f) {
	static_assert(D == 0, "The derivative of unknown functions is only known for D == 0");
	return f;
}

} // fields
