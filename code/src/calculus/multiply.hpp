#pragma once

#include "composite.hpp"
#include "addition.hpp"
#include "functions/constant.hpp"

namespace fields {
namespace operators {

/*template <typename F>
class MultiplyScalar;

template <typename F, typename G>
class MultiplyBase : public Composite<F, G> {
public:
	template <typename Lhs, typename Rhs>
	MultiplyBase(Lhs&& l, Rhs&& r) : Composite<F, G>(std::forward<Lhs>(l), std::forward<Rhs>(r)) {
	}

	template <typename T>
	auto operator()(T x) {
		return lhs()(x)*rhs()(x);
	}
};

template <typename F, typename G>
class Multiply : public MultiplyBase<F, G> {
public:
	template <typename Lhs, typename Rhs>
	Multiply(Lhs&& l, Rhs&& r) : MultiplyBase<F, G>(std::forward<Lhs>(l), std::forward<Rhs>(r)) {
	}
};

template <typename F>
class Multiply<F, F> : public MultiplyBase<F, F> {
public:
	Multiply(SharedField<F> f, SharedField<F> g) : MultiplyBase<F, F>(std::move(f), std::move(g)) {
	}

	template <typename T>
	auto operator() (T x) {
		if (&lhs() == &rhs()) {
			auto val = lhs()(x);		// underlying functions are the same, call once then square
			return val*val;
		} else {
			return MultiplyBase<F, F>::operator()(x);	// otherwise call default f*g
		}
	}
};*/

/*template <typename... Fs>
class Multiplication;

template <typename F, typename G>
class Multiplication<F, G> {
public:
    template <typename T>
    constexpr auto operator()(T x) const { return lhs()(x)*rhs()(x); }

    constexpr auto& lhs() const { return fs.first; }
    constexpr auto& rhs() const { return fs.second; }

    std::pair<F, G> fs;
};*/

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

    template <typename T>
    constexpr auto operator()(T x) const { return multiply_impl(x, std::make_index_sequence<N>{}); }

    template <std::size_t I>
    constexpr auto& get() const { return std::get<I>(fs); }

    template <std::size_t I, std::size_t J>
    constexpr auto sub_product() const {
        static_assert((I <= N) && (J <= N) && (I <= J), "Expect following conditions on I, J: (I <= N) && (J <= N) && (I <= J)");
        return subset_impl<I>(std::make_index_sequence<J - I>{});
    }

    constexpr auto derivative() const { return derivative_impl(std::make_index_sequence<N>{}); }

 private:
    template <typename... Gs> friend class Multiplication;

    explicit constexpr Multiplication(std::tuple<Fs...> ts) : fs(ts) {
    }

    // https://stackoverflow.com/questions/43242923/product-of-functions-with-a-fold-expression#43243060
    template <typename T, std::size_t... Is>
    constexpr auto multiply_impl(T x, std::index_sequence<Is...>) const { return (get<Is>()(x)*...); }

    template <std::size_t... Is>
    constexpr auto derivative_impl(std::index_sequence<Is...>) const { return (product_rule_term<Is>() + ...); }

    template <std::size_t I>
    constexpr auto product_rule_term() const {
        if constexpr (I == 0) {

            return d_dx(get<0>())*sub_product<1, N>();

        } else if constexpr (I == (N - 1)) {

            return sub_product<0, N - 1>()*d_dx(get<N - 1>());

        } else {

            return sub_product<0, I>()*d_dx(get<I>())*sub_product<I + 1, N>();

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



template <typename F, typename G>
class Division {
public:
    template <typename T>
    constexpr auto operator()(T x) const { return lhs(x)/rhs(x); }

    F lhs;
    G rhs;
};


template <typename G, typename... Fs>
constexpr auto operator*(Multiplication<Fs...> f, G g) -> detail::only_if_not_constant<G, Multiplication<Fs..., G>> { return Multiplication<Fs..., G>(f, g); }

template <typename... Fs, typename G>
constexpr auto operator*(G g, Multiplication<Fs...> f) -> detail::only_if_not_1_or_0<G, Multiplication<G, Fs...>> { return Multiplication<G, Fs...>(g, f); }

template <typename... Fs, typename... Gs>
constexpr auto operator*(Multiplication<Fs...> lhs, Multiplication<Gs...> rhs) { return Multiplication<Fs..., Gs...>(lhs, rhs); }

template <typename F, typename G>
constexpr auto operator*(F lhs, G rhs) { return Multiplication<F, G>(lhs, rhs); }




template <typename F, typename G>
constexpr auto operator/(F lhs, G rhs) { return Division<F, G>{lhs, rhs}; }


template <typename... Fs, typename... Gs>
constexpr auto operator==(Multiplication<Fs...> lhs, Multiplication<Gs...> rhs) { return std::is_same_v<decltype(lhs), decltype(rhs)>; }


template <typename... Fs>
constexpr auto d_dx(Multiplication<Fs...> y) { return y.derivative(); }




template <typename F, typename G>
constexpr auto d_dx(Division<F, G> y) { return (d_dx(y.lhs)*y.rhs - y.lhs*d_dx(y.rhs))/(y.rhs*y.rhs); }

}	// operators

}	// fields
