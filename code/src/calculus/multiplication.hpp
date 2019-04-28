#pragma once

#include "functions/constant.hpp"
#include "addition.hpp"
#include "division.hpp"

#include "comparison.hpp"

#include "derivative.hpp"

// C++ headers
#include <tuple>

// C headers
#include <cstdint>


namespace fields {

using Int = std::intmax_t;

template <Int A, Int B>
class Constant;

namespace operators {

template <typename... Fs>
class Multiplication {
 public:

    constexpr Multiplication() = default;

    template <typename... Ts, typename G>
    constexpr Multiplication(G lhs, Multiplication<Ts...> rhs) : fs(std::tuple_cat(std::make_tuple(lhs), rhs.fs)) {
    }

    template <typename... Ts, typename G>
    constexpr Multiplication(Multiplication<Ts...> lhs, G rhs) : fs(std::tuple_cat(lhs.fs, std::make_tuple(rhs))) {
    }

    template <typename... Ts, typename... Us>
    constexpr Multiplication(Multiplication<Ts...> lhs, Multiplication<Us...> rhs) : fs(std::tuple_cat(lhs.fs, rhs.fs)) {
    }

    template <typename F, typename G>
    constexpr Multiplication(F lhs, G rhs) : fs(std::make_tuple(lhs, rhs)) {
    }

    template <typename... Args>
    constexpr auto operator()(Args... args) const { return multiply_impl(std::make_index_sequence<N>{}, std::make_tuple(args...)); }

    template <std::size_t I>
    constexpr auto get() const { return std::get<I>(fs); }

    template <typename F>
    constexpr auto get() const { return std::get<F>(fs); }

    template <std::size_t I, std::size_t J>
    constexpr auto sub_product() const {
        static_assert((I <= N) && (J <= N) && (I <= J), "Expect following conditions on I, J: (I <= N) && (J <= N) && (I <= J)");
        return subset_impl<I>(std::make_index_sequence<J - I>{});
    }

    template <typename D>
    constexpr auto derivative(D d) const { return derivative_impl(d, std::make_index_sequence<N>{}); }

 private:
    template <typename... Gs> friend class Multiplication;

    explicit constexpr Multiplication(std::tuple<Fs...> ts) : fs(ts) {
    }

    // https://stackoverflow.com/questions/43242923/product-of-functions-with-a-fold-expression#43243060
    template <std::size_t... Is, typename Tuple>
    constexpr auto multiply_impl(std::index_sequence<Is...>, Tuple args) const { return (std::apply(get<Is>(), args)*...);}

    template <typename D, std::size_t... Is>
    constexpr auto derivative_impl(D d, std::index_sequence<Is...>) const {
        // return (((*this)*d(get<Is>())*(get<Is>()^(-1_c))) + ... );

        return (product_rule_term<Is>(d) + ...);
    }

    template <std::size_t I, typename D>
    constexpr auto product_rule_term(D d) const {
        if constexpr (I == 0) {
            return d(get<0>())*sub_product<1, N>();
        } else if constexpr (I == (N - 1)) {
            return sub_product<0, N - 1>()*d(get<N - 1>());
        } else {
            return sub_product<0, I>()*d(get<I>())*sub_product<I + 1, N>();
        }
    }

    template <std::size_t I, std::size_t... Is>
    constexpr auto subset_impl(std::index_sequence<Is...>) const {
        if constexpr (sizeof...(Is) == 1) {
            return get<I>();
        } else {
            return Multiplication<std::decay_t<decltype(get<I + Is>())>...>(std::make_tuple(get<I + Is>()...));
        }
    }

    std::tuple<Fs...> fs;

    constexpr static auto N = sizeof...(Fs);
};

template <typename... Fs>
auto operator<<(std::ostream& os, Multiplication<Fs...> y) -> std::ostream& {
    (os << ... << y.get<Fs>());
    return os;
}


// -------------------------------------------------------------------------------------------------
//                                      * operations
// -------------------------------------------------------------------------------------------------

// Constant - any multiplications with Constant on the right will be moved so it's on the left, also ensure all constants are moved to the front through multiplications

// TODO - Multiplication<F, Constant> should never happen...
template <Int A, Int B, Int C, Int D, typename F>
constexpr auto operator*(Constant<A, B> c, Multiplication<F, Constant<C, D>> y) { return (c*y.get<1>())*y.get<0>(); }

template <typename F, Int A, Int B, typename... Fs>
constexpr auto operator*(F f, Multiplication<Constant<A, B>, Fs...> y) {
    if constexpr (detail::is_constant<F>::value) {
        return (f*y.get<0>())*y.sub_product<1, sizeof...(Fs) + 1>();
    } else {
        return y.get<0>()*(f*y.sub_product<1, sizeof...(Fs) + 1>());
    }
}

template <Int A, Int B, typename... Fs>
constexpr auto operator*(Constant<A, B> f, Multiplication<Constant<A, B>, Fs...> y) {
    return (f*f)*y.sub_product<1, sizeof...(Fs) + 1>();
}

template <typename... Fs, Int A, Int B, typename... Gs>
constexpr auto operator*(Multiplication<Fs...> lhs, Multiplication<Constant<A, B>, Gs...> rhs) { return rhs.get<0>()*(lhs*rhs.sub_product<1, sizeof...(Gs) + 1>()); }

// Multiplications with unrelated factors
// Need to remove the case where G is constant as all constants are moved to the left-hand-side
template <typename G, typename... Fs>
constexpr auto operator*(Multiplication<Fs...> f, G g) {
    if constexpr (detail::is_constant<G>::value) {
        return g*f;
    } else {
        constexpr static auto match_found = (std::is_same_v<Fs, G> || ...);

        if constexpr (false == match_found) {
            constexpr static auto power_found = (detail::is_power<Fs, G>::value || ...);

            if constexpr (false == power_found) {
                return Multiplication<Fs..., G>(f, g);
            } else {
                return f.get<0>()*(f.sub_product<1, sizeof...(Fs)>()*g);
            }
        } else {
            return f.get<0>()*(f.sub_product<1, sizeof...(Fs)>()*g);
        }
    }
}

template <typename... Fs, typename G>
constexpr auto operator*(G g, Multiplication<Fs...> f) {
    if constexpr (std::is_same_v<G, Constant<0>>) {
        return 0_c;
    } else if constexpr (std::is_same_v<G, Constant<1>>) {
        return f;
    } else {
        constexpr static auto match_found = (util::is_same<Fs, G>::value || ...);

        if constexpr (false == match_found) {
            constexpr static auto power_found = (detail::is_power<Fs, G>::value || ...);

            if constexpr (false == power_found) {
                return Multiplication<G, Fs...>(g, f);
            } else {
                return f.get<0>()*(g*f.sub_product<1, sizeof...(Fs)>());
            }
        } else {
            return f.get<0>()*(g*f.sub_product<1, sizeof...(Fs)>());
        }
    }
}

template <typename... Fs>
constexpr auto operator*(Multiplication<Fs...> lhs, Multiplication<Fs...>) {
    return Power<Multiplication<Fs...>, 2>{lhs};
}

template <typename... Fs, typename... Gs>
constexpr auto operator*(Multiplication<Fs...> lhs, Multiplication<Gs...> rhs) {
    if constexpr (util::is_same<Multiplication<Fs...>, Multiplication<Gs...>>::value) {
        return Power<Multiplication<Fs...>, 2>{lhs};
    } else {
        return (lhs*rhs.get<0>())*rhs.sub_product<1, sizeof...(Gs)>();
    }
}

// Division with common factors
template <typename F, typename G, typename H>
constexpr auto operator*(Multiplication<G, F> m, Division<H, F> d) { return m.get<0>()*d.lhs; }

template <typename F, typename G, typename H>
constexpr auto operator*(Division<H, F> d, Multiplication<G, F> m) { return d.lhs*m.get<0>(); }

template <typename F, typename... Gs, typename H>
constexpr auto operator*(Multiplication<F, Gs...> m, Division<H, F> d) { return m.sub_product<1, sizeof...(Gs) + 1>()*d.lhs; }

template <typename F, typename... Gs, typename H>
constexpr auto operator*(Division<H, F> d, Multiplication<F, Gs...> m) { return d.lhs*m.sub_product<1, sizeof...(Gs) + 1>(); }

template <typename A, typename B, typename C>
constexpr auto operator*(Division<A, B> lhs, Division<B, C> rhs) { return lhs.lhs/rhs.rhs; }

template <typename A, typename B, typename C>
constexpr auto operator*(Division<A, B> lhs, Division<C, A> rhs) { return rhs.lhs/lhs.rhs; }

// Division with unrelated factors
template <typename A, typename B, typename C, typename D>
constexpr auto operator*(Division<A, B> lhs, Division<C, D> rhs) { return (lhs.lhs*rhs.lhs)/(lhs.rhs*rhs.rhs);}

template <typename... Fs, typename A, typename B>
constexpr auto operator*(Multiplication<Fs...> lhs, Division<A, B> rhs) { return (lhs*rhs.lhs)/rhs.rhs; }

template <typename F, typename G, typename H>
constexpr auto operator*(F f, Division<G, H> d) { return (f*d.lhs)/d.rhs; }

template <typename F, typename G, typename H>
constexpr auto operator*(Division<G, H> d, F f) { return (d.lhs*f)/d.rhs; }

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

template <typename F>
constexpr auto operator*(F lhs, F) {
    using namespace literals;
    if constexpr (std::is_same_v<std::decay_t<decltype(lhs)>, Constant<0>>) {
        return 0_c;
    } else {
        return Power<F, 2>{lhs};
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
constexpr auto operator*(F, Power<F, N> rhs) {
    using namespace literals;
    if constexpr (N == -1) {
        return 1_c;
    } else {
        return Power<F, N + 1>{rhs};
    }
}

template <typename F, typename G>
constexpr auto operator*(Multiplication<F, G> lhs, G rhs) {
    using namespace literals;
    if constexpr (std::is_same_v<std::decay_t<decltype(rhs)>, Constant<0>>) {
        return 0_c;
    } else {
        return lhs.get<0>()*Power<G, 2>{rhs};
    }
}

template <typename F, typename... Gs>
constexpr auto operator*(F lhs, Multiplication<F, Gs...> rhs) {
    return Power<F, 2>{lhs}*rhs.sub_product<1, sizeof...(Gs) + 1>();
}

template <typename F, typename G, Int N>
constexpr auto operator*(Multiplication<F, G> lhs, Power<G, N> rhs) {
    if constexpr (-1 == N) {
        return lhs.get<0>();
    } else {
        return lhs.get<0>()*Power<G, N + 1>{rhs};
    }
}

template <typename F, typename... Gs, Int N>
constexpr auto operator*(Power<F, N> lhs, Multiplication<F, Gs...> rhs) {
    return Power<F, N + 1>{lhs}*rhs.sub_product<1, sizeof...(Gs) + 1>();
}

template <typename F, typename G, Int N>
constexpr auto operator*(Multiplication<F, Power<G, N>> lhs, G rhs) {
    return lhs.get<0>()*Power<G, N + 1>{rhs};
}

template <typename F, typename... Gs, Int N>
constexpr auto operator*(F lhs, Multiplication<Power<F, N>, Gs...> rhs) {
    return Power<F, N + 1>{lhs}*rhs.sub_product<1, sizeof...(Gs) + 1>();
}

template <typename F, typename G, Int N, Int M>
constexpr auto operator*(Multiplication<F, Power<G, N>> lhs, Power<G, M> rhs) {
    if constexpr (N + M == 1) {
        return lhs.get<0>()*rhs.f();
    } else {
        return lhs.get<0>()*Power<G, N + M>{rhs};
    }
}

template <typename F, typename... Gs, Int N, Int M>
constexpr auto operator*(Power<F, N> lhs, Multiplication<Power<F, M>, Gs...> rhs) {
    return Power<F, N + M>{lhs}*rhs.sub_product<1, sizeof...(Gs) + 1>();
}

template <typename F, typename G, Int N>
constexpr auto operator*(Multiplication<F, Power<G, N>> lhs, Power<G, N> rhs) {
    return lhs.get<0>()*Power<G, 2*N>{rhs};
}

template <typename F, typename... Gs, Int N>
constexpr auto operator*(Power<F, N> lhs, Multiplication<Power<F, N>, Gs...> rhs) {
    return Power<F, 2*N>{lhs}*rhs.sub_product<1, sizeof...(Gs) + 1>();
}

template <Int A, Int B, Int C, Int D, Int N, typename... Fs>
constexpr auto operator*(Constant<A, B> lhs, Power<Multiplication<Constant<C, D>, Fs...>, N> rhs) {
    constexpr auto n = Constant<N>{};
    return (lhs*(rhs.f().get<0>()^n))*(rhs.f().sub_product<1, sizeof...(Fs) + 1>()^n);
}

// Main Multiplication
template <typename F, typename G>
constexpr auto operator*(F lhs, G rhs) {
    if constexpr (std::is_same_v<decltype(lhs), Constant<0>> || std::is_same_v<decltype(rhs), Constant<0>>) {
        using namespace literals;
        return 0_c;
    } else if constexpr (std::is_same_v<decltype(lhs), Constant<1>>) {
        return rhs;
    } else if constexpr (std::is_same_v<decltype(rhs), Constant<1>>) {
        return lhs;
    } else if constexpr (detail::is_constant<G>::value) {
        return rhs*lhs;
    } else if constexpr (std::is_arithmetic_v<G>) {
        return static_cast<G>(lhs)*rhs;
    } else if constexpr (std::is_arithmetic_v<F>) {
        return lhs*static_cast<F>(rhs);
    } else {
        return Multiplication<F, G>(lhs, rhs);
    }
}

namespace detail {

template <typename T, typename U>
struct is_multiple : std::false_type {};

template <Int A, Int B, typename... Fs>
struct is_multiple<Multiplication<Fs...>, Multiplication<Constant<A, B>, Fs...>> : std::true_type {};

template <Int A, Int B, typename... Fs, typename... Gs>
struct is_multiple<Multiplication<Fs...>, Multiplication<Constant<A, B>, Gs...>> : util::is_same<Multiplication<Fs...>, Multiplication<Gs...>> {};

template <Int A, Int B, typename... Fs>
struct is_multiple<Multiplication<Constant<A, B>, Fs...>, Multiplication<Fs...>> : std::true_type {};

template <Int A, Int B, typename... Fs, typename... Gs>
struct is_multiple<Multiplication<Constant<A, B>, Fs...>, Multiplication<Gs...>> : util::is_same<Multiplication<Fs...>, Multiplication<Gs...>> {};

template <Int A, Int B, Int C, Int D, typename... Fs>
struct is_multiple<Multiplication<Constant<A, B>, Fs...>, Multiplication<Constant<C, D>, Fs...>> : std::true_type {};

template <Int A, Int B, Int C, Int D, typename... Fs, typename... Gs>
struct is_multiple<Multiplication<Constant<A, B>, Fs...>, Multiplication<Constant<C, D>, Gs...>> : util::is_same<Multiplication<Fs...>, Multiplication<Gs...>> {};

}   //detail

}   // operators
}   // fields
