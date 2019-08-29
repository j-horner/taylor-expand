#pragma once

#include "functions/constant.hpp"

#include "operators/multiplication.hpp"

// C++ headers
#include <tuple>

namespace fields {

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
    constexpr auto operator()(Args... args) const { return multiply_impl(components_{}, std::make_tuple(args...)); }

    template <std::size_t I>
    constexpr auto get() const { return std::get<I>(fs); }

    template <typename F>
    constexpr auto get() const { return std::get<F>(fs); }

    template <std::size_t I, std::size_t J>
    constexpr auto sub_product() const {
        static_assert((I <= sizeof...(Fs)) && (J <= sizeof...(Fs)) && (I <= J), "Expect following conditions on I, J: (I <= N) && (J <= N) && (I <= J)");
        return subset_impl<I>(std::make_index_sequence<J - I>{});
    }

    template <typename D>
    constexpr auto derivative(D d) const { return derivative_impl(d, components_{}); }

 private:
	using components_ = std::make_index_sequence<sizeof...(Fs)>;
	 
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
            return d(get<0>())*sub_product<1, sizeof...(Fs)>();
        } else if constexpr (I == (sizeof...(Fs) - 1)) {
            return sub_product<0, sizeof...(Fs) - 1>()*d(get<sizeof...(Fs) - 1>());
        } else {
            return sub_product<0, I>()*d(get<I>())*sub_product<I + 1, sizeof...(Fs)>();
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
};

template <typename Stream, typename... Fs>
auto& operator<<(Stream& os, Multiplication<Fs...> y) {
    (os << ... << y.template get<Fs>());
    return os;
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

}   // fields

#include "operators/multiplication_impl.hpp"