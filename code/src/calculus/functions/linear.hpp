#pragma once

#include "constant.hpp"

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

namespace operators {

template <Int D = 1>
constexpr auto d_dx(fields::detail::X) {
    static_assert(D >= 0);

    using namespace literals;
    if constexpr (D == 0) {
        return x;
    } else if constexpr (D == 1) {
        return 1_c;
    } else {
        return 0_c;
    }
}

template <Int D = 1>
constexpr auto d_dx(fields::detail::T) {
    using namespace literals;
    if constexpr (D == 0) {
        return t;
    } else {
        return 0_c;
    }
}

template <Int D = 1>
constexpr auto d_dt(fields::detail::X) {
    using namespace literals;
    if constexpr (D == 0) {
        return x;
    } else {
        return 0_c;
    }
}

template <Int D = 1>
constexpr auto d_dt(fields::detail::T) {
    static_assert(D >= 0);

    using namespace literals;
    if constexpr (D == 0) {
        return t;
    } else if constexpr (D == 1) {
        return 1_c;
    } else {
        return 0_c;
    }
}

}   // operators


}