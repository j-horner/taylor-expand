#pragma once

#include "constant.hpp"

#include "../derivative.hpp"

namespace fields {

namespace detail {
class X {
public:
    template <typename Phi, typename X_, typename T_>
    constexpr auto operator()(Phi, X_ x, T_) const { return x; }
};

class T {
 public:
    template <typename Phi, typename X_, typename T_>
    constexpr auto operator()(Phi, X_, T_ t) const { return t; }
};

}   // detail

constexpr static auto x = detail::X{};
constexpr static auto t = detail::T{};

}