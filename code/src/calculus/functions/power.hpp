#pragma once

#include "constant.hpp"

#include "../../util/util.hpp"

#include <ostream>

// C headers
#include <cstdint>

namespace fields {

using Int = std::intmax_t;

template <Int A, Int B>
class Constant;

template <typename F, Int N>
class Power;

template <typename F, Int N>
class Power {
    static_assert(std::is_same_v<F, Constant<0>> == false, "0 should not have powers taken! 0^N: -> 0 for N > 0, -> 1 for N = 0 and is undefined for N < 0.");
    static_assert(std::is_same_v<F, Constant<1>> == false, "1 should not have powers taken! 1^N: -> 1 for and N.");

    static_assert(N != 0, "F^0 -> 1");

 public:

     explicit constexpr Power(F f) : f_(f) {
     }

    template <Int M>
    explicit constexpr Power(Power<F, M> f) : f_(f.f_) {
    }

    template <typename... Args>
    constexpr auto operator()(Args... args) const {
        if constexpr ((std::is_arithmetic_v<Args> && ...)) {
            return util::pow(f_(args...), N);
        } else if constexpr (std::is_same_v<decltype(f_(args...)), Constant<0>>) {
            return 0_c;
        } else if constexpr (std::is_same_v<decltype(f_(args...)), Constant<1>>) {
            return 1_c;
        } else {
            return Power<decltype(f_(args...)), N>{f_(args...)};
        }
    }

    constexpr auto f() const { return f_; }

 private:
    template <typename G, Int M> friend class Power;

    F f_;
};

template <typename F, Int N>
auto operator<<(std::ostream& os, Power<F, N> y) -> std::ostream& {
    os << y.f() << "^{" << N << "}";
    return os;
}

namespace operators {

template <typename F>
constexpr auto operator^(F, Constant<0, 1>) { using namespace literals; return 1_c; }

template <typename F>
constexpr auto operator^(F f, Constant<1, 1>) { return f; }

template <typename F, Int N>
constexpr auto operator^(F f, Constant<N, 1>) { return Power<F, N>{f}; }

template <typename F, Int N, Int M>
constexpr auto operator^(Power<F, N> f, Constant<M, 1>) { return Power<F, N*M>{f}; }

// power operator with integers, if rational powers are needed do it later
template <Int A, Int N>
constexpr auto operator^(Constant<A, 1>, Constant<N, 1>) {
    if constexpr (0 == N) {
        // anything to power 0 is 1
        return Constant<1, 1>{};
    } else if constexpr (0 > N) {
        // negative powers mean divide
        return Constant<1, 1>{} / (Constant<A, 1>{}^Constant<-N, 1>{});
    } else {
        // standard recursive power definition, could probably be better
        constexpr auto A_N = util::pow(A, N);

        return Constant<A_N, 1>{};
    }
}

template <Int A, Int B, Int N>
constexpr auto operator^(Constant<A, B>, Constant<N, 1>) { return (Constant<A, 1>{}^Constant<N, 1>{}) / (Constant<B, 1>{}^Constant<N, 1>{}); }

namespace detail {

template <typename T, typename U>
struct is_power : std::false_type {};

template <typename F, Int N>
struct is_power<Power<F, N>, F> : std::true_type {};

template <typename F, Int N>
struct is_power<F, Power<F, N>> : std::true_type {};

template <typename F, Int N, Int M>
struct is_power<Power<F, N>, Power<F, M>> : std::true_type {};

}   //detail

}   // operators
}   // fields