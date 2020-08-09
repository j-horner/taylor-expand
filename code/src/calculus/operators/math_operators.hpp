#pragma once

#include "../addition.hpp"
#include "../subtraction.hpp"
#include "../multiplication.hpp"
#include "../vector.hpp"

#include "../functions/power.hpp"

#include <algorithm>
#include <type_traits>

namespace fields {

// Main Addition
template <typename F, typename G>
constexpr auto operator+([[maybe_unused]] F lhs, [[maybe_unused]] G rhs) {
	if constexpr (std::is_same_v<F, fields::Constant<0>>) {
		return rhs;
	} else if constexpr (std::is_same_v<G, fields::Constant<0>>) {
		return lhs;
	} else if constexpr (fields::detail::is_constant<G>::value) {
		// constants should be the leading term
		return Addition<G, F>(rhs, lhs);
	} else {
		return Addition<F, G>(lhs, rhs);
	}
}

// Main Subtraction definition
template <typename F, typename G>
constexpr auto operator-([[maybe_unused]] F lhs, [[maybe_unused]] G rhs) {
	if constexpr (std::is_same_v<F, Constant<0>>) {
		return -rhs;
	} else if constexpr (std::is_same_v<G, Constant<0>>) {
		return lhs;
	} else {
		return Subtraction<F, G>{lhs, rhs};
	}
}

// Main Multiplication
template <typename F, typename G>
constexpr auto operator*([[maybe_unused]] F lhs, [[maybe_unused]] G rhs) {
	if constexpr (std::is_same_v<decltype(lhs), Constant<0>> || std::is_same_v<decltype(rhs), Constant<0>>) {
		return Constant<0>{};
	} else if constexpr (std::is_same_v<decltype(lhs), Constant<1>>) {
		return rhs;
	} else if constexpr (std::is_same_v<decltype(rhs), Constant<1>>) {
		return lhs;
	} else if constexpr (::fields::detail::is_constant<G>::value) {
		return rhs*lhs;
	} else {
		return Multiplication<F, G>(lhs, rhs);
	}
}

template <typename F, typename G>
constexpr auto operator/([[maybe_unused]] F lhs, [[maybe_unused]] G rhs) {
	static_assert(false == std::is_same_v<decltype(rhs), Constant<0>>, "Cannot divide by 0!");

	if constexpr (std::is_same_v<decltype(lhs), Constant<0>>) {
		return Constant<0>{};
	} else {
		return lhs * Power<G, -1>{rhs};
	}
}


// Definition of number 2
template <typename F>
constexpr auto operator+(F rhs, F) {
	return Constant<2>{}*rhs;
}



template <typename... Fs, Int A, Int B>
constexpr auto operator+(Constant<A, B> lhs, Addition<Fs...> rhs) {
	if constexpr (A == 0) {
		return rhs;
	} else if constexpr (fields::detail::is_constant<std::tuple_element_t<0, Addition<Fs...>>>::value) {
		return (lhs + rhs.template get<0>()) + rhs.template sub_sum<1, sizeof...(Fs)>();
	} else {
		static_assert((fields::detail::is_constant<Fs>::value || ...) == false, "If Addition contains a constant, it should be the leading element");
		return Addition<Constant<A, B>, Fs...>(lhs, rhs);
	}
}

template <typename... Fs, Int A, Int B>
constexpr auto operator+(Addition<Fs...> lhs, Constant<A, B> rhs) {
	return rhs + lhs;
}

// Addiions with common factors
template <typename F, typename G>
constexpr auto operator+(Addition<G, F> y, F f) {
	return y.template get<0>() + Constant<2>{}*f;
}

template <typename F, typename G>
constexpr auto operator+(F f, Addition<G, F> y) {
	return y.template get<0>() + Constant<2>{}*f;
}

template <typename F, typename... Gs>
constexpr auto operator+(Addition<F, Gs...> y, F f) {
	return y.template sub_sum<1, sizeof...(Gs) + 1>() + Constant<2>{}*f;
}

template <typename F, typename... Gs, typename = std::enable_if_t<false == fields::detail::is_constant<F>::value>>
constexpr auto operator+(F f, Addition<F, Gs...> y) {
	return y.template sub_sum<1, sizeof...(Gs) + 1>() + Constant<2>{}*f;
}

// Additions with unrelated factors
template <typename... Fs, typename G, typename = std::enable_if_t<false == fields::detail::is_power<G, Addition<Fs...>>::value>>
constexpr auto operator+(G g, Addition<Fs...> f) {
	return f + g;
}


namespace detail {

template <typename... Fs, typename... Gs>
constexpr auto search_for_term(Addition<Fs...> addition, Multiplication<Gs...> multiplication) {

	const auto exact_mapped_terms = util::map_terms<util::is_same>(multiplication, addition);
	const auto exact_terms_found = std::get<2>(exact_mapped_terms);

	if constexpr (exact_terms_found) {
		constexpr auto term_locations = std::get<1>(exact_mapped_terms);
		return util::get_first<std::decay_t<decltype(term_locations)>>{};
	} else {
		const auto multiple_mapped_terms = util::map_terms<fields::detail::is_multiple>(multiplication, addition);
		const auto multiple_terms_found = std::get<2>(multiple_mapped_terms);

		if constexpr (multiple_terms_found) {
			const auto term_locations = std::get<1>(multiple_mapped_terms);
			return util::get_first<std::decay_t<decltype(term_locations)>>{};
		} else {
			return std::false_type{};
		}
	}
}

template <typename... Fs, typename... Gs>
constexpr auto factorise(Addition<Fs...> addition, Multiplication<Gs...> multiple) {
	const auto term_location = search_for_term(addition, multiple);

	if constexpr (std::is_same_v<std::decay_t<decltype(term_location)>, std::false_type>) {
		return Addition<Fs..., Multiplication<Gs...>>(addition, multiple);
	} else {
		return (addition.template get<term_location.index>() + multiple) + addition.template filter<term_location.index>();
	}
}

}	 //detail

template <typename G, typename... Fs, typename = std::enable_if_t<false == fields::detail::is_power<G, Addition<Fs...>>::value>>
constexpr auto operator+(Addition<Fs...> f, [[maybe_unused]] G g) {
	if constexpr (std::is_same_v<G, Constant<0>>) {
		return f;
	} else {
		
		constexpr auto location_in_pack = util::contains<G, Addition<Fs...>>::index;

		if constexpr (location_in_pack >= 0) {
			return f.template combine_with_term<location_in_pack>(g);
		} else {
			static_assert(-1 == location_in_pack);

			constexpr auto subtraction_in_pack = fields::detail::contains_subtraction<G, Addition<Fs...>>::index;

			if constexpr (subtraction_in_pack >= 0) {
				return f.template combine_with_term<subtraction_in_pack>(g);
			} else {
				static_assert(-1 == subtraction_in_pack);

				constexpr auto multiple_location_in_pack = fields::detail::contains_multiple<G, Addition<Fs...>>::index;

				if constexpr (multiple_location_in_pack >= 0) {
					return f.template combine_with_term<multiple_location_in_pack>(g);
				} else {
					static_assert(-1 == multiple_location_in_pack);
					// static_assert(((fields::detail::is_sub_multiple<G, Fs>::value == false) && ...), "Common terms should be factored.");
					static_assert(((fields::detail::is_multiple<G, Fs>::value == false) && ...), "Common terms should be factored.");

					if constexpr (fields::detail::is_multiplication<G>::value) {
						// first check to see if the Addition is actually in the Multiplication in the RHS
						constexpr auto location_in_multiple = util::contains<Addition<Fs...>, G>::index;

						if constexpr (location_in_multiple >= 0) {
							return f*(Constant<1>{} + g.template filter<location_in_multiple>());
						} else {
							static_assert(location_in_multiple == -1);

							constexpr auto power_location_in_multiple = util::tuple_index<G, Addition<Fs...>, fields::detail::is_power>::value;

							if constexpr (power_location_in_multiple >= 0) {
								return f*(Constant<1>{} + (g/f));
							} else {
								static_assert(power_location_in_multiple == -1);
								return fields::detail::factorise(f, g);
							}
						}
					} else {
						return Addition<Fs..., G>(f, g);
					}
				}
			}
		}
	}
}

template <typename F, typename G>
constexpr auto operator+(Subtraction<G, F> y, F) { return y.lhs; }

template <typename F, typename G>
constexpr auto operator+(F f, Subtraction<G, F> y) { return y.lhs; }

template <typename F, typename G>
constexpr auto operator+(Subtraction<F, G> y, F f) {
	return Constant<2>{}*f - y.rhs;
}

template <typename F, typename G>
constexpr auto operator+(F f, Subtraction<F, G> y) {
	return Constant<2>{}*f - y.rhs;
}

template <typename F, typename G, typename H>
constexpr auto operator+(Addition<G, F> lhs, Subtraction<H, F> rhs) { return lhs.template get<0>() + rhs.lhs; }

template <typename F, typename G, typename H>
constexpr auto operator+(Subtraction<H, F> lhs, Addition<G, F> rhs) { return lhs.lhs + rhs.template get<0>(); }

template <typename F, typename G, typename... Gs>
constexpr auto operator+(Addition<F, Gs...> lhs, Subtraction<G, F> rhs) { return lhs.template sub_sum<1, sizeof...(Gs) + 1>() + rhs.lhs; }

template <typename F, typename G, typename... Gs>
constexpr auto operator+(Subtraction<G, F> lhs, Addition<F, Gs...> rhs) { return lhs.lhs + rhs.template sub_sum<1, sizeof...(Gs) + 1>(); }

template <typename F, typename G, typename H>
constexpr auto operator+(Subtraction<G, F> lhs, Subtraction<F, H> rhs) { return lhs.lhs - rhs.rhs; }

template <typename F, typename G, typename H>
constexpr auto operator+(Subtraction<F, G> lhs, Subtraction<H, F> rhs) { return rhs.lhs - lhs.rhs; }

template <typename F, typename G, typename H>
constexpr auto operator+(Subtraction<F, G> lhs, H rhs) { return (lhs.lhs + rhs) - lhs.rhs; }

template <typename F, typename G, typename H>
constexpr auto operator+(H lhs, Subtraction<F, G> rhs) { return (lhs + rhs.lhs) - rhs.rhs; }

template <typename A, typename B, typename C, typename D>
constexpr auto operator+(Subtraction<A, B> lhs, Subtraction<C, D> rhs) { return (lhs.lhs + rhs.lhs) - (lhs.rhs + rhs.rhs); }

template <typename F, typename G>
constexpr auto operator+(Multiplication<G, F> y, F f) {
	return (y.template get<0>() + Constant<1>{})* f;
}

template <typename F, typename G>
constexpr auto operator+(F f, Multiplication<G, F> y) {
	return (Constant<1>{} +y.template get<0>())* f;
}

template <typename F, typename... Gs, typename = std::enable_if_t<fields::detail::is_addition<F>::value == false>>
constexpr auto operator+(Multiplication<F, Gs...> y, F f) {
	return f * (y.template sub_product<1, sizeof...(Gs) + 1>() + Constant<1>{});
}

template <typename F, typename... Gs, typename = std::enable_if_t<fields::detail::is_addition<F>::value == false>>
constexpr auto operator+(F f, Multiplication<F, Gs...> y) {
	return f * (Constant<1>{} +y.template sub_product<1, sizeof...(Gs) + 1>());
}

template <typename F, typename... Gs>
constexpr auto operator+(Multiplication<Gs...> lhs, Multiplication<F, Gs...> rhs) {
	return (Constant<1>{} +rhs.template get<0>())* lhs;
}

template <typename F, typename... Gs>
constexpr auto operator+(Multiplication<F, Gs...> lhs, Multiplication<Gs...> rhs) {
	return (lhs.template get<0>() + Constant<1>{})* rhs;
}

template <typename F, typename... Gs>
constexpr auto operator+(Multiplication<F, Gs...> lhs, Multiplication<F, Gs...> rhs) {
	return (Constant<2>{}*lhs.template get<0>())* rhs.template sub_product<1, sizeof...(Gs) + 1>();
}

namespace detail {

template <typename T>
struct any_found_terms_in_rhs;

template <Int... idx>
struct any_found_terms_in_rhs<std::integer_sequence<Int, idx...>> {
	constexpr static auto value = ((idx >= 0) || ...);
};

template <typename T, typename U>
struct found_term_indicies;

template <Int... idx, Int... location>
struct found_term_indicies<std::integer_sequence<Int, idx...>, std::integer_sequence<Int, location...>> {
	static_assert(sizeof...(idx) == sizeof...(location));

	using type = std::integer_sequence<Int, ((location >= 0) ? idx : -1)...>;
};

template <Int... idx, typename... Fs>
constexpr auto remove_terms(std::integer_sequence<Int, idx...> indicies, Multiplication<Fs...> pack) {
	static_assert(sizeof...(idx) > 0, "Cannot remove 0 terms from 'pack'");
	static_assert(sizeof...(idx) <= sizeof...(Fs), "Trying to remove more terms from Multiplication<Fs...> than it has.");

	if constexpr (sizeof...(idx) == sizeof...(Fs)) {
		return Constant<1>{};
	} else {
		static_assert(((idx >= 0) && ...), "Can only remove terms which exist in 'pack'. Require all idx >= 0.");
		static_assert(((idx < sizeof...(Fs)) && ...), "Can only remove terms which exist in 'pack'.");

		constexpr auto is_index_not_in_list_of_terms_to_be_removed = [] (auto i) { return ((i != idx) && ...); };

		constexpr auto terms_to_keep = util::filter_indicies(std::make_integer_sequence<Int, sizeof...(Fs)>{}, is_index_not_in_list_of_terms_to_be_removed);

		return pack.get(terms_to_keep);
	}
}

template <Int... idx, typename... Fs>
constexpr auto remove_terms(std::integer_sequence<Int, idx...> indicies, Addition<Fs...> pack) {
	static_assert(sizeof...(idx) <= sizeof...(Fs), "Trying to remove more terms from Addition<Fs...> than it has.");

	constexpr auto is_index_not_in_list_of_terms_to_be_removed = [] (auto i) { return ((i != idx) && ...); };

	constexpr auto terms_to_keep = util::filter_indicies(std::make_integer_sequence<Int, sizeof...(Fs)>{}, is_index_not_in_list_of_terms_to_be_removed);

	return pack.get(terms_to_keep);
}

template <Int... idx, Int... location, typename... Fs, typename... Gs>
constexpr auto group_terms(std::integer_sequence<Int, idx...> indicies, std::integer_sequence<Int, location...> locations, Multiplication<Fs...> lhs, Multiplication<Gs...> rhs) {
	static_assert(sizeof...(idx) == sizeof...(location));
	static_assert(sizeof...(idx) > 0);

	return (lhs.get(indicies))*(remove_terms(indicies, lhs) + remove_terms(locations, rhs));
}

template <typename Indicies, typename Locations, typename LHS, typename RHS>
struct find_smallest_powers;

template <Int... idx, Int... location, typename... Fs, typename... Gs>
struct find_smallest_powers<std::integer_sequence<Int, idx...>, std::integer_sequence<Int, location...>, Multiplication<Fs...>, Multiplication<Gs...>> {

	using type = std::integer_sequence<Int, fields::detail::smallest_power<std::tuple_element_t<idx, Multiplication<Fs...>>,
																		   std::tuple_element_t<location, Multiplication<Gs...>>>::value...>;
};

template <Int P, typename F>
struct GetPowerType;

template <typename F>
struct GetPowerType<1, F> {
	using type = F;
};

template <typename F, Int N>
struct GetPowerType<1, Power<F, N>> {
	using type = F;
};

template <Int P, typename F, Int N>
struct GetPowerType<P, Power<F, N>> {
	using type = Power<F, P>;
};

template <Int P, typename F>
struct GetPowerType {
	static_assert(fields::detail::is_power<F, F>::value == false);
	using type = Power<F, P>;
};

template <Int... powers, typename... Fs>
constexpr auto get_powers(std::integer_sequence<Int, powers...>, Multiplication<Fs...> terms) {

	static_assert(sizeof...(powers) == sizeof...(Fs));

	// return Multiplication<std::conditional_t<powers == 1, Fs, Power<Fs, powers>>...>(terms);
	return Multiplication<typename GetPowerType<powers, Fs>::type...>(terms);
}

template <Int power, typename F, Int N>
constexpr auto get_powers(std::integer_sequence<Int, power>, Power<F, N> f) {
	if constexpr (power == 1) {
		return f.f();
	} else {
		static_assert(power <= N);
		return Power<F, power>{f};
	}
}

template <typename F, typename = std::enable_if_t<false == fields::detail::is_power<F, F>::value>>
constexpr auto get_powers(std::integer_sequence<Int, 1>, F f) {
	return f;
}

template <Int... power, typename... Fs>
constexpr auto remove_powers(std::integer_sequence<Int, power...> powers, Multiplication<Fs...> terms) {
	static_assert(sizeof...(power) == sizeof...(Fs));

	return terms/get_powers(powers, terms);
}

template <Int power, typename F, Int N>
constexpr auto remove_powers(std::integer_sequence<Int, power> powers, Power<F, N> f) {
	if constexpr (power == N) {
		return Constant<1>{};
	} else if constexpr (N - power == 1) {
		return f.f();
	} else {
		return Power<F, N - power>{f};
	}
}

template <typename F, typename = std::enable_if_t<false == fields::detail::is_power<F, F>::value>>
constexpr auto remove_powers(std::integer_sequence<Int, 1>, F) {
	return Constant<1>{};
}


template <Int... idx, Int... location, typename... Fs, typename... Gs>
constexpr auto group_powers(std::integer_sequence<Int, idx...> indicies, std::integer_sequence<Int, location...> locations, Multiplication<Fs...> lhs, Multiplication<Gs...> rhs) {
	static_assert(sizeof...(idx) == sizeof...(location));
	static_assert(sizeof...(idx) > 0);

	using powers_to_factor = typename find_smallest_powers<std::integer_sequence<Int, idx...>, std::integer_sequence<Int, location...>, Multiplication<Fs...>, Multiplication<Gs...>>::type;

	const auto common_powers_in_lhs = lhs.get(indicies);
	const auto common_powers_in_rhs = rhs.get(locations);

	return get_powers(powers_to_factor{}, common_powers_in_lhs)*(remove_powers(powers_to_factor{}, common_powers_in_lhs)*remove_terms(indicies, lhs) + remove_powers(powers_to_factor{}, common_powers_in_rhs)*remove_terms(locations, rhs));
}

/*template <typename F, typename... Gs>
constexpr auto expand_terms(F factor, Addition<Gs...> sum) {
	return Addition<std::decay_t<decltype(factor*sum.template get<Gs>())>...>((factor*sum.template get<Gs>())...);
}*/

template <Int Addition_Location, typename... Fs>
constexpr auto expand_terms(Multiplication<Fs...> pack) {
	constexpr auto indicies = std::make_integer_sequence<Int, sizeof...(Fs)>{};
	constexpr auto remaining_terms = util::filter_indicies(indicies, util::index_not<Addition_Location>);

	const auto addition = pack.template get<Addition_Location>();

	static_assert(fields::detail::is_addition<std::decay_t<decltype(addition)>>::value, "This function should only have been called if pack contained an Addition");

	return addition.expand_terms_with_factor(pack.get(remaining_terms));
	// return expand_terms(pack.get(remaining_terms), addition);
}

template <typename... Fs, typename... Gs, Int Rhs_Idx, Int... Rhs_Indicies>
constexpr auto expand_and_simplify(Addition<Fs...> expanded_lhs, Multiplication<Gs...> rhs, std::integer_sequence<Int, Rhs_Idx, Rhs_Indicies...>) {

	const auto expanded_rhs = fields::detail::expand_terms<Rhs_Idx>(rhs);

	const auto exact_map_result = fields::util::map_terms<util::is_same>(expanded_lhs, expanded_rhs);
	const auto exact_common_terms_exist = std::get<2>(exact_map_result);

	if constexpr (exact_common_terms_exist.value) {
		return expanded_lhs + expanded_rhs;
	} else {

		const auto multiple_map_result = fields::util::map_terms<fields::detail::is_multiple>(expanded_lhs, expanded_rhs);
		const auto multiple_common_terms_exist = std::get<2>(multiple_map_result);

		if constexpr (multiple_common_terms_exist) {
			return expanded_lhs + expanded_rhs;
		} else {
			if constexpr (sizeof...(Rhs_Indicies) == 0) {
				return std::false_type{};
			} else {
				return expand_and_simplify(expanded_lhs, rhs, std::integer_sequence<Int, Rhs_Indicies...>{});
			}
		}
	}
}

template <typename... Fs, typename... Gs, Int Lhs_Idx, Int... Lhs_Indicies, Int... Rhs_Indicies>
constexpr auto expand_and_simplify(Multiplication<Fs...> lhs, std::integer_sequence<Int, Lhs_Idx, Lhs_Indicies...>, Multiplication<Gs...> rhs, std::integer_sequence<Int, Rhs_Indicies...> rhs_addition_locations) {

	const auto expanded_lhs = fields::detail::expand_terms<Lhs_Idx>(lhs);

	const auto result = expand_and_simplify(expanded_lhs, rhs, rhs_addition_locations);

	if constexpr (std::is_same_v<std::decay_t<decltype(result)>, std::false_type>) {
		if constexpr (sizeof...(Lhs_Indicies) == 0) {
			return std::false_type{};
		} else {
			return expand_and_simplify(lhs, std::integer_sequence<Int, Lhs_Indicies...>{}, rhs, rhs_addition_locations);
		}
	} else {
		return result;
	}
}


template <typename... Fs, typename... Gs, Int... Lhs_Indicies, Int... Rhs_Indicies>
constexpr auto expand_terms_in_all_possible_combinations(Multiplication<Fs...> lhs, std::integer_sequence<Int, Lhs_Indicies...> lhs_addition_locations, Multiplication<Gs...> rhs, std::integer_sequence<Int, Rhs_Indicies...> rhs_addition_locations) {

	if constexpr ((sizeof...(Lhs_Indicies) == 0) || (sizeof...(Rhs_Indicies) == 0)) {
		// no terms to multiply out, return standard addition
		return Addition<Multiplication<Fs...>, Multiplication<Gs...>>(lhs, rhs);
	} else {
		const auto result = expand_and_simplify(lhs, lhs_addition_locations, rhs, rhs_addition_locations);
		if constexpr (std::is_same_v<std::decay_t<decltype(result)>, std::false_type>) {
			return Addition<Multiplication<Fs...>, Multiplication<Gs...>>(lhs, rhs);
		} else {
			return result;
		}
	}

}

template <typename... Fs>
struct is_term_addition;

template <typename... Fs>
struct is_term_addition<Multiplication<Fs...>> {

	template <typename idx>
	using predicate = fields::detail::is_addition<std::tuple_element_t<idx::value, Multiplication<Fs...>>>;
};

}	// detail

template <typename... Fs, typename... Gs>
constexpr auto operator+(Multiplication<Fs...> lhs, Multiplication<Gs...> rhs) {

	static_assert(sizeof...(Fs) > 0);
	static_assert(sizeof...(Gs) > 0);

	if constexpr (util::is_same<Multiplication<Fs...>, Multiplication<Gs...>>::value) {
		return Constant<2>{}*lhs;
	} else {
		
		const auto same_mapping = fields::util::map_terms<util::is_same>(lhs, rhs);
		
		const auto terms_successfully_found = std::get<2>(same_mapping);

		if constexpr (terms_successfully_found.value) {
			const auto mapped_terms = std::get<0>(same_mapping);
			const auto mapped_locations = std::get<1>(same_mapping);
		
			return fields::detail::group_terms(mapped_terms, mapped_locations, lhs, rhs);
		} else {

			const auto power_mapping = fields::util::map_terms<fields::detail::is_power>(lhs, rhs);

			const auto powers_successfully_found = std::get<2>(power_mapping);

			if constexpr (powers_successfully_found.value) {
				const auto mapped_terms = std::get<0>(power_mapping);
				const auto mapped_locations = std::get<1>(power_mapping);

				return fields::detail::group_powers(mapped_terms, mapped_locations, lhs, rhs);
			} else {

				constexpr auto lhs_indicies = std::make_integer_sequence<Int, sizeof...(Fs)>{};
				constexpr auto rhs_indicies = std::make_integer_sequence<Int, sizeof...(Gs)>{};

				constexpr auto lhs_addition_locations = util::filter_indicies<fields::detail::is_term_addition<Multiplication<Fs...>>::template predicate>(lhs_indicies);
				constexpr auto rhs_addition_locations = util::filter_indicies<fields::detail::is_term_addition<Multiplication<Gs...>>::template predicate>(rhs_indicies);

				return fields::detail::expand_terms_in_all_possible_combinations(lhs, lhs_addition_locations, rhs, rhs_addition_locations);








				
				// multiply out brackets to try and find common factors
				/*constexpr auto lhs_addition_location = util::tuple_find<std::tuple<Fs...>, fields::detail::is_addition>::value;
				constexpr auto rhs_addition_location = util::tuple_find<std::tuple<Gs...>, fields::detail::is_addition>::value;

				if constexpr ((-1 != lhs_addition_location) && (-1 != rhs_addition_location)) {

					const auto expanded_lhs = fields::detail::expand_terms<lhs_addition_location>(lhs);
					const auto expanded_rhs = fields::detail::expand_terms<rhs_addition_location>(rhs);

					const auto exact_map_result = fields::util::map_terms<util::is_same>(expanded_lhs, expanded_rhs);
					const auto exact_common_terms_exist = std::get<2>(exact_map_result);

					if constexpr (exact_common_terms_exist.value) {
						return expanded_lhs + expanded_rhs;
					} else {
						
						const auto multiple_map_result = fields::util::map_terms<fields::detail::is_multiple>(expanded_lhs, expanded_rhs);
						const auto multiple_common_terms_exist = std::get<2>(multiple_map_result);

						if constexpr (multiple_common_terms_exist) {
							return expanded_lhs + expanded_rhs;
						} else {
							return Addition<Multiplication<Fs...>, Multiplication<Gs...>>(lhs, rhs);
						}
					}
				}*/ /*else if constexpr (-1 != lhs_addition_location) {

					const auto expanded_lhs = fields::detail::expand_terms<lhs_addition_location>(lhs);

					const auto exact_terms_found = std::get<2>(util::map_terms<util::is_same>(rhs, expanded_lhs));
					const auto multiple_terms_found = std::get<2>(util::map_terms<fields::detail::is_multiple>(rhs, expanded_lhs));

					if constexpr (exact_terms_found || multiple_terms_found) {
						return expanded_lhs + rhs;
					} else {
						return Addition<Multiplication<Fs...>, Multiplication<Gs...>>(lhs, rhs);
					}
				} else if constexpr (-1 != rhs_addition_location) {

					constexpr auto expanded_rhs = fields::detail::expand_terms<rhs_addition_location>(rhs);

					const auto exact_terms_found = std::get<2>(util::map_terms<util::is_same>(lhs, expanded_rhs));
					const auto multiple_terms_found = std::get<2>(util::map_terms<fields::detail::is_multiple>(lhs, expanded_rhs));

					if constexpr (exact_terms_found || multiple_terms_found) {
						return expanded_rhs + lhs;
					} else {
						return Addition<Multiplication<Fs...>, Multiplication<Gs...>>(lhs, rhs);
					}
				}*//* else {
					return Addition<Multiplication<Fs...>, Multiplication<Gs...>>(lhs, rhs);
				}*/
			}
		}
	}
}

template <typename... Fs, typename F, typename = std::enable_if_t<fields::detail::is_addition<F>::value == false>>
constexpr auto operator+(F lhs, Multiplication<Fs...> rhs) {
	
	if constexpr (std::is_same_v<F, Constant<0>>) {
		return rhs;
	} else {
		{
			using location = util::contains<F, Multiplication<Fs...>>;
	
			if constexpr (-1 != location::index) {
				const auto remaining_terms = util::filter_indicies(std::make_integer_sequence<Int, sizeof...(Fs)>{}, [] (auto idx) { return idx != location::index; });

				return lhs*(Constant<1>{} + rhs.get(remaining_terms));
			} else {
				/*using power_location = fields::detail::contains_power<F, Multiplication<Fs...>>;

				if constexpr (-1 != power_location::index) {
					const auto remaining_terms = util::filter_indicies(std::make_integer_sequence<Int, sizeof...(Fs)>{}, [] (auto idx) { return idx != power_location::index; });

					if constexpr (fields::detail::is_power<F, F>::value) {
						using power_value = fields::detail::power_value<F, std::tuple_element_t<power_location::index, Multiplication<Fs...>>>;

						using min_exponent = Constant<std::min(lhs.exponent(), power_value::value)>;

						const auto common_factor = ((lhs.f())^Constant<-min_exponent{}>{});

						return ((lhs.f())^Constant<min_exponent{}>{})*((lhs*common_factor) + (rhs.template get<power_location::index>()*common_factor)*rhs.get(remaining_terms));
					} else {
						return lhs*(Constant<1>{} + (rhs.template get<power_location::index>()*Power<F, -1>(lhs))*rhs.template get(remaining_terms));
					}
				} else {
					return Addition<F, Multiplication<Fs...>>(lhs, rhs);
				}*/

				return Addition<F, Multiplication<Fs...>>(lhs, rhs);
			}
		}
	}
}

template <typename... Fs, typename F, typename = std::enable_if_t<fields::detail::is_addition<F>::value == false>>
constexpr auto operator+(Multiplication<Fs...> lhs, F rhs) {
	return rhs + lhs;
}

namespace detail {

template <Int... FoundTerms, Int... TermLocations, typename... Fs, typename... Gs>
constexpr auto contract_terms(std::integer_sequence<Int, FoundTerms...> lhs_terms, std::integer_sequence<Int, TermLocations...> rhs_terms, Addition<Fs...> lhs, Addition<Gs...> rhs) {
	static_assert(sizeof...(FoundTerms) == sizeof...(TermLocations));
	static_assert(sizeof...(FoundTerms) <= sizeof...(Fs));
	static_assert(sizeof...(TermLocations) <= sizeof...(Gs));
	return ((lhs.template get<FoundTerms>() + rhs.template get<TermLocations>()) + ...) + lhs.filter(lhs_terms) + rhs.filter(rhs_terms);
}

}	// detail

template <typename... Fs, typename... Gs>
constexpr auto operator+(Addition<Fs...> lhs, Addition<Gs...> rhs) {
	if constexpr (util::is_same<Addition<Fs...>, Addition<Gs...>>::value) {
		return Constant<2>{}*lhs;
	} else {
		// static_assert(false, "This implementation will not work for the case (expanded_lhs + expanded_rhs) because of all the checks against common factors in the Addition<Fs...> constructors. Should check for common factors here THEN call the final Addition constructor.");
		
		const auto same_mapping = fields::util::map_terms<util::is_same>(lhs, rhs);

		const auto terms_successfully_found = std::get<2>(same_mapping);

		if constexpr (terms_successfully_found.value) {
			const auto mapped_terms = std::get<0>(same_mapping);
			const auto mapped_locations = std::get<1>(same_mapping);

			return fields::detail::contract_terms(mapped_terms, mapped_locations, lhs, rhs);
		} else {
			const auto multiple_mapping = fields::util::map_terms<fields::detail::is_multiple>(lhs, rhs);

			const auto multiples_successfully_found = std::get<2>(multiple_mapping);

			if constexpr (multiples_successfully_found.value) {
				const auto mapped_terms = std::get<0>(multiple_mapping);
				const auto mapped_locations = std::get<1>(multiple_mapping);

				return fields::detail::contract_terms(mapped_terms, mapped_locations, lhs, rhs);
			} else {
				return Addition<Fs..., Gs...>(lhs, rhs);
			}
		}


		// return (lhs + rhs.template get<0>()) + rhs.template sub_sum<1, sizeof...(Gs)>();
	}
}


template <typename... Ts, typename... Us>
constexpr auto operator+(Vector<Ts...> v, Vector<Us...> u) -> std::enable_if_t<false == ((std::is_same_v<Ts, Us> && ...)), decltype(v.addition(u))> {
	static_assert(sizeof...(Ts) == sizeof...(Us));
	return v.addition(u);
}

/*
template <typename... Fs, typename... Gs, typename H, auto multiple_location = fields::detail::contains_sub_multiple<H, Multiplication<Addition<Fs...>, Gs...>>::index, typename = std::enable_if_t<multiple_location >= 0>>
constexpr auto operator+(Multiplication<Addition<Fs...>, Gs...> lhs, H rhs) {
	static_assert(multiple_location >= 0);
	
	static_assert(sizeof...(Fs) > 1);
	static_assert(sizeof...(Gs) > 0);

	auto summation = lhs.template get<0>();
	auto multiplier = lhs.template sub_product<1, sizeof...(Gs) + 1>();

	constexpr auto remaining_terms = util::filter_indicies(std::make_integer_sequence<Int, sizeof...(Fs)>{}, [] (auto idx) { return idx != multiple_location; });

	return (summation.template get<multiple_location>()*multiplier + rhs) + summation.get(remaining_terms)*multiplier;
}
*/

namespace detail {

template <Int... idx, Int... location, typename... Fs, typename... Gs, typename... Hs>
constexpr auto group_terms(std::integer_sequence<Int, idx...> indicies, std::integer_sequence<Int, location...> locations, Multiplication<Addition<Fs...>, Gs...> lhs, Addition<Hs...> rhs) {
	static_assert(sizeof...(idx) == sizeof...(location));
	static_assert(sizeof...(idx) > 0);

	auto summation = lhs.template get<0>();
	auto multiplier = lhs.template sub_product<1, sizeof...(Gs) + 1>();


	return ((summation.template get<location>()*multiplier + rhs.template get<idx>()) + ...) + remove_terms(locations, summation)*multiplier + remove_terms(indicies, rhs);
}

}

/*
template <typename... Fs, typename... Gs, typename... Hs,
			typename found_multiple_locations = std::integer_sequence<Int, fields::detail::contains_sub_multiple<Hs, Multiplication<Addition<Fs...>, Gs...>>::index...>,
			typename enabled = std::enable_if_t<fields::detail::any_found_terms_in_rhs<found_multiple_locations>::value>>
constexpr auto operator+(Multiplication<Addition<Fs...>, Gs...> lhs, Addition<Hs...> rhs) {

	static_assert(sizeof...(Fs) > 1);
	static_assert(sizeof...(Gs) > 0);
	static_assert(sizeof...(Hs) > 1);

	constexpr auto is_term_found = [] (auto idx) { return idx >= 0; };

	constexpr auto term_indicies = util::filter_indicies(typename fields::detail::found_term_indicies<std::make_integer_sequence<Int, sizeof...(Hs)>, found_multiple_locations>::type{}, is_term_found);

	constexpr auto term_locations = util::filter_indicies(found_multiple_locations{}, is_term_found);

	return fields::detail::group_terms(term_indicies, term_locations, lhs, rhs);
}

template <typename... Fs, typename... Gs, typename... Hs,
	typename found_multiple_locations = std::integer_sequence<Int, fields::detail::contains_sub_multiple<Hs, Multiplication<Addition<Fs...>, Gs...>>::index...>,
	typename enabled = std::enable_if_t<fields::detail::any_found_terms_in_rhs<found_multiple_locations>::value>>
constexpr auto operator+(Addition<Hs...> lhs, Multiplication<Addition<Fs...>, Gs...> rhs) {

	static_assert(sizeof...(Fs) > 1);
	static_assert(sizeof...(Gs) > 0);
	static_assert(sizeof...(Hs) > 1);

	return rhs + lhs;
}

template <typename... Fs, typename... Gs, typename H, auto multiple_location = fields::detail::contains_sub_multiple<H, Multiplication<Addition<Fs...>, Gs...>>::index, typename = std::enable_if_t<multiple_location >= 0>>
constexpr auto operator+(H lhs, Multiplication<Addition<Fs...>, Gs...> rhs) {
	return rhs + lhs;
}
*/

















// -f = (-1)*f
template <typename F>
constexpr auto operator-(F f) {
	return Constant<-1>{}*f;
}

// Definition of 0
template <typename F>
constexpr auto operator-(F, F) {
	return Constant<0>{};
}

// Addition with common factors
template <typename F, typename G>
constexpr auto operator-(Addition<G, F> y, F) { return y.template get<0>(); }

template <typename F, typename G>
constexpr auto operator-(F, Addition<G, F> y) { return -y.template get<0>(); }

template <typename F, typename... Gs>
constexpr auto operator-(Addition<F, Gs...> y, F) { return y.template sub_sum<1, sizeof...(Gs) + 1>(); }

template <typename F, typename... Gs>
constexpr auto operator-(F, Addition<F, Gs...> y) { return -y.template sub_sum<1, sizeof...(Gs) + 1>(); }

template <typename F, typename... Gs>
constexpr auto operator-(Addition<F, Gs...> lhs, Addition<Gs...> rhs) { return lhs.template get<0>(); }

template <typename F, typename... Gs>
constexpr auto operator-(Addition<Gs...> lhs, Addition<F, Gs...> rhs) { return -rhs.template get<0>(); }

template <typename F, typename G, typename... Gs>
constexpr auto operator-(Addition<G, F> lhs, Addition<F, Gs...> rhs) { return lhs.template get<0>() - rhs.template sub_sum<1, sizeof...(Gs) + 1>(); }

template <typename F, typename G, typename... Gs>
constexpr auto operator-(Addition<F, Gs...> lhs, Addition<G, F> rhs) { return lhs.template sub_sum<1, sizeof...(Gs) + 1>() - rhs.template get<0>(); }

template <typename F, typename G, typename H>
constexpr auto operator-(Addition<G, F> lhs, Addition<H, F> rhs) { return lhs.template get<0>() - rhs.template get<0>(); }




// Subtractions with common factors
template <typename F, typename G>
constexpr auto operator-(Subtraction<G, F> y, F f) {
	using namespace literals;
	return y.lhs - 2_c * f;
}

template <typename F, typename G>
constexpr auto operator-(F f, Subtraction<G, F> y) {
	using namespace literals;
	return 2_c * f - y.lhs;
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
	return (y.template get<0>() - 1_c) * f;
}

template <typename F, typename G>
constexpr auto operator-(F f, Multiplication<G, F> y) {
	using namespace literals;
	return (1_c - y.template get<0>()) * f;
}

template <typename F, typename... Gs>
constexpr auto operator-(Multiplication<F, Gs...> y, F f) {
	using namespace literals;
	return f * (y.template sub_product<1, sizeof...(Gs) + 1>() - 1_c);
}

template <typename F, typename... Gs>
constexpr auto operator-(F f, Multiplication<F, Gs...> y) {
	using namespace literals;
	return f * (1_c - y.template sub_product<1, sizeof...(Gs) + 1>());
}

template <typename F, typename... Gs>
constexpr auto operator-(Multiplication<Gs...> lhs, Multiplication<F, Gs...> rhs) {
	using namespace literals;
	return (1_c - rhs.template get<0>()) * lhs;
}

template <typename F, typename... Gs>
constexpr auto operator-(Multiplication<F, Gs...> lhs, Multiplication<Gs...> rhs) {
	using namespace literals;
	return (lhs.template get<0>() - 1_c) * rhs;
}

template <typename F, typename G, typename... Hs>
constexpr auto operator-(Multiplication<F, Hs...> lhs, Multiplication<G, Hs...> rhs) {
	return (lhs.template get<0>() - rhs.template get<0>()) * lhs.template sub_product<1, sizeof...(Hs) + 1>();
}


template <typename... Fs, typename G>
constexpr auto operator-(Addition<Fs...> lhs, G rhs) {
	constexpr auto location = util::contains<G, Addition<Fs...>>::index;

	if constexpr (-1 == location) {
		constexpr auto subtraction_location = fields::detail::contains_subtraction<G, Addition<Fs...>>::index;

		if constexpr (-1 == subtraction_location) {
			constexpr auto multiple_location = fields::detail::contains_multiple<G, Addition<Fs...>>::index;

			if constexpr (-1 == multiple_location) {
				return Subtraction<Addition<Fs...>, G>{lhs, rhs};
			} else {
				if constexpr (0 == multiple_location) {
					return (lhs.template get<0>() - rhs) + lhs.template sub_sum<1, sizeof...(Fs)>();
				} else if constexpr (sizeof...(Fs) - 1 == multiple_location) {
					return lhs.template sub_sum<0, sizeof...(Fs) - 1>() + (lhs.template get<multiple_location>() - rhs);
				} else {
					return lhs.template sub_sum<0, multiple_location>() + (lhs.template get<multiple_location>() - rhs) + lhs.template sub_sum<multiple_location + 1, sizeof...(Fs)>();
				}
			}
		} else {
			if constexpr (0 == subtraction_location) {
				return (lhs.template get<0>() - rhs) + lhs.template sub_sum<1, sizeof...(Fs)>();
			} else if constexpr (sizeof...(Fs) - 1 == subtraction_location) {
				return lhs.template sub_sum<0, sizeof...(Fs) - 1>() + (lhs.template get<subtraction_location>() - rhs);
			} else {
				lhs.template sub_sum<0, subtraction_location>() + (lhs.template get<subtraction_location>() - rhs) + lhs.template sub_sum<subtraction_location + 1, sizeof...(Fs)>();
			}
		}
	} else {
		if constexpr (0 == location) {
			return lhs.template sub_sum<1, sizeof...(Fs)>();
		} else if constexpr (sizeof...(Fs) - 1 == location) {
			return lhs.template sub_sum<0, sizeof...(Fs) - 1>();
		} else {
			return lhs.template sub_sum<0, location>() + lhs.template sub_sum<location + 1, sizeof...(Fs)>();
		}
	}
}
























// Constant - any multiplications with Constant on the right will be moved so it's on the left, also ensure all constants are moved to the front through multiplications

// TODO - Multiplication<F, Constant> should never happen...
template <Int A, Int B, Int C, Int D, typename F>
constexpr auto operator*(Constant<A, B> c, Multiplication<F, Constant<C, D>> y) {
	static_assert(sizeof(F) == 0);
	return (c * y.template get<1>()) * y.template get<0>();
}

template <typename F, Int A, Int B, typename... Fs>
constexpr auto operator*(F f, Multiplication<Constant<A, B>, Fs...> y) {
	if constexpr (fields::detail::is_constant<F>::value) {
		return (f * y.template get<0>()) * y.template sub_product<1, sizeof...(Fs) + 1>();
	} else {
		return y.template get<0>() * (f * y.template sub_product<1, sizeof...(Fs) + 1>());
	}
}

template <Int A, Int B, typename... Fs>
constexpr auto operator*(Constant<A, B> f, Multiplication<Constant<A, B>, Fs...> y) {
	return (f * f) * y.template sub_product<1, sizeof...(Fs) + 1>();
}

template <typename... Fs, Int A, Int B, typename... Gs>
constexpr auto operator*(Multiplication<Fs...> lhs, Multiplication<Constant<A, B>, Gs...> rhs) { return rhs.template get<0>() * (lhs * rhs.template sub_product<1, sizeof...(Gs) + 1>()); }

// Multiplications with unrelated factors
// Need to remove the case where G is constant as all constants are moved to the left-hand-side
template <typename G, typename... Fs>
constexpr auto operator*(Multiplication<Fs...> f, G g) {
	/*if constexpr (fields::detail::is_constant<G>::value) {
		return g * f;
	}
	else {
		constexpr auto match_found = (std::is_same_v<Fs, G> || ...);

		if constexpr (false == match_found) {
			constexpr auto power_found = (fields::detail::is_power<Fs, G>::value || ...);

			if constexpr (false == power_found) {
				return Multiplication<Fs..., G>(f, g);
			}
			else {
				return f.template get<0>() * (f.template sub_product<1, sizeof...(Fs)>() * g);
			}
		}
		else {
			return f.template get<0>() * (f.template sub_product<1, sizeof...(Fs)>() * g);
		}
	}*/
	return g * f;
}

template <typename... Fs, typename G, typename = std::enable_if_t<fields::detail::is_power<G, Multiplication<Fs...>>::value == false>>
constexpr auto operator*([[maybe_unused]] G g, [[maybe_unused]] Multiplication<Fs...> f) {
	if constexpr (std::is_same_v<G, Constant<0>>) {
		return Constant<0>{};
	}
	else if constexpr (std::is_same_v<G, Constant<1>>) {
		return f;
	}
	else if constexpr (fields::detail::is_constant<G>::value) {
		return Multiplication<G, Fs...>(g, f);
	}

	else {
		constexpr auto location_in_pack = util::contains<G, Multiplication<Fs...>>::index;

		if constexpr (-1 == location_in_pack) {
			constexpr auto power_location_in_pack = fields::detail::contains_power<G, Multiplication<Fs...>>::index;

			if constexpr (-1 == power_location_in_pack) {
				return Multiplication<Fs..., G>(f, g);
			}
			else {
				static_assert(power_location_in_pack >= 0);
				if constexpr (power_location_in_pack == 0) {
					return (g * f.template get<0>()) * f.template sub_product<1, sizeof...(Fs)>();
				}
				else if constexpr (power_location_in_pack == sizeof...(Fs) - 1) {
					return f.template sub_product<0, power_location_in_pack>() * (g * f.template get<power_location_in_pack>());
				}
				else {
					static_assert(power_location_in_pack > 0);
					return ((f.template sub_product<0, power_location_in_pack>() * (g * f.template get<power_location_in_pack>())) * f.template sub_product<power_location_in_pack + 1, sizeof...(Fs)>());
				}
			}
		}
		else {
			static_assert(location_in_pack >= 0);
			if constexpr (location_in_pack == 0) {
				return (g * f.template get<0>()) * f.template sub_product<1, sizeof...(Fs)>();
			}
			else if constexpr (location_in_pack == sizeof...(Fs) - 1) {
				return f.template sub_product<0, location_in_pack>() * (g * f.template get<location_in_pack>());
			}
			else {
				static_assert(location_in_pack > 0);
				return ((f.template sub_product<0, location_in_pack>() * (g * f.template get<location_in_pack>())) * f.template sub_product<location_in_pack + 1, sizeof...(Fs)>());
			}
		}




		/*constexpr auto match_found = (util::is_same<Fs, G>::value || ...);

		if constexpr (false == match_found) {
			constexpr auto power_found = (fields::detail::is_power<Fs, G>::value || ...);

			if constexpr (false == power_found) {
				return Multiplication<G, Fs...>(g, f);
			}
			else {
				return f.template get<0>() * (g * f.template sub_product<1, sizeof...(Fs)>());
			}
		}
		else {
			return f.template get<0>() * (g * f.template sub_product<1, sizeof...(Fs)>());
		}*/
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
	}
	else {
		return (lhs * rhs.template get<0>()) * rhs.template sub_product<1, sizeof...(Gs)>();
	}
}

template <typename... Fs, Int A, Int B>
constexpr auto operator*(Multiplication<Constant<A, B>, Fs...> lhs, Multiplication<Constant<A, B>, Fs...>) {
	if constexpr (sizeof...(Fs) == 1) {
		return (Constant<A, B>{}*Constant<A, B>{})*(lhs.template get<1>()*lhs.template get<1>());
	} else {
		return (Constant<A, B>{}*Constant<A, B>{})*(lhs.template sub_product<1, sizeof...(Fs) + 1>()*lhs.template sub_product<1, sizeof...(Fs) + 1>());
	}
}

template <typename F>
constexpr auto operator*(F lhs, F) {
	if constexpr (std::is_same_v<std::decay_t<decltype(lhs)>, Constant<0>>) {
		return Constant<0>{};
	}
	else {
		return Power<F, 2>{lhs};
	}
}


template <typename F, typename G>
constexpr auto operator*(Multiplication<F, G> lhs, G rhs) {
	if constexpr (std::is_same_v<std::decay_t<decltype(rhs)>, Constant<0>>) {
		return Constant<0>{};
	}
	else {
		return lhs.template get<0>() * Power<G, 2>{rhs};
	}
}

template <typename F, typename... Gs>
constexpr auto operator*(F lhs, Multiplication<F, Gs...> rhs) {
	return Power<F, 2>{lhs}*rhs.template sub_product<1, sizeof...(Gs) + 1>();
}

template <typename F, typename G, Int N>
constexpr auto operator*(Multiplication<F, G> lhs, Power<G, N> rhs) {
	if constexpr (-1 == N) {
		return lhs.template get<0>();
	}
	else {
		return lhs.template get<0>() * Power<G, N + 1>{rhs};
	}
}

template <typename F, typename... Gs, Int N>
constexpr auto operator*(Power<F, N> lhs, Multiplication<F, Gs...> rhs) {
	if constexpr (-1 == N) {
		return rhs.template sub_product<1, sizeof...(Gs) + 1>();
	}
	else {
		return Power<F, N + 1>{lhs}*rhs.template sub_product<1, sizeof...(Gs) + 1>();
	}
}

template <typename F, typename G, Int N>
constexpr auto operator*(Multiplication<F, Power<G, N>> lhs, G rhs) {
	if constexpr (-1 == N) {
		return lhs.template get<0>();
	}
	else {
		return lhs.template get<0>() * Power<G, N + 1>{rhs};
	}
}

template <typename F, typename... Gs, Int N>
constexpr auto operator*(F lhs, Multiplication<Power<F, N>, Gs...> rhs) {
	return Power<F, N + 1>{lhs}*rhs.template sub_product<1, sizeof...(Gs) + 1>();
}

template <typename F, typename G, Int N, Int M>
constexpr auto operator*(Multiplication<F, Power<G, N>> lhs, Power<G, M> rhs) {
	if constexpr (N + M == 1) {
		return lhs.template get<0>() * rhs.f();
	}
	else {
		return lhs.template get<0>() * Power<G, N + M>{rhs};
	}
}

template <typename F, typename... Gs, Int N, Int M>
constexpr auto operator*(Power<F, N> lhs, Multiplication<Power<F, M>, Gs...> rhs) {
	if constexpr (N + M == 1) {
		return lhs.f()* rhs.template sub_product<1, sizeof...(Gs) + 1>();
	} else {
		return Power<F, N + M>{lhs}*rhs.template sub_product<1, sizeof...(Gs) + 1>();
	}
}

template <typename F, typename G, Int N>
constexpr auto operator*(Multiplication<F, Power<G, N>> lhs, Power<G, N> rhs) {
	return lhs.template get<0>() * Power<G, 2 * N>{rhs};
}

template <typename F, typename... Gs, Int N>
constexpr auto operator*(Power<F, N> lhs, Multiplication<Power<F, N>, Gs...> rhs) {
	return Power<F, 2 * N>{lhs}*rhs.template sub_product<1, sizeof...(Gs) + 1>();
}

template <Int A, Int B, Int C, Int D, Int N, typename... Fs>
constexpr auto operator*(Constant<A, B> lhs, Power<Multiplication<Constant<C, D>, Fs...>, N> rhs) {
	constexpr auto n = Constant<N>{};
	return (lhs * (rhs.f().template get<0>() ^ n)) * (rhs.f().template sub_product<1, sizeof...(Fs) + 1>() ^ n);
}

template <typename F, typename... Fs>
constexpr auto operator*(F f, Vector<Fs...> v) {
	return v.multiply_scalar(f);
}

template <typename F, typename... Fs>
constexpr auto operator*(Vector<Fs...> v, F f) {
	return v.multiply_scalar(f);
}













































// -------------------------------------------------------------------------------------------------
//                                      / operations
// -------------------------------------------------------------------------------------------------

// Multiplication with common factors
template <typename F, typename G>
constexpr auto operator/(Multiplication<G, F> m, F) { return m.template get<0>(); }

template <typename F, typename G>
constexpr auto operator/(F, Multiplication<G, F> m) { return Constant<1, 1>{} / m.template get<0>(); }

template <typename F, typename... Gs>
constexpr auto operator/(Multiplication<F, Gs...> m, F) { return m.template sub_product<1, sizeof...(Gs) + 1>(); }

template <typename F, typename... Gs>
constexpr auto operator/(F, Multiplication<F, Gs...> m) { return Constant<1, 1>{} / m.template sub_product<1, sizeof...(Gs) + 1>(); }

template <typename F, typename... Gs>
constexpr auto operator/(Multiplication<F, Gs...> lhs, Multiplication<Gs...>) { return lhs.template get<0>(); }

template <typename F, typename... Gs>
constexpr auto operator/(Multiplication<Gs...>, Multiplication<F, Gs...> rhs) { return Constant<1>{} / rhs.template get<0>(); }

template <typename F, typename G, typename... Gs>
constexpr auto operator/(Multiplication<G, F> lhs, Multiplication<F, Gs...> rhs) { return lhs.template get<0>() / rhs.template sub_product<1, sizeof...(Gs) + 1>(); }

template <typename F, typename G, typename... Gs>
constexpr auto operator/(Multiplication<F, Gs...> lhs, Multiplication<G, F> rhs) { return lhs.template sub_product<1, sizeof...(Gs) + 1>() / rhs.template get<0>(); }

template <typename F, typename G, typename H>
constexpr auto operator/(Multiplication<G, F> lhs, Multiplication<H, F> rhs) { return lhs.template get<0>() / rhs.template get<0>(); }

// division by constant is equivalent to multiplying by inverse
template <typename F, Int A, Int B>
constexpr auto operator/(F f, Constant<A, B> c) {
	return (Constant<1>{} / c)* f;
}


}	// fields