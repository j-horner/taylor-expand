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

template <typename T>
struct is_multiplication : std::false_type {};

template <typename... Fs>
struct is_multiplication<Multiplication<Fs...>> : std::true_type {};

template <typename T>
constexpr static auto is_not_multiplication = is_multiplication<T>::value == false;


template <typename F, typename G>
class Division {
public:
    template <typename T>
    constexpr auto operator()(T x) const { return lhs(x)/rhs(x); }

    F lhs;
    G rhs;

    static_assert(is_not_constant<std::decay_t<G>>);    // dividing by constants is equivalent my multiplication by inverse so this should never be constructed
};

// -------------------------------------------------------------------------------------------------
//                                      * operations
// -------------------------------------------------------------------------------------------------

// Constant - any multiplications with Constant on the right will be moved so it's on the left, also ensure all constants are moved to the front through multiplications

// TODO - Multiplication<F, Constant> should never happen...
template <Int A, Int B, Int C, Int D, typename F>
constexpr auto operator*(Constant<A, B> c, Multiplication<F, Constant<C, D>> y) { return (c*y.get<1>())*y.get<0>(); }

template <Int A, Int B, Int C, Int D, typename... Fs>
constexpr auto operator*(Constant<A, B> c, Multiplication<Constant<C, D>, Fs...> y) { return (c*y.get<0>())*y.sub_product<1, sizeof...(Fs) + 1>(); }

template <typename F, Int A, Int B, typename... Fs>
constexpr auto operator*(F f, Multiplication<Constant<A, B>, Fs...> y) { return y.get<0>()*(f*y.sub_product<1, sizeof...(Fs) + 1>()); }


// Multiplications with common factors
// TODO - create a Power class for this

// Multiplications with unrelated factors
// Need to remove the case where G is constant as all constants are moved to the left-hand-side
template <typename G, typename... Fs>
constexpr auto operator*(Multiplication<Fs...> f, G g) -> detail::only_if_not_constant<G, Multiplication<Fs..., G>> { return Multiplication<Fs..., G>(f, g); }

// Need to remove the case where G is 1 or 0 (these are trivial) but if we disable for ALL constants, expressions like 2*G will incorrectly call the generic operator* for F*G
template <typename... Fs, typename G>
constexpr auto operator*(G g, Multiplication<Fs...> f) -> detail::only_if_not_1_or_0<G, Multiplication<G, Fs...>> { return Multiplication<G, Fs...>(g, f); }

template <typename... Fs, typename... Gs>
constexpr auto operator*(Multiplication<Fs...> lhs, Multiplication<Gs...> rhs) { return Multiplication<Fs..., Gs...>(lhs, rhs); }

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

template <typename F, typename G, typename H>
constexpr auto operator*(F f, Division<G, H> d) { return (f*d.lhs)/d.rhs; }

template <typename F, typename G, typename H>
constexpr auto operator*(Division<G, H> d, F f) { return (d.lhs*f)/d.rhs; }

// Main Multiplication
template <typename F, typename G>
constexpr auto operator*(F lhs, G rhs) { return Multiplication<F, G>(lhs, rhs); }


// -------------------------------------------------------------------------------------------------
//                                      / operations
// -------------------------------------------------------------------------------------------------

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

template <typename F, typename G>
constexpr auto operator/(F lhs, G rhs) { return Division<F, G>{lhs, rhs}; }


// Comparison operators
template <typename... Fs, typename... Gs>
constexpr auto operator==(Multiplication<Fs...> lhs, Multiplication<Gs...> rhs) { return std::is_same_v<decltype(lhs), decltype(rhs)>; }

template <typename A, typename B, typename C, typename D>
constexpr auto operator==(Division<A, B> lhs, Division<C, D> rhs) { return std::is_same_v<decltype(lhs), decltype(rhs)>; }


// Derivative operators
template <typename... Fs>
constexpr auto d_dx(Multiplication<Fs...> y) { return y.derivative(); }

template <typename F, typename G>
constexpr auto d_dx(Division<F, G> y) { return (d_dx(y.lhs)*y.rhs - y.lhs*d_dx(y.rhs))/(y.rhs*y.rhs); }

}	// operators

}	// fields
