#pragma once

#include "constant.hpp"

namespace fields {

class X {
public:
    template <typename T>
    constexpr auto operator()(T x) const { return x; }
};

constexpr static auto x = X{};

namespace operators {

constexpr auto d_dx(X) {
    using namespace literals;
    return 1_c;
}

static_assert(d_dx(x) == 1, "The derivative of 'x' is not 1.");

}   // operators


}