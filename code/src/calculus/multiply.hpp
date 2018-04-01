#pragma once

#include "composite.hpp"
#include "addition.hpp"
#include "functions\constant.hpp"

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
     constexpr static auto N = sizeof...(Fs);

     constexpr Multiplication() = default;

     template <typename... Ts, typename G>
     constexpr Multiplication(G lhs, Multiplication<Ts...> rhs) : fs(std::tuple_cat(std::make_tuple(lhs), rhs.fs)) {
     }

     template <typename... Ts, typename G>
     constexpr Multiplication(Multiplication<Ts...> lhs, G rhs) : Multiplication(rhs, lhs) {
     }

     template <typename F, typename G>
     constexpr Multiplication(F lhs, G rhs) : fs(std::make_tuple(lhs, rhs)) {
     }

     explicit constexpr Multiplication(std::tuple<Fs...> ts) : fs(ts) {
     }

     std::tuple<Fs...> fs;

     template <typename T>
     constexpr auto operator()(T x) const { return multiply_impl(x, std::make_index_sequence<N>{}); }

     template <std::size_t I>
     constexpr auto& get() const { return std::get<I>(fs); }

    template <std::size_t I, std::size_t J>
    constexpr auto subset() const {
        static_assert((I <= N) && (J <= N) && (I <= J), "Expect following conditions on I, J: (I <= N) && (J <= N) && (I <= J)");
        return subset_impl<I>(std::make_index_sequence<J - I>{});
    }

     constexpr auto product_rule() const { return product_rule_impl(std::make_index_sequence<N>{}); }

 private:

    // https://stackoverflow.com/questions/43242923/product-of-functions-with-a-fold-expression#43243060
    template <typename T, std::size_t... Is>
    constexpr auto multiply_impl(T x, std::index_sequence<Is...>) const { return (std::get<Is>(fs)(x)*...); }

    template <std::size_t... Is>
    constexpr auto product_rule_impl(std::index_sequence<Is...>) const { return (product_rule_term<Is>() + ...); }

    template <std::size_t I>
    constexpr auto product_rule_term() const {
        if constexpr (I == 0) {
            
            return d_dx(get<0>())*subset<1, N>();
        
        } else if constexpr (I == (N - 1)) {
            
            return subset<0, N - 1>()*d_dx(get<N - 1>());
        
        } else {

            return subset<0, I>()*d_dx(get<I>())*subset<I + 1, N>();
        
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
constexpr auto operator*(Multiplication<Fs...> f, G g) -> detail::only_if_not_constant<G, decltype(g*f)> { return g*f; }

template <typename... Fs, typename G>
constexpr auto operator*(G g, Multiplication<Fs...> f) -> detail::only_if_not_1_or_0<G, Multiplication<G, Fs...>> { return Multiplication<G, Fs...>(g, f); }

/*template <Int A, Int B, Int C, Int D, typename... Fs>
constexpr auto operator*(Constant<A, B>, Multiplication<Constant<C, D>, Fs...> y) { return Constant<A, B>{}*Constant<C, D>{}*y.subset<1, sizeof...(Fs) + 1>(); }*/

template <typename F, typename G>
constexpr auto operator*(F lhs, G rhs) { return Multiplication<F, G>(lhs, rhs); }




/*
template <typename F, typename G>
constexpr auto operator*(F lhs, G rhs) -> typename std::enable_if_t<(is_one_or_zero<F>::value == false) &&
                                                                    (is_one_or_zero<G>::value == false) &&
                                                                    ((is_constant<F>::value && is_constant<G>::value) == false), Multiplication<F, G>> {
    return Multiplication<F, G>(lhs, rhs);
}


template <typename... Fs, typename G>
constexpr auto operator*(G lhs, Multiplication<Fs...> rhs) -> typename std::enable_if_t<is_one_or_zero<G>::value == false, Multiplication<Fs..., G>> { return rhs*lhs; }

template <typename... Fs, typename G>
constexpr auto operator*(Multiplication<Fs...> lhs, G rhs) -> typename std::enable_if_t<is_one_or_zero<G>::value == false, Multiplication<Fs..., G>> { return Multiplication<Fs..., G>(lhs, rhs); }

*/



template <typename F, typename G>
constexpr auto operator/(F lhs, G rhs) { return Division<F, G>{lhs, rhs}; }


template <typename A, typename B, typename C, typename D>
constexpr auto operator==(Multiplication<A, B>, Multiplication<C, D>) { return (std::is_same_v<A, C> && std::is_same_v<B, D>) || (std::is_same_v<A, D> && std::is_same_v<B, C>); }


template <typename... Fs>
constexpr auto d_dx(Multiplication<Fs...> y) {
    return y.product_rule();
}




template <typename F, typename G>
constexpr auto d_dx(Division<F, G> y) { return (d_dx(y.lhs)*y.rhs - y.lhs*d_dx(y.rhs))/(y.rhs*y.rhs); }

}	// operators

}	// fields
