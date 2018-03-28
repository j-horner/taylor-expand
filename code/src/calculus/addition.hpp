#pragma once

#include "composite.hpp"
#include "multiply.hpp"
#include "functions\constant.hpp"

namespace fields {
namespace operators {

/*template <typename F, typename G>
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

*/

template <typename F, typename G>
class Addition {
 public:
    template <typename T>
    constexpr auto operator()(T x) const { return lhs(x) + rhs(x); }
 
     F lhs;
     G rhs;
};

template <typename F, typename G>
class Subtraction {
public:
    template <typename T>
    constexpr auto operator()(T x) const { return lhs(x) - rhs(x); }

    F lhs;
    G rhs;
};

template <typename F>
constexpr auto operator+(F, F) { return 2_c*F{}; }

template <typename F, Int A, Int B>
constexpr auto operator+(Multiplication<Constant<A, B>, F> y, F f) { return (y.lhs + 1_c)*f; }

template <typename F, Int A, Int B>
constexpr auto operator+(F f, Multiplication<Constant<A, B>, F> y) { return y + f; }

template <typename F, typename G>
constexpr auto operator+(F lhs, G rhs) { return Addition<F, G>{lhs, rhs}; }

template <typename F, typename G>
constexpr auto operator-(F lhs, G rhs) { return Subtraction<F, G>{lhs, rhs}; }

template <typename F, typename G>
constexpr auto d_dx(Addition<F, G> y) { return d_dx(y.lhs) + d_dx(y.rhs); }

template <typename F, typename G>
constexpr auto d_dx(Subtraction<F, G> y) { return d_dx(y.lhs) - d_dx(y.rhs); }

}	// operators
}	// fields