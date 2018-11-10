#pragma once

#include "power.hpp"

#include "../addition.hpp"
#include "../comparison.hpp"

#include "../../util/util.hpp"

// C++ headers
#include <ostream>
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
    Constant() = default;

    template <Int A, Int B>
    explicit constexpr Constant(Constant<A, B>) {
        static_assert((A == Num) && (B == Denom));
    }

    constexpr static auto num = std::ratio<Num, Denom>::num;
    constexpr static auto den = std::ratio<Num, Denom>::den;

    // automatically convert to normal real numbers
    template <typename T>
    constexpr operator T() const { static_assert(std::is_arithmetic_v<T>, "Can only implicitly convert to arithmetic types."); return static_cast<T>(evaluate<T>()); }

    // this is a constant function, whatever arguments we try to pass it, it will return the same number
    template <typename... Ts>
    constexpr auto operator()(Ts...) const { return *this; }

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

template <Int N>
constexpr auto factorial(Constant<N>) {
    static_assert(N >= 0, "Can only take factorial of positive integers.");

    if constexpr (N == 0) {
        return 1_c;
    } else {
        return Constant<util::factorial(N)>{};
    }
}

template <Int A, Int B>
auto operator<<(std::ostream& os, Constant<A, B>) -> std::ostream& {
    if constexpr (B == 1) {
        os << A;
    } else {
        os << "(" << A << "/" << B << ")";
    }

    return os;
}

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
constexpr auto operator "" _c() { return detail::literal_impl<Cs...>(); }

}   // literals

namespace operators {
namespace detail {

template <typename T>
struct is_constant : std::false_type {};

template <Int A, Int B>
struct is_constant<Constant<A, B>> : std::true_type {};

}   //detail
}   // operators

}   // fields
