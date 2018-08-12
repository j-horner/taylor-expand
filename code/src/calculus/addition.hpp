#pragma once

#include "composite.hpp"
#include "multiplication.hpp"
#include "functions/constant.hpp"

namespace fields {
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

    template <typename T>
    constexpr auto operator()(T x) const { return sum_impl(x, std::make_index_sequence<N>{}); }

    template <std::size_t I>
    constexpr auto& get() const { return std::get<I>(fs); }

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

    template <typename T, std::size_t... Is>
    constexpr auto sum_impl(T x, std::index_sequence<Is...>) const { return (get<Is>()(x) + ...); }

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

template <typename T>
struct is_addition : std::false_type {};

template <typename... Fs>
struct is_addition<Addition<Fs...>> : std::true_type {};

template <typename T>
constexpr static bool is_not_addition = is_addition<T>::value == false;



template <typename F, typename G>
class Subtraction {
public:
    template <typename T>
    constexpr auto operator()(T x) const { return lhs(x) - rhs(x); }

    F lhs;
    G rhs;
};

template <typename... Fs>
class Multiplication;

template <typename F, typename G>
class Division;


// -------------------------------------------------------------------------------------------------
//                                      + operations
// -------------------------------------------------------------------------------------------------

// Definition of number 2
template <typename F>
constexpr auto operator+(F rhs, F) -> detail::only_if_not_constant<F, decltype(Constant<2>{}*rhs)> {
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
template <typename G, typename... Fs>
constexpr auto operator+(Addition<Fs...> f, G g) -> detail::only_if_not_0<G, Addition<Fs..., G>> { return Addition<Fs..., G>(f, g); }

template <typename... Fs, typename G>
constexpr auto operator+(G g, Addition<Fs...> f) -> detail::only_if_not_0<G, Addition<G, Fs...>> { return Addition<G, Fs...>(g, f); }

template <typename... Fs, typename... Gs>
constexpr auto operator+(Addition<Fs...> lhs, Addition<Gs...> rhs) { return Addition<Fs..., Gs...>(lhs, rhs); }

// Subtractions with common factors
template <typename F, typename G>
constexpr auto operator+(Subtraction<G, F> y, F) {
    using namespace literals;
    return y.lhs;
}

template <typename F, typename G>
constexpr auto operator+(F f, Subtraction<G, F> y) {
    using namespace literals;
    return y.lhs;
}

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
constexpr auto operator+(Multiplication<F, Gs...> lhs, Multiplication<G, Gs...> rhs) -> detail::only_if_not_same<F, G, Multiplication<std::decay_t<decltype(lhs.get<0>() + rhs.get<0>())>, Gs...>> {
    return (lhs.get<0>() + rhs.get<0>())*lhs.sub_product<1, sizeof...(Gs) + 1>();
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

// Main Addition
template <typename F, typename G>
constexpr auto operator+(F lhs, G rhs) { return Addition<F, G>(lhs, rhs); }




// -------------------------------------------------------------------------------------------------
//                                      - operations
// -------------------------------------------------------------------------------------------------

// Definition of 0
template <typename F>
constexpr auto operator-(F, F) -> detail::only_if_not_constant<F, Constant<0>> {
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

// Main Subtraction definition
template <typename F, typename G>
constexpr auto operator-(F lhs, G rhs) { return Subtraction<F, G>{lhs, rhs}; }

// Comparison operators
template <typename... Fs, typename... Gs>
constexpr auto operator==(Addition<Fs...> lhs, Addition<Gs...> rhs) { return std::is_same_v<decltype(lhs), decltype(rhs)>; }

template <typename A, typename B, typename C, typename D>
constexpr auto operator==(Subtraction<A, B> lhs, Subtraction<C, D> rhs) { return std::is_same_v<decltype(lhs), decltype(rhs)>; }

// Derivative operators
template <Int D = 1, typename... Fs>
constexpr auto d_dx(Addition<Fs...> y) {
    return y.derivative([] (auto f) { return d_dx<D>(f); });
}

template <Int D = 1, typename... Fs>
constexpr auto d_dt(Addition<Fs...> y) { return y.derivative([] (auto f) { return d_dt<D>(f); }); }

template <Int D = 1, typename F, typename G>
constexpr auto d_dx(Subtraction<F, G> y) { return d_dx<D>(y.lhs) - d_dx<D>(y.rhs); }

template <Int D = 1, typename F, typename G>
constexpr auto d_dt(Subtraction<F, G> y) { return d_dt<D>(y.lhs) - d_dt<D>(y.rhs); }

}	// operators
}	// fields