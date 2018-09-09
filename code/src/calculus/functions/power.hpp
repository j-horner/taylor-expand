#pragma once

#include "constant.hpp"

#include "../../util/util.hpp"

// C headers
#include <cstdint>

namespace fields {

using Int = std::intmax_t;

template <Int A, Int B>
class Constant;

template <typename F, Int N>
class Power {
 public:
    constexpr Power() = default;

    explicit constexpr Power(F f) : f_(f) {
    }

    template <typename... Args>
    constexpr auto operator()(Args&&... args) const {
        return util::pow(f_(std::forward<Args>(args)...), N);
    }

 private:
    F f_;
};

namespace operators {

template <typename F>
constexpr auto operator^(F, Constant<0, 1>) {
    using namespace literals;
    return 1_c;
}

template <typename F>
constexpr auto operator^(F f, Constant<1, 1>) {
    return f;
}

template <typename F, Int N>
constexpr auto operator^(F f, Constant<N, 1>) {
    return Power<F, N>{f};
}

// power operator with integers, if rational powers are needed do it later
template <Int A, Int N>
constexpr auto operator^(Constant<A, 1>, Constant<N, 1>) {
    if constexpr (0 == N) {
        // anything to power 0 is 1
        return Constant<1, 1>{};
    } else if constexpr (0 > N) {
        // negative powers mean divide
        return Constant<1, 1>{} / (Constant<A, 1>{}^Constant<-N, 1>{});
    } else {
        // standard recursive power definition, could probably be better
        constexpr auto A_N = util::pow(A, N);

        return Constant<A_N, 1>{};
    }
}

template <Int A, Int B, Int N>
constexpr auto operator^(Constant<A, B>, Constant<N, 1>) { return (Constant<A, 1>{}^Constant<N, 1>{}) / (Constant<B, 1>{}^Constant<N, 1>{}); }

}   // operators
}   // fields