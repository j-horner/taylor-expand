#pragma once

// C++ headers
#include <ratio>

// C headers
#include <cstdint>

namespace fields {

// A class to represent constant functions
// Basically std::ratio with a few extra things

using Int = std::intmax_t;

template <Int Num,
          Int Denom = 1>
class Constant : std::ratio<Num, Denom> {
 public:
    constexpr static auto num = std::ratio<Num, Denom>::num;
    constexpr static auto den = std::ratio<Num, Denom>::den;

    // automatically convert to normal real numbers
    template <typename T>
    constexpr static auto operator T() const { return static_cast<T>(evaluate<T>()); }

    // this is a constant function, whatever arguments we try to pass it, it will return the same number
    template <typename Ts...>
    constexpr static auto operator(Ts...) const {
        return evaluate<double>();
    }

 private:
    template <typename T>
    constexpr static auto evaluate() const {
        if constexpr (1 == den) {
            // the constant is an integer, maintain precision
            return N;
        } else {
            static_assert(0 != (num % den), "std::ratio should have reduced the fraction!");
            // otherwise return the closest real value
            return static_cast<T>(num)/static_cast<T>(den);
        }
    }
};

namespace operators {

// TODO(jsh) : need to add comparison operators

// all constants have 0 derivative
template <Int A, Int B>
constexpr auto d_dx(Constant<A, B>) { return Constant<0>{}; }

// ---------------------------------------------------------------------------------
// define compile time arthimetic i.e. +,-,*,/ operators for Constant<N, D>
template <Int A, Int B, Int C, Int D>
constexpr auto operator+(Constant<A, B>, Constant<C, D>) {
    using R = std::ratio_add<std::ratio<A, B>, std::ratio<C, D>>;
    return Constant<R::num, R::den>{};
}

template <Int A, Int B, Int C, Int D>
constexpr auto operator-(Constant<A, B>, Constant<C, D>) {
    using R = std::ratio_subtract<std::ratio<A, B>, std::ratio<C, D>>;
    return Constant<R::num, R::den>{};
}

template <Int A, Int B, Int C, Int D>
constexpr auto operator*(Constant<A, B>, Constant<C, D>) {
    using R = std::ratio_multiply<std::ratio<A, B>, std::ratio<C, D>>;
    return Constant<R::num, R::den>{};
}

template <Int A, Int B, Int C, Int D>
constexpr auto operator/(Constant<A, B>, Constant<C, D>) {
    using R = std::ratio_divide<std::ratio<A, B>, std::ratio<C, D>>;
    return Constant<R::num, R::den>{};
}
// ---------------------------------------------------------------------------------

// -(a/b) -> (-a)/b
template <Int A, Int B>
constexpr auto operator-(Constant<A, B>) { return Constant<-A, B>{}; }

// ---------------------------------------------------------------------------------
// power operator with integers, if rational powers are needed do it later
template <Int A, Int N>
constexpr auto operator^(Constant<A>, Constant<N>) {
    if constexpr (0 == N) {
        // anything to power 0 is 1
        return Constant<1>{};
    } else if constexpr (0 > N) {
        // negative powers mean divide
        return Constant<1>{} / (Constant<A>{}^Constant<-N>{});
    } else {
        // standard recursive power definition, could probably be better
        return Constant<A>{}*(Constant<A>{}^Constant<N - 1>{});
    }
}

template <Int A, Int B, Int N>
constexpr auto operator^(Constant<A, B>, Constant<N>) { return (Constant<A>{}^Constant<N>{}) / (Constant<B>{}^Constant<N>{}); }
// ---------------------------------------------------------------------------------

// anything multiplied by 0 is 0
template <typename F> constexpr auto operator*(F&&, Constant<0>) { return Constant<0>{}; }
template <typename F> constexpr auto operator*(Constant<0>, F&&) { return Constant<0>{}; }

// adding 0 to something does nothing
template <typename F> constexpr auto operator+(F&& f, Constant<0>) { return std::forward<F>(f); }
template <typename F> constexpr auto operator+(Constant<0>, F&& f) { return std::forward<F>(f); }

// multiplying by 1 does nothing
template <typename F> constexpr auto operator*(F&& f, Constant<1>) { return std::forward<F>(f); }
template <typename F> constexpr auto operator*(Constant<1>, F&& f) { return std::forward<F>(f); }

}   // operators
}   // fields