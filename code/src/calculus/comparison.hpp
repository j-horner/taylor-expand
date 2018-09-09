#pragma once

#include "functions/constant.hpp"

// C++ headers
#include <type_traits>

namespace fields {
namespace operators {

template <typename F, typename G>
constexpr auto operator==(F lhs, G rhs) {
    constexpr auto default_comparison = std::is_same_v<std::decay_t<decltype(lhs)>, std::decay_t<decltype(rhs)>>;

    if constexpr (detail::is_constant<F>::value && std::is_arithmetic_v<G>) {
        return static_cast<G>(lhs) == rhs;
    } else if constexpr (detail::is_constant<G>::value && std::is_arithmetic_v<F>) {
        return static_cast<F>(rhs) == lhs;
    } else {
        return default_comparison;
    }
}

} // operators
} // fields
