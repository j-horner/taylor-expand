#pragma once

#include "functions/constant.hpp"
#include "multiplication.hpp"

// C++ headers
#include <type_traits>

// C headers
#include <cstdint>

namespace fields {

using Int = std::intmax_t;

template <Int A, Int B>
class Constant;

namespace detail {
template <typename T> struct is_constant;
}

namespace operators {

template <typename F, typename G>
class Division {
 public:
    template <typename T>
    constexpr auto operator()(T x) const { return lhs(x)/rhs(x); }

    F lhs;
    G rhs;

    static_assert(detail::is_constant<std::decay_t<G>>::value == false, "Division by constants should never occur as they can always be cast as a multiplication.");
};



// -------------------------------------------------------------------------------------------------
//                                      / operations
// -------------------------------------------------------------------------------------------------

template <typename... Fs>
class Addition;

template <typename F, typename G>
class Subtraction;

template <typename... Fs>
class Multiplication;

// Multiplication with common factors
template <typename F, typename G>
constexpr auto operator/(Multiplication<G, F> m, F) { return m.get<0>(); }

template <typename F, typename G>
constexpr auto operator/(F, Multiplication<G, F> m) { return 1_c/m.get<0>(); }

template <typename F, typename... Gs>
constexpr auto operator/(Multiplication<F, Gs...> m, F) { return m.sub_product<1, sizeof...(Gs) + 1>(); }

template <typename F, typename... Gs>
constexpr auto operator/(F, Multiplication<F, Gs...> m) { return 1_c/m.sub_product<1, sizeof...(Gs) + 1>(); }

template <typename F, typename... Gs>
constexpr auto operator/(Multiplication<F, Gs...> lhs, Multiplication<Gs...>) { return lhs.get<0>(); }

template <typename F, typename... Gs>
constexpr auto operator/(Multiplication<Gs...>, Multiplication<F, Gs...> rhs) { return 1_c/rhs.get<0>(); }

template <typename F, typename G, typename... Gs>
constexpr auto operator/(Multiplication<G, F> lhs, Multiplication<F, Gs...> rhs) { return lhs.get<0>()/rhs.sub_product<1, sizeof...(Gs) + 1>(); }

template <typename F, typename G, typename... Gs>
constexpr auto operator/(Multiplication<F, Gs...> lhs, Multiplication<G, F> rhs) { return lhs.sub_product<1, sizeof...(Gs) + 1>()/rhs.get<0>(); }

template <typename F, typename G, typename H>
constexpr auto operator/(Multiplication<G, F> lhs, Multiplication<H, F> rhs) { return lhs.get<0>()/rhs.get<0>(); }

// Division with Multiplication
template <typename F, typename G, typename... Gs>
constexpr auto operator/(Multiplication<Gs...> lhs, Division<F, G> rhs) { return lhs*rhs.rhs/rhs.lhs; }

template <typename F, typename G, typename... Gs>
constexpr auto operator/(Division<F, G> lhs, Multiplication<Gs...> rhs) { return lhs.lhs/(lhs.rhs*rhs); }

// Division with common factors
template <typename F, typename G, typename H>
constexpr auto operator/(Division<F, G> lhs, Division<F, H> rhs) { return rhs.rhs/lhs.rhs; }

template <typename F, typename G, typename H>
constexpr auto operator/(Division<G, F> lhs, Division<H, F> rhs) { return lhs.lhs/rhs.lhs; }

// Division with unrelated factors
template <typename A, typename B, typename C, typename D>
constexpr auto operator/(Division<A, B> lhs, Division<C, D> rhs) { return (lhs.lhs*rhs.rhs)/(lhs.rhs*rhs.lhs); }

template <Int A, Int B, Int C, Int D>
constexpr auto operator/(Constant<A, B>, Constant<C, D>) {
    static_assert(C != 0, "Attempting to divide by 0!");
    using R = std::ratio_divide<std::ratio<A, B>, std::ratio<C, D>>;
    return Constant<R::num, R::den>{};
}

// division by constant is equivalent to multiplying by inverse
template <typename F, Int A, Int B>
constexpr auto operator/(F f, Constant<A, B> c) { return (1_c/c)*f; }

template <typename F, typename G>
constexpr auto operator/(F lhs, G rhs) {
    static_assert(false == std::is_same_v<decltype(rhs), Constant<0>>, "Cannot divide by 0!");

    if constexpr (std::is_same_v<decltype(lhs), Constant<0>>) {
        using namespace literals;
        return 0_c;
    } else {
        return Division<F, G>{lhs, rhs};
    }
}

} // operators
} // fields
