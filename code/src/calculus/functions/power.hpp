#pragma once

#include "constant.hpp"

#include "../../util/util.hpp"
#include "../../util/integer.hpp"

namespace fields {

template <Int A, Int B>
class Constant;

template <typename... Fs>
class Addition;

template <typename... Fs>
class Multiplication;

namespace detail {

template <typename T, typename U>
struct is_power : std::false_type {};

}

template <typename F, Int N, typename Enabled = std::enable_if_t<fields::detail::is_power<F, F>::value == false>>
class Power;

template <typename F, Int N>
class Power<F, N> {
    static_assert(std::is_same_v<F, Constant<0, 1>> == false, "0 should not have powers taken! 0^N: -> 0 for N > 0, -> 1 for N = 0 and is undefined for N < 0.");
    static_assert(std::is_same_v<F, Constant<1, 1>> == false, "1 should not have powers taken! 1^N: -> 1 for and N.");

	static_assert(fields::detail::is_constant<F>::value == false, "Power of Constant should be another Constant");

    static_assert(fields::detail::is_power<F, F>::value == false, "Power<Power<T, N>, M> should not be created. Create Power<T, N*M> instead.");

    static_assert(N != 0, "F^0 -> 1");
    static_assert(N != 1, "F^1 -> F");

 public:
    explicit constexpr Power(F f) : f_(f) {
    }

    template <Int M>
    explicit constexpr Power(Power<F, M> f) : f_(f.f_) {
    }

    constexpr operator F() const { return f_; }

    template <typename... Args>
    constexpr auto operator()(Args... args) const {

		if constexpr ((std::is_arithmetic_v<Args> && ...)) {
            return util::pow(f_(args...), N);
        } else if constexpr (std::is_same_v<decltype(f_(args...)), Constant<0>>) {
			return Constant<0>{};
        } else if constexpr (std::is_same_v<decltype(f_(args...)), Constant<1>>) {
			return Constant<1>{};
		} else if constexpr (fields::detail::is_constant<decltype(f_(args...))>::value) {
			return (f_(args...))^Constant<N>{};
		} else {
            return Power<decltype(f_(args...)), N>{f_(args...)};
        }
    }

    constexpr auto f() const { return f_; }

    constexpr static auto exponent() { return N; }


 private:
    template <typename G, Int M, typename Enabled> friend class Power;

    F f_;
};

template <typename Stream, typename F, Int N>
auto operator<<(Stream& os, Power<F, N> y) -> Stream& {
    os << "(" << y.f() << ")" << "^{" << N << "}";
    return os;
}

template <typename F>
constexpr auto operator^(F, Constant<0, 1>) -> std::enable_if_t<false == detail::is_constant<F>::value, Constant<1>> {
	return Constant<1>{};
}

template <typename F>
constexpr auto operator^(F f, Constant<1, 1>) -> std::enable_if_t<false == detail::is_constant<F>::value, F> {
	return f;
}

template <typename F, Int N>
constexpr auto operator^(F f, Constant<N, 1>) /*-> std::enable_if_t<false == detail::is_constant<F>::value, Power<F, N>>*/ {
	if constexpr (N == 0) {
		return Constant<1>{};
	} else if constexpr (N == 1) {
		return f;
	} else {
		return Power<F, N>{f};
	}
}

template <typename F, Int N, Int M>
constexpr auto operator^(Power<F, N> f, Constant<M, 1>) {
    if constexpr (N == -1) {
        return f.f();
    } else {
        return Power<F, N*M>{f};
    }
}


//
// grouping powers in this way possibly makes the calculation worse...
//
template <typename F, Int N>
constexpr auto operator+(Power<F, N> lhs, F rhs) {
    if constexpr (N == 2) {
        return rhs*(Constant<1>{} + rhs);
    } else {
        return rhs*(Constant<1>{} + Power<F, N - 1>(lhs));
    }
}

template <typename F, Int N>
constexpr auto operator+(F lhs, Power<F, N> rhs) {
    return rhs + lhs;
}

template <typename F, Int N, Int M, typename = ::std::enable_if_t<(N != M)>>
constexpr auto operator+(Power<F, N> lhs, Power<F, M> rhs) {
    if constexpr (N - M == 1) {
        return rhs*(Constant<1>{} + lhs.f());
    } else if constexpr (M - N == 1) {
        return lhs*(Constant<1>{} + rhs.f());
    } else if constexpr (N > M) {
        return rhs*(Constant<1>{} + Power<F, N - M>(lhs));
    } else {
        return lhs*(Constant<1>{} + Power<F, M - N>(rhs));
    }
}

template <typename F, Int N>
constexpr auto operator*(Power<F, N> lhs, F) {
    if constexpr (N == -1) {
        using namespace literals;
        return 1_c;
    } else {
        return Power<F, N + 1>{lhs};
    }
}

template <typename F, Int N>
constexpr auto operator*(F, [[maybe_unused]] Power<F, N> rhs) {
    if constexpr (N == -1) {
        return Constant<1>{};
    } else {
        return Power<F, N + 1>{rhs};
    }
}

template <typename F, Int N>
constexpr auto operator*(Power<F, N> lhs, Power<F, N>) {
    return Power<F, 2 * N>{lhs};
}

template <typename F, Int N, Int M, typename = std::enable_if_t<(N != M) && (N + M != 0)>>
constexpr auto operator*(Power<F, N> lhs, Power<F, M>) {
    if constexpr (N + M == 1) {
        return lhs.f();
    } else {
        return Power<F, N + M>{lhs};
    }
}

template <typename F, Int N, Int M>
constexpr auto operator/(Power<F, N> lhs, Power<F, M> rhs) {
    if constexpr (N == M) {
        return Constant<1>{};
    } else if constexpr (N - M == 1) {
        return lhs.f();
    } else {
        return Power<F, N - M>{lhs};
    }
}

namespace detail {

template <typename F, Int N>
struct is_power<Power<F, N>, F> : std::true_type {};

template <typename F, typename G, Int N>
struct is_power<Power<F, N>, G> : std::bool_constant<util::is_same<F, G>::value> {};

template <typename F, Int N>
struct is_power<F, Power<F, N>> : std::true_type {};

template <typename F, typename G, Int N>
struct is_power<F, Power<G, N>> : std::bool_constant<util::is_same<F, G>::value> {};

template <typename F, Int N, Int M>
struct is_power<Power<F, N>, Power<F, M>> : std::true_type {};

template <typename F, typename G, Int N, Int M>
struct is_power<Power<F, N>, Power<G, M>> : std::bool_constant<util::is_same<F, G>::value> {};

template <typename T, typename U>
struct smallest_power;

template <typename F, Int N>
struct smallest_power<Power<F, N>, F> {
    constexpr static auto value = (N > 1 ? 1 : N);
    static_assert(value != 0);
};

template <typename F, Int N>
struct smallest_power<F, Power<F, N>> : smallest_power<Power<F, N>, F> {
};

template <typename F, Int N, Int M>
struct smallest_power<Power<F, N>, Power<F, M>> {
    constexpr static auto value = (N > M ? M : N);

    static_assert(value != 0);
    static_assert(value != 1);
};


template <typename G, typename P>
struct power_value;

template <typename G, typename P>
struct power_value : std::integral_constant<Int, 0> {};

template <typename G>
struct power_value<G, G> : std::integral_constant<Int, 1> {};

template <typename G, Int N>
struct power_value<G, Power<G, N>> : std::integral_constant<Int, N> {};


template <typename... Ts>
struct contains_power;

template <typename T, typename... Ts>
struct contains_power<T, Addition<Ts...>> {
    static_assert(sizeof...(Ts) >= 0);
    constexpr static auto index = util::tuple_index<std::tuple<Ts...>, T, fields::detail::is_power>::value;
};

template <typename T, typename... Ts>
struct contains_power<T, Multiplication<Ts...>> {
	static_assert(sizeof...(Ts) >= 0);
	constexpr static auto index = util::tuple_index<std::tuple<Ts...>, T, fields::detail::is_power>::value;
};

}   //detail

}   // fields