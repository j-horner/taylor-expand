#pragma once

namespace fields {

template <typename F, typename G>
class Subtraction {
public:
    template <typename... Args>
    constexpr auto operator()(Args... args) const { return lhs(args...) - rhs(args...); }

    F lhs;
    G rhs;
};

template <typename Stream, typename F, typename G>
auto& operator<<(Stream& os, Subtraction<F, G> y) {
    os << "(" << y.lhs << " - " << y.rhs << ")";
    return os;
}

} // fields
