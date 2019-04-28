#pragma once

#include "multiplication.hpp"
#include "subtraction.hpp"

#include "functions/constant.hpp"

// C++ headers
#include <ostream>
#include <tuple>

namespace fields {

using Int = std::intmax_t;

template <Int A, Int B>
class Constant;

namespace operators {

template <typename... Fs>
class Addition {
 public:
    constexpr Addition() = default;

    template <typename... Ts, typename G>
    constexpr Addition(G lhs, Addition<Ts...> rhs) : fs(std::tuple_cat(std::make_tuple(lhs), rhs.fs)) {
    }

    template <typename... Ts, typename G>
    constexpr Addition(Addition<Ts...> lhs, G rhs) : fs(std::tuple_cat(lhs.fs, std::make_tuple(rhs))) {
    }

    template <typename... Ts, typename... Us>
    constexpr Addition(Addition<Ts...> lhs, Addition<Us...> rhs) : fs(std::tuple_cat(lhs.fs, rhs.fs)) {
    }

    template <typename F, typename G>
    constexpr Addition(F lhs, G rhs) : fs(std::make_tuple(lhs, rhs)) {
    }

    template <typename... Args>
    constexpr auto operator()(Args... args) const { return sum_impl(std::make_index_sequence<N>{}, std::make_tuple(args...)); }

    template <std::size_t I>
    constexpr auto get() const { return std::get<I>(fs); }

    template <typename F>
    constexpr auto get() const { return std::get<F>(fs); }

    template <std::size_t I, std::size_t J>
    constexpr auto sub_sum() const {
        static_assert((I <= N) && (J <= N) && (I <= J), "Expect following conditions on I, J: (I <= N) && (J <= N) && (I <= J)");
        return subset_impl<I>(std::make_index_sequence<J - I>{});
    }

     template <typename D>
     constexpr auto derivative(D d) const { return derivative_impl(d, std::make_index_sequence<N>{}); }

 private:
    template <typename... Gs> friend class Addition;

    explicit constexpr Addition(std::tuple<Fs...> ts) : fs(ts) {
    }

    template <std::size_t... Is, typename Tuple>
    constexpr auto sum_impl(std::index_sequence<Is...>, Tuple args) const { return (std::apply(get<Is>(), args) + ...); }

    template <typename D, std::size_t... Is>
    constexpr auto derivative_impl(D d, std::index_sequence<Is...>) const { return ((d(get<Is>())) + ...); }

    template <std::size_t I, std::size_t... Is>
    constexpr auto subset_impl(std::index_sequence<Is...>) const {
        if constexpr (sizeof...(Is) == 1) {
            return get<I>();
        } else {
            return Addition<std::decay_t<decltype(get<I + Is>())>...>(std::make_tuple(get<I + Is>()...));
        }
    }

    std::tuple<Fs...> fs;

    constexpr static auto N = sizeof...(Fs);
};

namespace detail {

template <typename T, typename... Rest>
auto print(std::ostream& os, T first, Rest... Rest) -> void;

template <typename T>
auto print(std::ostream& os, T last) -> void {
    os << last;
}

template <typename T, typename... Rest>
auto print(std::ostream& os, T first, Rest... rest) -> void {
    os << first << " + ";
    print(os, rest...);
}

}

template <typename... Fs>
auto operator<<(std::ostream& os, Addition<Fs...> y) -> std::ostream& {
    os << "(";
    detail::print(os, y.get<Fs>()...);
    os << ")";

    return os;
}


// -------------------------------------------------------------------------------------------------
//                                      + operations
// -------------------------------------------------------------------------------------------------

template <typename F, typename G>
class Subtraction;

template <typename... Fs>
class Multiplication;

template <typename F, typename G>
class Division;

// Definition of number 2
template <typename F>
constexpr auto operator+(F rhs, F) {
    using namespace literals;
    return 2_c*rhs;
}

// Addiions with common factors
template <typename F, typename G>
constexpr auto operator+(Addition<G, F> y, F f) {
    using namespace literals;
    return y.get<0>() + 2_c*f;
}

template <typename F, typename G>
constexpr auto operator+(F f, Addition<G, F> y) {
    using namespace literals;
    return y.get<0>() + 2_c*f;
}

template <typename F, typename... Gs>
constexpr auto operator+(Addition<F, Gs...> y, F f) {
    using namespace literals;
    return y.sub_sum<1, sizeof...(Gs) + 1>() + 2_c*f;
}

template <typename F, typename... Gs>
constexpr auto operator+(F f, Addition<F, Gs...> y) {
    using namespace literals;
    return y.sub_sum<1, sizeof...(Gs) + 1>() + 2_c*f;
}

// Additions with unrelated factors
template <typename... Fs, typename G>
constexpr auto operator+(G g, Addition<Fs...> f) {
    return f + g;
}

template <typename G, typename... Fs>
constexpr auto operator+(Addition<Fs...> f, G g) {
    if constexpr (std::is_same_v<G, Constant<0>>) {
        return f;
    } else {
        constexpr static auto match_found = (util::is_same<Fs, G>::value || ...);

        if constexpr (false == match_found) {
            constexpr static auto multiple_found = (detail::is_multiple<Fs, G>::value || ...);

            if constexpr (false == multiple_found) {
                return Addition<Fs..., G>(f, g);
            } else {
                return f.get<0>() + (f.sub_sum<1, sizeof...(Fs)>() + g);
            }
        } else {
            return f.get<0>() + (f.sub_sum<1, sizeof...(Fs)>() + g);
        }
    }
}

template <typename... Fs, typename... Gs>
constexpr auto operator+(Addition<Fs...> lhs, Addition<Gs...> rhs) {
    if constexpr (util::is_same<Addition<Fs...>, Addition<Gs...>>::value) {
        using namespace literals;
        return 2_c*lhs;
    } else {
        return (lhs + rhs.get<0>()) + rhs.sub_sum<1, sizeof...(Gs)>();
    }
}

// Subtractions with common factors
template <typename F, typename G>
constexpr auto operator+(Subtraction<G, F> y, F) { return y.lhs; }

template <typename F, typename G>
constexpr auto operator+(F f, Subtraction<G, F> y) { return y.lhs; }

template <typename F, typename G>
constexpr auto operator+(Subtraction<F, G> y, F f) {
    using namespace literals;
    return 2_c*f - y.rhs;
}

template <typename F, typename G>
constexpr auto operator+(F f, Subtraction<F, G> y) {
    using namespace literals;
    return 2_c*f - y.rhs;
}

template <typename F, typename G, typename H>
constexpr auto operator+(Addition<G, F> lhs, Subtraction<H, F> rhs) { return lhs.get<0>() + rhs.lhs; }

template <typename F, typename G, typename H>
constexpr auto operator+(Subtraction<H, F> lhs, Addition<G, F> rhs) { return lhs.lhs + rhs.get<0>(); }

template <typename F, typename G, typename... Gs>
constexpr auto operator+(Addition<F, Gs...> lhs, Subtraction<G, F> rhs) { return lhs.sub_sum<1, sizeof...(Gs) + 1>() + rhs.lhs; }

template <typename F, typename G, typename... Gs>
constexpr auto operator+(Subtraction<G, F> lhs, Addition<F, Gs...> rhs) { return lhs.lhs + rhs.sub_sum<1, sizeof...(Gs) + 1>(); }

template <typename F, typename G, typename H>
constexpr auto operator+(Subtraction<G, F> lhs, Subtraction<F, H> rhs) { return lhs.lhs - rhs.rhs; }

template <typename F, typename G, typename H>
constexpr auto operator+(Subtraction<F, G> lhs, Subtraction<H, F> rhs) { return rhs.lhs - lhs.rhs; }

// Subtractions with unrelated factors
template <typename F, typename G, typename H>
constexpr auto operator+(Subtraction<F, G> lhs, H rhs) { return (lhs.lhs + rhs) - lhs.rhs; }

template <typename F, typename G, typename H>
constexpr auto operator+(H lhs, Subtraction<F, G> rhs) { return (lhs + rhs.lhs) - rhs.rhs; }

template <typename A, typename B, typename C, typename D>
constexpr auto operator+(Subtraction<A, B> lhs, Subtraction<C, D> rhs) { return (lhs.lhs + rhs.lhs) - (lhs.rhs + rhs.rhs); }

// Multiplication with common factors
template <typename F, typename G>
constexpr auto operator+(Multiplication<G, F> y, F f) {
    using namespace literals;
    return (y.get<0>() + 1_c)*f;
}

template <typename F, typename G>
constexpr auto operator+(F f, Multiplication<G, F> y) {
    using namespace literals;
    return (1_c + y.get<0>())*f;
}

template <typename F, typename... Gs>
constexpr auto operator+(Multiplication<F, Gs...> y, F f) {
    using namespace literals;
    return f*(y.sub_product<1, sizeof...(Gs) + 1>() + 1_c);
}

template <typename F, typename... Gs>
constexpr auto operator+(F f, Multiplication<F, Gs...> y) {
    using namespace literals;
    return f*(1_c + y.sub_product<1, sizeof...(Gs) + 1>());
}

template <typename F, typename... Gs>
constexpr auto operator+(Multiplication<Gs...> lhs, Multiplication<F, Gs...> rhs) {
    using namespace literals;
    return (1_c + rhs.get<0>())*lhs;
}

template <typename F, typename... Gs>
constexpr auto operator+(Multiplication<F, Gs...> lhs, Multiplication<Gs...> rhs) {
    using namespace literals;
    return (lhs.get<0>() + 1_c)*rhs;
}

template <typename F, typename G, typename... Gs>
constexpr auto operator+(Multiplication<F, Gs...> lhs, Multiplication<G, Gs...> rhs) {
    return (lhs.get<0>() + rhs.get<0>())*lhs.sub_product<1, sizeof...(Gs) + 1>();
}

template <typename F, typename... Gs>
constexpr auto operator+(Multiplication<F, Gs...> lhs, Multiplication<F, Gs...> rhs) {
    return (2_c*lhs.get<0>())*rhs.sub_product<1, sizeof...(Gs) + 1>();
}

template <typename... Fs, typename... Gs>
constexpr auto operator+(Multiplication<Fs...> lhs, Multiplication<Gs...> rhs) -> std::enable_if_t<util::is_same<Multiplication<Fs...>, Multiplication<Gs...>>::value, Multiplication<Constant<2, 1>, Fs...>> {
    using namespace literals;
    return 2_c*lhs;
}

template <Int A, Int B, Int C, Int D, typename... Fs, typename... Gs>
constexpr auto operator+(Multiplication<Constant<A, B>, Fs...> lhs, Multiplication<Constant<C, D>, Gs...> rhs) -> std::enable_if_t<util::is_same<Multiplication<Fs...>, Multiplication<Gs...>>::value, Multiplication<decltype(Constant<A, B>{} + Constant<C, D>{}), Fs...>> {
    return (Constant<A, B>{} + Constant<C, D>{})*lhs.sub_product<1, sizeof...(Fs) + 1>();
}

// Division with common factors
template <typename F, typename G>
constexpr auto operator+(Division<F, G> y, F f) {
    using namespace literals;
    return f*(1_c + y.rhs)/y.rhs;
}

template <typename F, typename G>
constexpr auto operator+(F f, Division<F, G> y) {
    using namespace literals;
    return f*(y.rhs + 1_c)/y.rhs;
}

// Division with unrelated factors
template <typename F, typename G, typename H>
constexpr auto operator+(Division<F, G> lhs, H rhs) { return (lhs.lhs + lhs.rhs*rhs)/lhs.rhs; }

template <typename F, typename G, typename H>
constexpr auto operator+(H lhs, Division<F, G> rhs) { return (lhs*rhs.rhs + rhs.lhs)/rhs.rhs; }

template <typename A, typename B, typename C, typename D>
constexpr auto operator+(Division<A, B> lhs, Division<C, D> rhs) { return (lhs.lhs*rhs.rhs + lhs.rhs*rhs.lhs)/(lhs.rhs*rhs.rhs); }


template <Int A, Int B, Int C, Int D>
constexpr auto operator+(Constant<A, B>, Constant<C, D>) {
    using R = std::ratio_add<std::ratio<A, B>, std::ratio<C, D>>;
    return Constant<R::num, R::den>{};
}

template <Int A, Int B>
constexpr auto operator+(Constant<A, B> lhs, Constant<A, B>) {
    using namespace literals;
    return 2_c*lhs;
}

// Main Addition
template <typename F, typename G>
constexpr auto operator+(F lhs, G rhs) {
    if constexpr (std::is_same_v<F, Constant<0>>) {
        return rhs;
    } else if constexpr (std::is_same_v<G, Constant<0>>) {
        return lhs;
    } else if constexpr (std::is_arithmetic_v<G>) {
        return static_cast<G>(lhs) + rhs;
    } else if constexpr (std::is_arithmetic_v<F>) {
        return lhs + static_cast<F>(rhs);
    } else {
        return Addition<F, G>(lhs, rhs);
    }
}

}	// operators
}	// fields