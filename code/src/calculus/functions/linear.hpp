#pragma once

namespace fields {

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

template <typename Stream>
auto& operator<<(Stream& os, X) {
    os << "x";
    return os;
}

template <typename Stream>
auto& operator<<(Stream& os, T) {
    os << "t";
    return os;
}

constexpr static auto x = X{};
constexpr static auto t = T{};

}	// fields