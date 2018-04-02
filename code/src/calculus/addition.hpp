#pragma once

#include "composite.hpp"
#include "multiply.hpp"
#include "functions/constant.hpp"

namespace fields {
namespace operators {

/*template <typename F, typename G>
class Addition : public Composite<F, G> {
 public:
    template <typename Lhs, typename Rhs>
    Addition(Lhs&& l, Rhs&& r) : Composite<F, G>(std::forward<Lhs>(l), std::forward<Rhs>(r)) {
    }

    template <typename T>
    auto operator() (T x) { return f_(x) + g_(x); }
};

template <typename F, typename G>
class Subtraction : public Composite<F, G> {
public:
template <typename Lhs, typename Rhs>
Subtraction(Lhs&& l, Rhs&& r) : Composite<F, G>(std::forward<Lhs>(l), std::forward<Rhs>(r)) {
}

template <typename T>
auto operator() (T x) { return f_(x) - g_(x); }
};

*/

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

     constexpr auto derivative() const { return derivative_impl(std::make_index_sequence<N>{}); }

 private:
    template <typename... Gs> friend class Addition;

    explicit constexpr Addition(std::tuple<Fs...> ts) : fs(ts) {
    }

    template <typename T, std::size_t... Is>
    constexpr auto sum_impl(T x, std::index_sequence<Is...>) const { return (get<Is>()(x) + ...); }

    template <std::size_t... Is>
    constexpr auto derivative_impl(std::index_sequence<Is...>) const { return (d_dx(get<Is>()) + ...); }

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

template <typename F>
constexpr auto operator+(F, F) {
    using namespace literals;
    return 2_c*F{};
}

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

template <typename G, typename... Fs>
constexpr auto operator+(Addition<Fs...> f, G g) { return Addition<Fs..., G>(f, g); }

template <typename... Fs, typename G>
constexpr auto operator+(G g, Addition<Fs...> f) { return Addition<G, Fs...>(g, f); }

template <typename... Fs, typename... Gs>
constexpr auto operator+(Addition<Fs...> lhs, Addition<Gs...> rhs) { return Addition<Fs..., Gs...>(lhs, rhs); }

template <typename F, typename G>
constexpr auto operator+(F lhs, G rhs) { return Addition<F, G>(lhs, rhs); }






template <typename F>
constexpr auto operator-(F, F) {
    using namespace literals;
    return 0_c;
}

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
    static_assert(false, "Carry on here!");
    return (Multiplication<Gs...>{} - 1_c)*f;
}

template <typename F, typename... Gs>
constexpr auto operator-(F f, Multiplication<Gs..., F> y) { return -(y - f); }

template <typename A, typename B, typename C, typename D>
constexpr auto operator-(Division<A, B> f, Division<C, D> g) { return (f.lhs*g.rhs - f.rhs*g.lhs)/(f.rhs*g.rhs); }

template <typename F, typename G>
constexpr auto operator-(F lhs, G rhs) { return Subtraction<F, G>{lhs, rhs}; }


template <typename... Fs, typename... Gs>
constexpr auto operator==(Addition<Fs...> lhs, Addition<Gs...> rhs) { return std::is_same_v<decltype(lhs), decltype(rhs)>; }


template <typename... Fs>
constexpr auto d_dx(Addition<Fs...> y) { return y.derivative(); }



template <typename F, typename G>
constexpr auto d_dx(Subtraction<F, G> y) { return d_dx(y.lhs) - d_dx(y.rhs); }

}	// operators
}	// fields