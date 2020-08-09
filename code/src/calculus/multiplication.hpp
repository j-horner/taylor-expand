#pragma once

#include "comparison.hpp"

#include "functions/constant.hpp"
#include "functions/power.hpp"

// C++ headers
#include <tuple>

namespace fields {

template <typename F, Int N, typename Enabled>
class Power;

template <typename... Fs>
class Multiplication {
	static_assert(sizeof...(Fs) > 1, "Atleast 2 terms are required to form a multiple.");
	static_assert((std::is_same_v<Constant<1>, Fs> || ...) == false, "All *1 terms should be removed.");
public:
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

	// dont define copy constructor if Fs... and Gs... are the same
	template <typename... Gs, typename = std::enable_if_t<(std::is_same_v<Fs, Gs> && ...) == false>>
	constexpr Multiplication(Multiplication<Gs...> args) : fs(args.fs) {
	}

	template <typename... Args>
	constexpr auto operator()(Args... args) const { return multiply_impl(components_{}, std::make_tuple(args...)); }

	template <Int I>
	constexpr auto get() const { return std::get<I>(fs); }

	template <Int... Idx>
	constexpr auto get(std::integer_sequence<Int, Idx...>) const {
		static_assert(sizeof...(Idx) > 0, "Multiplication::get(...) must return atleast 1 term.");
		static_assert((((Idx < sizeof...(Fs)) && (Idx >= 0)) && ...));
		
		if constexpr (sizeof...(Idx) == 1) {
			return (... , get<Idx>());
		} else {
			static_assert(sizeof...(Idx) > 1, "Atleast 2 terms are required to form a multiple.");
			return Multiplication<std::decay_t<decltype(get<Idx>())>...>(std::make_tuple(get<Idx>()...));
		}
	}

	template <typename F>
	constexpr auto get() const { return std::get<F>(fs); }

	template <Int I, Int J>
	constexpr auto sub_product() const {
		static_assert(I <= sizeof...(Fs));
		static_assert(J <= sizeof...(Fs));
		static_assert(I < J);
		return subset_impl<I>(std::make_integer_sequence<Int, J - I>{});
	}

	template <typename D>
	constexpr auto derivative(D d) const { return derivative_impl(d, components_{}); }

	template <Int Idx>
	constexpr auto filter() const {
		constexpr auto remaining_terms = util::filter_indicies(components_{}, util::index_not<Idx>);
		return get(remaining_terms);
	}

private:
	using components_ = std::make_integer_sequence<Int, sizeof...(Fs)>;

	template <typename... Gs> friend class Multiplication;

	explicit constexpr Multiplication(std::tuple<Fs...> ts) : fs(ts) {
	}

	// https://stackoverflow.com/questions/43242923/product-of-functions-with-a-fold-expression#43243060
	template <Int... Is, typename Tuple>
	constexpr auto multiply_impl(std::integer_sequence<Int, Is...>, Tuple args) const { return (std::apply(get<Is>(), args) *...); }

	template <typename D, Int... Is>
	constexpr auto derivative_impl(D d, std::integer_sequence<Int, Is...>) const { return (... + product_rule_term<Is>(d)); }

	template <Int I, typename D>
	constexpr auto product_rule_term(D d) const {
		constexpr auto remaining_indicies = util::filter_indicies(components_{}, [] (auto idx) { return idx != I; });

		return d(get<I>())*get(remaining_indicies);
	}

	template <Int I, Int... Is>
	constexpr auto subset_impl(std::integer_sequence<Int, Is...>) const { return get(std::integer_sequence<Int, (I + Is)...>{}); }

	std::tuple<Fs...> fs;
};

template <typename Stream, typename... Fs>
auto operator<<(Stream& os, Multiplication<Fs...> y)->std::enable_if_t<(sizeof...(Fs) > 1), Stream&> {
	(os << ... << y.template get<Fs>());
	return os;
}

}	// fields

namespace std {

template <typename... Ts>
struct tuple_size<fields::Multiplication<Ts...>> {
	constexpr static auto value = sizeof...(Ts);

	static_assert(value > 1);
};

template <std::size_t I, typename... Ts>
struct tuple_element<I, fields::Multiplication<Ts...>> : tuple_element<I, tuple<Ts...>> {
	static_assert(sizeof...(Ts) > 1);
};

}	// std


namespace fields {
namespace detail {

template <typename T>
struct is_multiplication : std::false_type {};

template <typename... Ts>
struct is_multiplication<Multiplication<Ts...>> : std::true_type {};

// =====================================================================================
//	is_multiple<T, U> is symmetric i.e. if T is a multiple of U, then U is a multiple T
// =====================================================================================

template <typename T, typename U>
struct is_multiple;

template <typename T>
struct is_multiple<T, T> : std::true_type {};

template <typename T, typename... Gs>
struct is_multiple<T, Multiplication<Gs...>> : std::bool_constant<util::contains<T, Multiplication<Gs...>>::index >= 0> {};

template <typename T, typename... Gs>
struct is_multiple<Multiplication<Gs...>, T> : std::bool_constant<util::contains<T, Multiplication<Gs...>>::index >= 0> {};

template <Int A, Int B, typename F>
struct is_multiple<F, Multiplication<Constant<A, B>, F>> : std::true_type {};

template <Int A, Int B, typename F>
struct is_multiple<Multiplication<Constant<A, B>, F>, F> : std::true_type {};

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

template <typename... Fs, typename... Gs>
struct is_multiple<Multiplication<Fs...>, Multiplication<Gs...>> : util::is_same<Multiplication<Fs...>, Multiplication<Gs...>> {};

template <Int A, Int B, Int C, Int D, typename... Fs, typename... Gs>
struct is_multiple<Multiplication<Constant<A, B>, Fs...>, Multiplication<Constant<C, D>, Gs...>> : util::is_same<Multiplication<Fs...>, Multiplication<Gs...>> {};

template <typename T, typename U>
struct is_multiple : std::false_type {};


// =====================================================================================
//	is_sub_multiple<T, U> is non-symmetric i.e. if T is a multiple of U, U is only a multiple of T if U = T
//
//	e.g.	A*B is a multiple of A but A is not a multiple of A*B
//
// =====================================================================================

template <typename T, typename U, typename Enabled = void>
struct is_sub_multiple : std::false_type {};

template <typename T>
struct is_sub_multiple<T, T> : std::true_type {};

template <typename T, typename... Gs>
struct is_sub_multiple<Multiplication<Gs...>, T, std::enable_if_t<false == is_multiplication<T>::value>> : std::bool_constant<util::contains<T, Multiplication<Gs...>>::index >= 0> {};

// template <Int A, Int B, typename T>
// struct is_sub_multiple<Multiplication<Constant<A, B>, T>, T> : std::true_type {};

template <Int A, Int B, typename... Fs>
struct is_sub_multiple<Multiplication<Constant<A, B>, Fs...>, Multiplication<Fs...>> : std::true_type {};

template <typename... Fs, typename... Gs>
struct is_sub_multiple<Multiplication<Fs...>, Multiplication<Gs...>, std::enable_if_t<false == std::is_same_v<Multiplication<Fs...>, Multiplication<Gs...>>>> : std::bool_constant<((util::contains<Gs, Multiplication<Fs...>>::index >= 0) && ...)> {};

/*template <Int A, Int B, typename... Fs, typename... Gs>
struct is_sub_multiple<Multiplication<Constant<A, B>, Fs...>, Multiplication<Gs...>, std::enable_if_t<(false == std::is_same_v<Multiplication<Constant<A, B>, Fs...>, Multiplication<Gs...>>)>> : is_sub_multiple<Multiplication<Fs...>, Multiplication<Gs...>> {};

template <Int A, Int B, Int C, Int D, typename... Fs, typename... Gs>
struct is_sub_multiple<Multiplication<Constant<A, B>, Fs...>, Multiplication<Constant<C, D>, Gs...>, std::enable_if_t<(false == std::is_same_v<Multiplication<Constant<A, B>, Fs...>, Multiplication<Constant<C, D>, Gs...>>)>> : is_sub_multiple<Multiplication<Fs...>, Multiplication<Gs...>> {};
*/






/*
template <typename T, typename U>
struct multiply {
	using type = Multiplication<T, U>;
};

template <typename U, typename... Gs>
struct multiply<U, Multiplication<Gs...>> {
	
	constexpr static auto occurances = (static_cast<int>(util::is_same<U, Gs>::value) + ...);

	static_assert((occurances == 0) || (occurances == 1), "U should appear no more than once, otherwise it should have been Power<U, N> or not at all.");		

	constexpr static auto occured = (occurances == 1);
	
	// static_assert(false, "Failing here when constructing D*(2*D*C*T)");

	constexpr static auto power_occurances = (static_cast<int>(fields::detail::is_power<U, Gs>::value) + ...);

	static_assert((power_occurances == 0) || (power_occurances == 1), "Power<U, N> should appear no more than once, otherwise it should have been U or not at all.");

	constexpr static auto power_occured = (power_occurances == 1);

	static_assert((occured && power_occured) ? fields::detail::is_power<U, U>::value : true, "Both U and Power<U, N> should not have occured together unless U = Power<F, N> for some F. In this case we still only have one occurance which triggered both conditions.");


	template <typename G, typename P>
	struct power_value;

	template <typename G, typename P>
	struct power_value : std::integral_constant<Int, 0> {};

	template <typename G>
	struct power_value<G, G> : std::integral_constant<Int, 1> {};

	template <typename G, Int N>
	struct power_value<G, Power<G, N>> : std::integral_constant<Int, N> {
	};

	// search for U in the multiplication if it is present
	constexpr static auto location_in_pack = occured ? util::contains<U, Multiplication<Gs...>>::index : -1;

	static_assert(occured ? location_in_pack >= 0 : true, "If U occurred, it has a valid location.");

	// only search for Power<U> if U has not already been found
	constexpr static auto power_location_in_pack = ((power_occured) && (false == occured)) ? fields::detail::contains_power<U, Multiplication<Gs...>>::index : -1;

	static_assert(((power_occured) && (false == occured)) ? power_location_in_pack >= 0 : true, "If Power<U, N> occured, it has a valid location.");
	
	static_assert((power_occured && occured) ? (location_in_pack >= 0) && (power_location_in_pack == -1) : true, "If U occured and U = Power<F, N>, it has a valid location and we will use that (not power_location).");

	template <typename F, typename G>
	struct multiply_term {
		using type = void;
	};

	template <typename F, Int N>
	struct multiply_term<F, Power<F, N>> {
		using type = std::conditional_t<N == -1, Constant<1>, Power<F, N + 1>>;
	};

	template <typename F, Int N, Int M>
	struct multiply_term<Power<F, N>, Power<F, M>> {
		using type = std::conditional_t<N == -M, Constant<1>, Power<F, N + M>>;
	};

	template <typename F, Int N>
	struct multiply_term<Power<F, N>, Power<F, N>> {
		using type = Power<F, 2*N>;
	};

	template <typename F>
	struct multiply_term<F, F> {
		using type = Power<F, 2>;
	};

	template <Int location, typename Is>
	struct merged_multiply;

	template <Int location, Int... Is>
	struct merged_multiply<location, std::integer_sequence<Int, Is...>> { 
		using type = Multiplication<std::conditional_t<Is == location,
										typename multiply_term<U, std::tuple_element_t<Is, Multiplication<Gs...>>>::type,
										std::tuple_element_t<Is, Multiplication<Gs...>>>
									...>;
	};

	using type = std::conditional_t<occured,
										typename merged_multiply<location_in_pack, std::make_integer_sequence<Int, sizeof...(Gs)>>::type,
										std::conditional_t<power_occured,
															typename merged_multiply<power_location_in_pack, std::make_integer_sequence<Int, sizeof...(Gs)>>::type,
															Multiplication<U, Gs...>>>;
};

template <typename T, typename... Gs>
struct multiply<Multiplication<Gs...>, T> {
	using type = typename multiply<T, Multiplication<Gs...>>::type;
};

template <typename F, typename... Fs, typename... Gs>
struct multiply<Multiplication<F, Fs...>, Multiplication<Gs...>> : multiply<F, typename multiply<Multiplication<Fs...>, Multiplication<Gs...>>::type> {
};

template <typename F, typename... Gs>
struct multiply<Multiplication<F>, Multiplication<Gs...>> : multiply<F, Multiplication<Gs...>> {
};
*/

template <typename T, typename U>
struct contains_multiple;

/*template <typename T, typename... Ts>
struct contains_multiple<T, Multiplication<Ts...>> {
	static_assert(sizeof...(Ts) > 1);
	constexpr static auto index = util::contains<T, Multiplication<Ts...>>::index >= 0 ? 0 : -1;
};*/

template <typename T, typename... Ts>
struct contains_multiple<T, Addition<Ts...>> {
	static_assert(sizeof...(Ts) > 1);
	
	constexpr static auto index = util::tuple_index<Addition<Ts...>, T, fields::detail::is_multiple>::value;
};

/*template <typename T, typename... Fs, typename... Gs>
struct contains_multiple<T, Multiplication<Addition<Fs...>, Gs...>> {
	static_assert(sizeof...(Fs) > 1);

	constexpr static auto index = fields::detail::contains_multiple<T, Addition<typename multiply<Fs, Multiplication<Gs...>>::type...>>::index;
};*/

template <typename T, typename U>
struct contains_sub_multiple;

template <typename T, typename... Ts>
struct contains_sub_multiple<T, Multiplication<Ts...>> {
	static_assert(sizeof...(Ts) > 1);
	constexpr static auto index = util::contains<T, Multiplication<Ts...>>::index >= 0 ? 0 : -1;
};

template <typename T, typename... Ts>
struct contains_sub_multiple<T, Addition<Ts...>> {
	static_assert(sizeof...(Ts) > 1);

	constexpr static auto index = util::tuple_index<Addition<Ts...>, T, fields::detail::is_sub_multiple>::value;
};

/*template <typename T, typename... Fs, typename... Gs>
struct contains_sub_multiple<T, Multiplication<Addition<Fs...>, Gs...>> {
	static_assert(sizeof...(Fs) > 1);

	constexpr static auto index = fields::detail::contains_sub_multiple<T, Addition<typename multiply<Fs, Multiplication<Gs...>>::type...>>::index;
};*/

}   //detail
}   // fields


