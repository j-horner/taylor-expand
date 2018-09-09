#pragma once

#include "constant.hpp"

#include "../derivative.hpp"

namespace fields {

namespace detail {
class X {
public:
    template <typename R>
    constexpr auto operator()(R x) const { return x; }
};

class T {
 public:
    template <typename R>
    constexpr auto operator()(R t) const { return t; }
};

}   // detail

constexpr static auto x = detail::X{};
constexpr static auto t = detail::T{};

}