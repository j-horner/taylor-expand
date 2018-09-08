#pragma once

#include "constant.hpp"

#include "../../util/util.hpp"

// C headers
#include <cstdint>

namespace fields {

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
constexpr auto operator^(F, Constant<0>) {
    using namespace literals;
    return 1_c;
}

template <typename F>
constexpr auto operator^(F f, Constant<1>) {
    return f;
}

template <typename F, Int N>
constexpr auto operator^(F f, Constant<N>) {
    return Power<F, N>{f};
}

}   // operators
}   // fields