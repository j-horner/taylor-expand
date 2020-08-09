#pragma once

#include "integer.hpp"

// C++ headers
#include <tuple>
#include <type_traits>

namespace fields {
namespace util {

template <typename Int>
constexpr auto factorial(Int n) noexcept -> Int {
    return (n == Int{0}) ? Int{1} : n*factorial(n - 1);
}

template <typename Int>
constexpr auto binomial(Int n, Int k) noexcept {
    return factorial(n) / (factorial(k)*factorial(n - k));
}

template <typename T, typename Int>
constexpr auto pow(T x, Int n) noexcept {
    static_assert(std::is_integral_v<Int>, "n must be an integral type.");
    static_assert(std::is_arithmetic_v<T>, "This function is intended to be used with arithmetic types.");

    const auto N = (n >= 0) ? n : -n;

    auto y = T{1};
    for (auto i = 0; i < N; ++i) {
        y *= x;
    }

    return (n >= 0) ? y : T{1}/y;
}


template <typename T1, typename T2>
struct is_same : std::is_same<T1, T2> {};


namespace detail {


template <typename... Ts>
struct count : std::integral_constant<int, 0> {};

template <typename T, std::size_t... Idx, typename... Ts>
struct count<T, std::index_sequence<Idx...>, std::tuple<Ts...>> : std::integral_constant<int, (static_cast<int>(util::is_same<T, std::decay_t<std::tuple_element_t<Idx, std::tuple<Ts...>>>>::value) + ...)> {};

}   // detail


template <typename... Ts>
struct count : std::integral_constant<int, 0> {};

template <typename T>
struct count<T, std::tuple<>> : std::integral_constant<int, 0> {};

template <typename T, typename... Ts>
struct count<T, std::tuple<Ts...>> : std::integral_constant<int, ((sizeof...(Ts) == 0) ? 0
    : detail::count<T, std::make_index_sequence<sizeof...(Ts)>, std::tuple<Ts...>>::value)> {};

template <typename T1, typename T2>
struct is_permutation : std::bool_constant<std::is_same_v<T1, T2>> {};


template <typename... Ts, typename... Us>
struct is_permutation<std::tuple<Ts...>, std::tuple<Us...>> : std::bool_constant<(sizeof...(Ts) > sizeof...(Us)) ? ((count<Ts, std::tuple<Ts...>>::value == count<Ts, std::tuple<Us...>>::value) && ...)
    : ((count<Us, std::tuple<Ts...>>::value == count<Us, std::tuple<Us...>>::value) && ...)> {};

template <typename Tuple, template<typename...> typename Predicate, Int idx = static_cast<Int>(std::tuple_size<Tuple>::value) - 1>
struct tuple_find {
    static_assert(std::tuple_size<Tuple>::value > 0);
    static_assert(idx >= 0);
    constexpr static auto value = Predicate<typename std::tuple_element<idx, Tuple>::type>::value ? idx : tuple_find<Tuple, Predicate, idx - 1>::value;
};

template <typename Tuple, template<typename...> typename Predicate>
struct tuple_find<Tuple, Predicate, -1> {
    constexpr static auto value = -1;
};

template <typename Tuple, typename T, template<typename...> typename Predicate = std::is_same, Int idx = static_cast<Int>(std::tuple_size<Tuple>::value) - 1>
struct tuple_index {
    static_assert(std::tuple_size<Tuple>::value > 0);
    static_assert(idx >= 0);
    constexpr static auto value = Predicate<typename std::tuple_element<idx, Tuple>::type, T>::value ? idx : tuple_index<Tuple, T, Predicate, idx - 1>::value;
};

template <typename Tuple, typename T, template<typename...> typename Predicate>
struct tuple_index<Tuple, T, Predicate, -1> {
    constexpr static auto value = -1;
};

namespace detail {

// https://stackoverflow.com/questions/41723704/how-to-filter-a-stdinteger-sequence
template <auto... idx>
struct value_sequence {};

template <auto... As, auto... Bs>
constexpr auto operator+(value_sequence<As...>, value_sequence<Bs...>) {
    return value_sequence<As..., Bs...>{};
}

template <Int idx, typename P>
constexpr auto filter_an_index(P predicate) {
    if constexpr (predicate(idx)) {
        return value_sequence<idx>{};
    } else {
        return value_sequence<>{};
    }
}

template <Int idx, template<typename...> typename Predicate>
constexpr auto filter_an_index() {
    if constexpr (Predicate<std::integral_constant<Int, idx>>::value) {
        return value_sequence<idx>{};
    } else {
        return value_sequence<>{};
    }
}

template <Int... idx>
constexpr auto to_integer_sequence(value_sequence<idx...>) {
    return std::integer_sequence<Int, idx...>{};
}

}   // detail

template <Int... idx, typename P>
constexpr auto filter_indicies(std::integer_sequence<Int, idx...>, P predicate) {
    using namespace fields::util::detail;
    static_assert(sizeof...(idx) > 0);
    return to_integer_sequence((filter_an_index<idx>(predicate) + ...));
}

template <template<typename...> typename Predicate, Int... idx>
constexpr auto filter_indicies(std::integer_sequence<Int, idx...>) {
    using namespace fields::util::detail;
    static_assert(sizeof...(idx) > 0);
    return to_integer_sequence((filter_an_index<idx, Predicate>() + ...));
}


namespace detail {

template <typename T, typename U>
struct found_term_indicies;

template <Int... idx, Int... location>
struct found_term_indicies<std::integer_sequence<Int, idx...>, std::integer_sequence<Int, location...>> {
    static_assert(sizeof...(idx) == sizeof...(location));

    using type = std::integer_sequence<Int, ((location >= 0) ? idx : -1)...>;
};

}   // detail

template <Int... terms, Int... locations>
constexpr auto have_terms_been_found(std::integer_sequence<Int, terms...>, std::integer_sequence<Int, locations...>) {
    static_assert(sizeof...(terms) == sizeof...(locations));

    return sizeof...(terms) > 0;
}

template <template<typename...> typename Predicate = std::is_same, template<typename...> typename TupleL, template<typename...> typename TupleR, typename... Ts, typename... Us>
constexpr auto map_terms(TupleL<Ts...> lhs, TupleR<Us...> rhs) {

    using lhs_indicies = std::make_integer_sequence<Int, sizeof...(Ts)>;
    // using rhs_indicies = std::make_integer_sequence<Int, sizeof...(Us)>;

    using result_of_each_search_in_rhs = std::integer_sequence<Int, tuple_index<TupleR<Us...>, Ts, Predicate>::value...>;

    constexpr auto is_found = [] (auto idx) { return idx >= 0; };

    constexpr auto found_terms = filter_indicies(typename fields::util::detail::found_term_indicies<lhs_indicies, result_of_each_search_in_rhs>::type{}, is_found);

    constexpr auto term_locations = filter_indicies(result_of_each_search_in_rhs{}, is_found);

    return std::tuple{found_terms, term_locations, std::bool_constant<have_terms_been_found(found_terms, term_locations)>{}};
}

// must be lambda and not function template because a function template becomes function pointer when passed as a predicate and looses its constexpr
template <Int Idx>
constexpr auto index_not = [] (Int idx) { return Idx != idx; };

template <typename... Fs>
struct get_first;

template <typename Int, Int I, Int... idx>
struct get_first<std::integer_sequence<Int, I, idx...>> {
    constexpr static auto index = I;
};

}   // util
}   // fields
