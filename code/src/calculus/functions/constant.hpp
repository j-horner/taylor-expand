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
		return Constant<1>{};
    } else {
        return Constant<util::factorial(N)>{};
    }
}

template <typename Stream, Int A, Int B>
auto& operator<<(Stream& os, Constant<A, B>) {
    if constexpr (B == 1) {
        os << A;
    } else {
        os << "(" << A << "/" << B << ")";
    }

    return os;
}

template <Int A, Int B, Int C, Int D>
constexpr auto operator==(Constant<A, B>, Constant<C, D>) {
	return std::ratio_equal_v<std::ratio<A, B>, std::ratio<C, D>>;
}

template <typename T, Int A, Int B>
constexpr auto operator==(T lhs, Constant<A, B> rhs) -> std::enable_if_t<std::is_arithmetic_v<T>, T> {
	return lhs == static_cast<T>(rhs);
}

template <typename T, Int A, Int B>
constexpr auto operator==(Constant<A, B> lhs, T rhs) -> std::enable_if_t<std::is_arithmetic_v<T>, T> {
	return static_cast<T>(lhs) == rhs;
}


template <Int A, Int B>
constexpr auto operator+(Constant<A, B>, Constant<A, B>) {
	using R = std::ratio_add<std::ratio<A, B>, std::ratio<A, B>>;
	return Constant<R::num, R::den>{};
}

template <Int A, Int B, Int C, Int D>
constexpr auto operator+(Constant<A, B>, Constant<C, D>) {
	using R = std::ratio_add<std::ratio<A, B>, std::ratio<C, D>>;
	return Constant<R::num, R::den>{};
}

template <typename T, Int A, Int B>
constexpr auto operator+(T lhs, Constant<A, B> rhs) -> std::enable_if_t<std::is_arithmetic_v<T>, T> {
	return lhs + static_cast<T>(rhs);
}

template <typename T, Int A, Int B>
constexpr auto operator+(Constant<A, B> lhs, T rhs) -> std::enable_if_t<std::is_arithmetic_v<T>, T> {
	return static_cast<T>(lhs) + rhs;
}

template <Int A, Int B>
constexpr auto operator-(Constant<A, B>) {
	return Constant<-A, B>{};
}

template <Int A, Int B, Int C, Int D>
constexpr auto operator-(Constant<A, B>, Constant<C, D>) {
	using R = std::ratio_subtract<std::ratio<A, B>, std::ratio<C, D>>;
	return Constant<R::num, R::den>{};
}

template <Int A, Int B>
constexpr auto operator-(Constant<A, B>, Constant<A, B>) {
	return Constant<0>{};
}

template <typename T, Int A, Int B>
constexpr auto operator-(T lhs, Constant<A, B> rhs) -> std::enable_if_t<std::is_arithmetic_v<T>, T> {
	return lhs - static_cast<T>(rhs);
}

template <typename T, Int A, Int B>
constexpr auto operator-(Constant<A, B> lhs, T rhs) -> std::enable_if_t<std::is_arithmetic_v<T>, T> {
	return static_cast<T>(lhs) - rhs;
}

template <Int A, Int B, Int C, Int D>
constexpr auto operator*(Constant<A, B>, Constant<C, D>) {
	using R = std::ratio_multiply<std::ratio<A, B>, std::ratio<C, D>>;
	return Constant<R::num, R::den>{};
}

template <Int A, Int B>
constexpr auto operator*(Constant<A, B>, Constant<A, B>) {
	using R = std::ratio_multiply<std::ratio<A, B>, std::ratio<A, B>>;
	return Constant<R::num, R::den>{};
}

template <typename T, Int A, Int B>
constexpr auto operator*(T lhs, Constant<A, B> rhs) -> std::enable_if_t<std::is_arithmetic_v<T>, T> {
	return lhs*static_cast<T>(rhs);
}

template <typename T, Int A, Int B>
constexpr auto operator*(Constant<A, B> lhs, T rhs) -> std::enable_if_t<std::is_arithmetic_v<T>, T> {
	return static_cast<T>(lhs)*rhs;
}

template <Int A, Int B, Int C, Int D>
constexpr auto operator/(Constant<A, B>, Constant<C, D>) {
	static_assert(C != 0, "Attempting to divide by 0!");
	using R = std::ratio_divide<std::ratio<A, B>, std::ratio<C, D>>;
	return Constant<R::num, R::den>{};
}

template <typename T, Int A, Int B>
constexpr auto operator/(T lhs, Constant<A, B> rhs) -> std::enable_if_t<std::is_arithmetic_v<T>, T> {
	return lhs/static_cast<T>(rhs);
}

template <typename T, Int A, Int B>
constexpr auto operator/(Constant<A, B> lhs, T rhs) -> std::enable_if_t<std::is_arithmetic_v<T>, T> {
	return static_cast<T>(lhs)/rhs;
}

// power operator with integers, if rational powers are needed do it later
template <Int A, Int N>
constexpr auto operator^(Constant<A, 1>, Constant<N, 1>) {
	if constexpr (0 == N) {
		// anything to power 0 is 1
		return Constant<1>{};
	}
	else if constexpr (-1 == N) {
		return Constant<1>{} / Constant<A>{};
	}
	else if constexpr (0 > N) {
		// negative powers mean divide
		return Constant<1>{} / (Constant<A>{}^ Constant<-N>{});
	}
	else {
		// standard recursive power definition, could probably be better
		constexpr auto A_N = util::pow(A, N);

		return Constant<A_N>{};
	}
}

template <Int A, Int B, Int N>
constexpr auto operator^(Constant<A, B>, Constant<N, 1>) {
	return (Constant<A>{}^ Constant<N>{}) / (Constant<B>{}^ Constant<N>{});
}


namespace literals {
namespace detail {

template <char C>
constexpr auto convert_char_to_int();

template <>
constexpr auto convert_char_to_int<'.'>() { return std::integral_constant<char, '.'>{}; }

template <char C>
constexpr auto convert_char_to_int() {
    constexpr auto I = static_cast<Int>(C - '0');   // convert a char digit into it's numerical equivalent

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

namespace detail {

template <typename T>
struct is_constant : std::false_type {};

template <Int A, Int B>
struct is_constant<Constant<A, B>> : std::true_type {};

}   //detail
}   // fields
