#pragma once

#include "composite.hpp"

namespace fields {
namespace operators {

template <typename F, typename G>
class Addition : public Composite<F, G> {
 public:
    template <typename Lhs, typename Rhs>
    Addition(Lhs&& l, Rhs&& r) : Composite<F, G>(std::forward<Lhs>(l), std::forward<Rhs>(r)) {
    }

    template <typename T>
    auto operator() (T x) { return f_(x) + g_(x); }
};

template <typename F, typename G>
class Subtraction : public Composite<F, G> {
 public:
    template <typename Lhs, typename Rhs>
    Subtraction(Lhs&& l, Rhs&& r) : Composite<F, G>(std::forward<Lhs>(l), std::forward<Rhs>(r)) {
    }

    template <typename T>
    auto operator() (T x) { return f_(x) - g_(x); }
};

}	// operators
}	// fields