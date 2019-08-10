#pragma once

#include "functions/constant.hpp"

#include "operators/division.hpp"

// C++ headers
#include <type_traits>

namespace fields {

template <typename F, typename G>
class Division {
 public:
    template <typename... Args>
    constexpr auto operator()(Args... args) const { return lhs(args...)/rhs(args...); }

    F lhs;
    G rhs;

    static_assert(::fields::detail::is_constant<std::decay_t<G>>::value == false, "Division by constants should never occur as they can always be cast as a multiplication.");
};

template <typename Stream, typename F, typename G>
auto& operator<<(Stream& os, Division<F, G> y) {
    os << "(" << y.lhs << "/" << y.rhs << ")";
    return os;
}

} // fields

#include "operators/division_impl.hpp"