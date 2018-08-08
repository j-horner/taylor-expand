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

constexpr auto d_dx(fields::detail::X) {
    using namespace literals;
    return 1_c;
}

constexpr auto d_dx(fields::detail::T) {
    using namespace literals;
    return 0_c;
}

constexpr auto d_dt(fields::detail::X) {
    using namespace literals;
    return 0_c;
}

constexpr auto d_dt(fields::detail::T) {
    using namespace literals;
    return 1_c;
}

}   // operators


}