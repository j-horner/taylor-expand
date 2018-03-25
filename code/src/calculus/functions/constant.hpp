#pragma once

#include "../../util/util.hpp"

// C++ headers
#include <ratio>
#include <utility>

// C headers
#include <cstdint>

namespace fields {

// A class to represent constant functions
// Basically std::ratio with a few extra things

using Int = std::intmax_t;

template <Int Num, Int Denom = 1>
class Constant : std::ratio<Num, Denom> {
 public:
    constexpr static auto num = std::ratio<Num, Denom>::num;
    constexpr static auto den = std::ratio<Num, Denom>::den;

    // automatically convert to normal real numbers
    template <typename T>
    constexpr operator T() const { return static_cast<T>(evaluate<T>()); }

    // this is a constant function, whatever arguments we try to pass it, it will return the same number
    template <typename... Ts>
    constexpr auto operator()(Ts...) const {
        return evaluate<double>();
    }

 private:
    template <typename T>
    constexpr static auto evaluate() {
        if constexpr ((1 == den) || ((num % den) == 0)) {
            return num/den;     // the constant is an integer, maintain precision
        } else {
            return static_cast<T>(num)/static_cast<T>(den); // otherwise return the closest real value
        }
    }
};

namespace literals {

namespace detail {
template <char C>
constexpr auto convert_char_to_int();

template <>
constexpr auto convert_char_to_int<'.'>() { return std::integral_constant<char, '.'>{}; }

template <char C>
constexpr auto convert_char_to_int() {
    constexpr static auto I = static_cast<Int>(C - '0');   // convert a char digit into it's numerical equivalent

    static_assert((9 >= I) && (0 <= I), "Characters specified in Constant<N> literal are not 0-9.");
    return Constant<I>{};
}

constexpr auto combine_decimals() { return Constant<0>{}; }

template <Int A>
constexpr auto combine_decimals(Constant<A>) { return Constant<A>{}; }

template <Int A, Int B, typename... Cs>
constexpr auto combine_decimals(Constant<A, B>, Cs... digits) {
    return Constant<A, B>{} + Constant<1, 10>{}*combine_decimals(digits...);
}

template <Int A, Int B>
constexpr auto combine(Constant<A, B>) { return Constant<A, B>{}; }

template <Int A, Int B, typename... Cs>
constexpr auto combine(Constant<A, B>, std::integral_constant<char, '.'>, Cs... digits) {
    return Constant<A, B>{} + Constant<1, 10>{}*combine_decimals(digits...);
}

template <Int A, Int B, Int C, typename... Cs>
constexpr auto combine(Constant<A, B>, Constant<C>, Cs... digits) {
    return combine(Constant<10>{}*Constant<A, B>{} + Constant<C>{}, digits...);
}

template <char... Cs>
constexpr auto literal_impl() {
    return combine(convert_char_to_int<Cs>()...);
}

}   //detail

template <char... Cs>
constexpr auto operator "" _c() {
    return detail::literal_impl<Cs...>();
}

}   // literals


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
        constexpr auto A_N = util::pow(A, N);

        return Constant<A_N>{};
    }
}

template <Int A, Int B, Int N>
constexpr auto operator^(Constant<A, B>, Constant<N>) { return (Constant<A>{}^Constant<N>{}) / (Constant<B>{}^Constant<N>{}); }
// ---------------------------------------------------------------------------------

// anything multiplied by 0 is 0
// template <typename F> constexpr auto operator*(F, Constant<0>) { return Constant<0>{}};
// template <typename F> constexpr auto operator*(Constant<0>, F) { return Constant<0>{}};

// adding 0 to something does nothing
// template <typename F> constexpr auto operator+(F f, Constant<0>) { return f; }
// template <typename F> constexpr auto operator+(Constant<0>, F f) { return f; }

// multiplying by 1 does nothing
// template <typename F> constexpr auto operator*(F f, Constant<1>) { return f; }
// template <typename F> constexpr auto operator*(Constant<1>, F f) { return f; }

// ---------------------------------------------------------------------------------
// comparison operators
template <Int A, Int B, Int C, Int D>
constexpr auto operator==(Constant<A, B> a, Constant<C, D> b) { return std::is_same_v<decltype(a), decltype(b)>;  }

template <Int A, Int B, typename T>
constexpr auto operator==(Constant<A, B> a, T b) { return static_cast<T>(a) == b; }

template <Int A, Int B, typename T>
constexpr auto operator==(T b, Constant<A, B> a) { return a == b; }

}   // operators
}   // fields
