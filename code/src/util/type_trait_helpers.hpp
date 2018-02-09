#pragma once

#include <tuple>
#include <type_traits>

namespace fields {
namespace util {

// determines if a tuple has a given type, deduces the position in the tuple if so
template <typename T, typename Tuple>
struct has_type {
};

// empty tuple does not have the desired type, return false
template <typename T>
struct has_type<T, std::tuple<>> : std::false_type {
    constexpr static auto Pos = -1;
};

// if the type is found return true
template <typename T, typename... Ts>
struct has_type<T, std::tuple<T, Ts...>> : std::true_type {
    constexpr static auto Pos = 0;
};

// otherwise move on to the next type
template <typename T, typename U, typename... Ts>
struct has_type<T, std::tuple<U, Ts...>> : has_type<T, std::tuple<Ts...>> {
    using Base = has_type<T, std::tuple<Ts...>>;

    constexpr static auto Pos = Base::type::value ? Base::Pos + 1 : -1;
};

template <typename T, typename Tuple>
using tuple_contains_type = typename has_type<T, Tuple>::type;

template <typename T, typename Tuple>
using tuple_type_position = typename has_type<T, Tuple>::Pos;


template <typename T, typename Tuple>
struct partition_tuple {
};

template <typename T, typename... Ts>
struct partition_tuple<T, std::tuple<T, Ts...>> {
    using first = std::tuple<>;
    using second = std::tuple<Ts...>;
};

template <typename T, typename U, typename... Ts>
struct partition_tuple<T, std::tuple<U, Ts...>> : partition_tuple<T, std::tuple<Ts...>> {
    using Base = partition_tuple<T, std::tuple<Ts...>>;
    using first = decltype(std::tuple_cat(U{}, Base::first{}));
};

template <typename T, typename Tuple>
using split_tuple_first = typename partition_tuple<T, Tuple>::first;

template <typename T, typename Tuple>
using split_tuple_second = typename partition_tuple<T, Tuple>::second;

}   // util
}   // fields
