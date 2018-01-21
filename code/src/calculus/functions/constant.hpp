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

template <Int Num,
          Int Denom = 1>
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

namespace literals {

namespace detail {
template <char C>
constexpr auto convert_char_to_int() {
    constexpr auto I = static_cast<Int>(C - '0');

    static_assert((9 >= I) && (0 <= I), "Characters specified in Constant<N> literal are not 0-9.");
    return I;
}

static_assert(convert_char_to_int<'0'>() == 0, "Could not convert '0' to 0");
static_assert(convert_char_to_int<'1'>() == 1, "Could not convert '1' to 1");
static_assert(convert_char_to_int<'2'>() == 2, "Could not convert '2' to 2");
static_assert(convert_char_to_int<'3'>() == 3, "Could not convert '3' to 3");
static_assert(convert_char_to_int<'4'>() == 4, "Could not convert '4' to 4");
static_assert(convert_char_to_int<'5'>() == 5, "Could not convert '5' to 5");
static_assert(convert_char_to_int<'6'>() == 6, "Could not convert '6' to 6");
static_assert(convert_char_to_int<'7'>() == 7, "Could not convert '7' to 7");
static_assert(convert_char_to_int<'8'>() == 8, "Could not convert '8' to 8");
static_assert(convert_char_to_int<'9'>() == 9, "Could not convert '9' to 9");

template <char... Cs>
constexpr auto convert_chars_to_ints() {
    return std::integer_sequence<Int, convert_char_to_int<Cs>()...>{}; }

template <Int... Powers>
constexpr auto evaluate_powers_of_10(std::integer_sequence<Int, Powers...>) {
    constexpr auto N = static_cast<Int>(sizeof...(Powers) - 1);
    return std::integer_sequence<Int, util::pow(static_cast<Int>(10), static_cast<Int>(N - Powers))...>{};
}

template <std::size_t N>
constexpr auto create_powers_of_10() {
    return evaluate_powers_of_10(std::make_integer_sequence<Int, N>{});
}

template <Int... Digits, Int... Powers>
constexpr auto inner_product(std::integer_sequence<Int, Digits...>, std::integer_sequence<Int, Powers...>) {
    return std::integer_sequence<Int, Digits*Powers...>{};
}

template <Int... Coefficients>
constexpr auto sum_coefficients(std::integer_sequence<Int, Coefficients...>) {
    constexpr auto N = (... + Coefficients);
    return Constant<N>{};
}

template <char... Cs>
constexpr auto literal_impl() {
    constexpr auto powers = create_powers_of_10<sizeof...(Cs)>();
    constexpr auto ints = convert_chars_to_ints<Cs...>();

    return sum_coefficients(inner_product(ints, powers));
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