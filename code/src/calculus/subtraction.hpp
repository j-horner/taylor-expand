#pragma once

#include "functions/constant.hpp"

#include "addition.hpp"
#include "multiplication.hpp"

#include <ostream>

// C headers
#include <cstdint>

namespace fields {

using Int = std::intmax_t;

template <Int A, Int B>
class Constant;

namespace operators {

template <typename F, typename G>
class Subtraction {
public:
    template <typename... Args>
    constexpr auto operator()(Args... args) const { return lhs(args...) - rhs(args...); }

    F lhs;
    G rhs;
};

template <typename F, typename G>
auto operator<<(std::ostream& os, Subtraction<F, G> y) -> std::ostream& {
    os << "(" << y.lhs << " - " << y.rhs << ")";
    return os;
}

// -------------------------------------------------------------------------------------------------
//                                      - operations
// -------------------------------------------------------------------------------------------------

template <typename... Fs>
class Addition;

template <typename... Fs>
class Multiplication;

template <typename F, typename G>
class Division;

// -f = (-1)*f
template <Int A, Int B>
constexpr auto operator-(Constant<A, B>) { return Constant<-A, B>{}; }

template <typename F>
constexpr auto operator-(F f) {
    using namespace literals;
    return -1_c*f;
}

// Definition of 0
template <typename F>
constexpr auto operator-(F, F) {
    using namespace literals;
    return 0_c;
}

// Addition with common factors
template <typename F, typename G>
constexpr auto operator-(Addition<G, F> y, F) { return y.get<0>(); }

template <typename F, typename G>
constexpr auto operator-(F, Addition<G, F> y) { return -y.get<0>(); }

template <typename F, typename... Gs>
constexpr auto operator-(Addition<F, Gs...> y, F) { return y.sub_sum<1, sizeof...(Gs) + 1>(); }

template <typename F, typename... Gs>
constexpr auto operator-(F, Addition<F, Gs...> y) { return -y.sub_sum<1, sizeof...(Gs) + 1>(); }

template <typename F, typename... Gs>
constexpr auto operator-(Addition<F, Gs...> lhs, Addition<Gs...> rhs) { return lhs.get<0>(); }

template <typename F, typename... Gs>
constexpr auto operator-(Addition<Gs...> lhs, Addition<F, Gs...> rhs) { return -rhs.get<0>(); }

template <typename F, typename G, typename... Gs>
constexpr auto operator-(Addition<G, F> lhs, Addition<F, Gs...> rhs) { return lhs.get<0>() - rhs.sub_sum<1, sizeof...(Gs) + 1>(); }

template <typename F, typename G, typename... Gs>
constexpr auto operator-(Addition<F, Gs...> lhs, Addition<G, F> rhs) { return lhs.sub_sum<1, sizeof...(Gs) + 1>() - rhs.get<0>(); }

template <typename F, typename G, typename H>
constexpr auto operator-(Addition<G, F> lhs, Addition<H, F> rhs) { return lhs.get<0>() - rhs.get<0>(); }

// Subtractions with common factors
template <typename F, typename G>
constexpr auto operator-(Subtraction<G, F> y, F f) {
    using namespace literals;
    return y.lhs - 2_c*f;
}

template <typename F, typename G>
constexpr auto operator-(F f, Subtraction<G, F> y) {
    using namespace literals;
    return 2_c*f - y.lhs;
}

template <typename F, typename G>
constexpr auto operator-(Subtraction<F, G> y, F) { return -y.rhs; }

template <typename F, typename G>
constexpr auto operator-(F, Subtraction<F, G> y) { return y.rhs; }

// Subtractions with unrelated factors
template <typename F, typename G, typename H>
constexpr auto operator-(Subtraction<F, G> lhs, H rhs) { return lhs.lhs - (lhs.rhs + rhs); }

template <typename F, typename G, typename H>
constexpr auto operator-(H lhs, Subtraction<F, G> rhs) { return (lhs + rhs.rhs) - rhs.lhs; }

template <typename A, typename B, typename C, typename D>
constexpr auto operator-(Subtraction<A, B> lhs, Subtraction<C, D> rhs) { return (lhs.lhs + rhs.rhs) - (lhs.rhs + rhs.lhs); }

// Mulitplications with common factors
template <typename F, typename G>
constexpr auto operator-(Multiplication<G, F> y, F f) {
    using namespace literals;
    return (y.get<0>() - 1_c)*f;
}

template <typename F, typename G>
constexpr auto operator-(F f, Multiplication<G, F> y) {
    using namespace literals;
    return (1_c - y.get<0>())*f;
}

template <typename F, typename... Gs>
constexpr auto operator-(Multiplication<F, Gs...> y, F f) {
    using namespace literals;
    return f*(y.sub_product<1, sizeof...(Gs) + 1>() - 1_c);
}

template <typename F, typename... Gs>
constexpr auto operator-(F f, Multiplication<F, Gs...> y) {
    using namespace literals;
    return f*(1_c - y.sub_product<1, sizeof...(Gs) + 1>());
}

template <typename F, typename... Gs>
constexpr auto operator-(Multiplication<Gs...> lhs, Multiplication<F, Gs...> rhs) {
    using namespace literals;
    return (1_c - rhs.get<0>())*lhs;
}

template <typename F, typename... Gs>
constexpr auto operator-(Multiplication<F, Gs...> lhs, Multiplication<Gs...> rhs) {
    using namespace literals;
    return (lhs.get<0>() - 1_c)*rhs;
}

// Division with common factors
template <typename F, typename G>
constexpr auto operator-(Division<F, G> y, F f) {
    using namespace literals;
    return f*(1_c - y.rhs)/y.rhs;
}

template <typename F, typename G>
constexpr auto operator-(F f, Division<F, G> y) {
    using namespace literals;
    return f*(y.rhs - 1_c)/y.rhs;
}

// Division with unrelated factors
template <typename F, typename G, typename H>
constexpr auto operator-(Division<F, G> lhs, H rhs) { return (lhs.lhs - lhs.rhs*rhs)/lhs.rhs; }

template <typename F, typename G, typename H>
constexpr auto operator-(H lhs, Division<F, G> rhs) { return (lhs*rhs.rhs - rhs.lhs)/rhs.rhs; }

template <typename A, typename B, typename C, typename D>
constexpr auto operator-(Division<A, B> lhs, Division<C, D> rhs) { return (lhs.lhs*rhs.rhs - lhs.rhs*rhs.lhs)/(lhs.rhs*rhs.rhs); }

template <Int A, Int B, Int C, Int D>
constexpr auto operator-(Constant<A, B>, Constant<C, D>) {
    using R = std::ratio_subtract<std::ratio<A, B>, std::ratio<C, D>>;
    return Constant<R::num, R::den>{};
}

template <Int A, Int B>
constexpr auto operator-(Constant<A, B>, Constant<A, B>) {
    using namespace literals;
    return 0_c;
}

// Main Subtraction definition
template <typename F, typename G>
constexpr auto operator-(F lhs, G rhs) {
    static_assert(sizeof(lhs) > 0, "silence unused variable warning");
    static_assert(sizeof(rhs) > 0, "silence unused variable warning");
    
    if constexpr (std::is_same_v<F, Constant<0>>) {
        return -rhs;
    } else if constexpr (std::is_same_v<G, Constant<0>>) {
        return lhs;
    } else if constexpr (std::is_arithmetic_v<G> && detail::is_constant<F>::value) {
        return static_cast<G>(lhs) - rhs;
    } else if constexpr (std::is_arithmetic_v<F> && detail::is_constant<G>::value) {
        return lhs - static_cast<F>(rhs);
    } else {
        return Subtraction<F, G>{lhs, rhs};
    }
}

} // operators
} // fields
