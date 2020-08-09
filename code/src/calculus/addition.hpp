#pragma once

#include "multiplication.hpp"

#include "functions/constant.hpp"

// C++ headers
#include <tuple>
#include <utility>

namespace fields {

template <typename... Fs>
class Addition {
	static_assert(sizeof...(Fs) > 1, "Need atleast 2 terms to form an Addition");
	static_assert(((std::is_same_v<Constant<0>, Fs> == false) && ...), "All + 0 terms should be removed");

public:
	template <typename... Ts, typename G>
	constexpr Addition(G lhs, Addition<Ts...> rhs) : fs(std::tuple_cat(std::make_tuple(lhs), rhs.fs)) {
		assert_no_common_factors<G>(rhs);
		static_assert((fields::detail::is_constant<G>::value && (fields::detail::is_constant<Ts>::value || ...)) == false, "Either G should be constant or Ts... should contain 1 Constant, not both.");
	}

	template <typename... Ts, typename G>
	constexpr Addition(Addition<Ts...> lhs, G rhs) : fs(std::tuple_cat(lhs.fs, std::make_tuple(rhs))) {
		assert_no_common_factors<G>(lhs);
		static_assert((fields::detail::is_constant<G>::value && (fields::detail::is_constant<Ts>::value || ...)) == false, "Either G should be constant or Ts... should contain 1 Constant, not both.");
	}

	template <typename... Ts, typename... Us>
	constexpr Addition(Addition<Ts...> lhs, Addition<Us...> rhs) : fs(std::tuple_cat(lhs.fs, rhs.fs)) {
		(assert_no_common_factors<Ts>(rhs), ...);
		static_assert(((fields::detail::is_constant<Ts>::value || ...)&& (fields::detail::is_constant<Us>::value || ...)) == false, "Either G should be constant or Ts... should contain 1 Constant, not both.");
	}

	template <typename F, typename G>
	constexpr Addition(F lhs, G rhs) : fs(std::make_tuple(lhs, rhs)) {
		static_assert(fields::detail::is_multiple<F, G>::value == false, "Common terms should be factored.");
		static_assert(fields::detail::is_constant<G>::value == false, "Constants should be the leading term");
	}

	template <typename... Args, typename = std::enable_if_t<(fields::detail::is_multiplication<Args>::value&& ...)>>
	constexpr Addition(Args... args) : fs(std::forward<Args>(args)...) {
		static_assert((fields::detail::is_multiplication<Args>::value && ...), "This constructor should only be used when multiplying out brackets to attempt factorisation.");
	}

	template <typename... Args>
	constexpr auto operator()(Args... args) const { return sum_impl(components_{}, std::make_tuple(args...)); }

	template <Int I>
	constexpr auto get() const { return std::get<I>(fs); }

	template <Int... Idx>
	constexpr auto get(std::integer_sequence<Int, Idx...>) const {
		static_assert((((Idx < sizeof...(Fs)) && (Idx >= 0)) && ...));

		if constexpr (sizeof...(Idx) == 1) {
			return (..., get<Idx>());
		} else {
			return Addition<std::decay_t<decltype(get<Idx>())>...>(std::make_tuple(get<Idx>()...));
		}
	}

	template <typename F>
	constexpr auto get() const { return std::get<F>(fs); }

	template <Int I, Int J>
	constexpr auto sub_sum() const {
		static_assert(I <= sizeof...(Fs));
		static_assert(J <= sizeof...(Fs));
		static_assert(I < J);
		return subset_impl<I>(std::make_integer_sequence<Int, J - I>{});
	}

	template <Int Idx>
	constexpr auto filter() const {
		constexpr auto remaining_terms = util::filter_indicies(components_{}, util::index_not<Idx>);

		return get(remaining_terms);
	}

	template <Int... Idx>
	constexpr auto filter(std::integer_sequence<Int, Idx...>) const {
		constexpr auto remaining_terms = util::filter_indicies(components_{}, [] (Int idx) { return ((idx != Idx) && ...); });

		return get(remaining_terms);
	}

	template <Int Idx, typename F>
	constexpr auto combine_with_term(F f) const {
		return (get<Idx>() + f) + filter<Idx>();
	}
	
	template <typename F>
	constexpr auto expand_terms_with_factor(F factor) const {
		return Addition<std::decay_t<decltype(factor*get<Fs>())>...>(DontGuardAgainstCommonFactors{}, (factor*get<Fs>())...);
	}

	template <typename D>
	constexpr auto derivative(D d) const { return derivative_impl(d, components_{}); }

private:
	using components_ = std::make_integer_sequence<Int, sizeof...(Fs)>;

	template <typename... Gs> friend class Addition;

	using DontGuardAgainstCommonFactors = std::true_type;
	
	explicit constexpr Addition(std::tuple<Fs...> ts) : fs(ts) {
	}

	template <typename... Args>
	constexpr Addition(DontGuardAgainstCommonFactors, Args... args) : fs(std::forward<Args>(args)...) {
	}

	template <Int... Is, typename Tuple>
	constexpr auto sum_impl(std::integer_sequence<Int, Is...>, Tuple args) const { return (std::apply(get<Is>(), args) + ...); }

	template <typename D, Int... Is>
	constexpr auto derivative_impl(D d, std::integer_sequence<Int, Is...>) const { return (... + (d(get<Is>()))); }

	template <Int I, Int... Is>
	constexpr auto subset_impl(std::integer_sequence<Int, Is...>) const {
		return get(std::integer_sequence<Int, (I + Is)...>{});
	}

	template <typename T, typename... Ts>
	constexpr static auto assert_no_common_factors(Addition<Ts...>) -> void {
		static_assert(((fields::detail::is_multiple<T, Ts>::value == false) && ...), "Common terms should be factored.");
	}

	std::tuple<Fs...> fs;
};

}	// fields


namespace std {

template <typename... Ts>
struct tuple_size<fields::Addition<Ts...>> {
	constexpr static auto value = sizeof...(Ts);
};

template <std::size_t I, typename... Ts>
struct tuple_element<I, fields::Addition<Ts...>> : tuple_element<I, tuple<Ts...>> {
};

}	// std




namespace fields {
namespace detail {

template <typename T>
struct is_addition : std::false_type {};

template <typename... Ts>
struct is_addition<Addition<Ts...>> : std::true_type {};

template <typename Stream, typename T, typename... Rest>
auto print(Stream& os, T first, Rest... rest) -> void;

template <typename Stream, typename T>
auto print(Stream& os, T last) -> void {
	os << last;
}

template <typename Stream, typename T, typename... Rest>
auto print(Stream& os, T first, Rest... rest) -> void {
	os << first << " + ";
	print(os, rest...);
}

}	// detail

template <typename Stream, typename... Fs>
auto operator<<(Stream& os, Addition<Fs...> y) -> std::enable_if_t<(sizeof...(Fs) > 1), Stream&> {
	os << "(";
	fields::detail::print(os, y.template get<Fs>()...);
	os << ")";

	return os;
}

}	// fields
