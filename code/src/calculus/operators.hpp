#pragma once

#include "addition.hpp"
#include "derivative.hpp"
#include "multiply.hpp"
#include "multiply_scalar.hpp"
#include "shared_field.hpp"

#include <cassert>
#include <memory>

namespace fields {
namespace operators {

// ------------------------------------------------------
//  Multiply two functions
// ------------------------------------------------------
template <typename F, typename G>
auto operator*(SharedField<F> f, SharedField<G> g) { return Multiply<F, G>(std::move(f), std::move(g)); }

template <typename F, typename G>
auto operator*(F&& f, G&& g) { return Multiply<F, G>(std::forward<F>(f), std::forward<G>(g)); }

// ------------------------------------------------------
//  Multiply function by scalar
// ------------------------------------------------------
template <typename F>
auto operator*(SharedField<F> f, double k) { return MultiplyScalar<F>(std::move(f), k); }

template <typename F>
auto operator*(double k, SharedField<F> f) { return std::move(f)*k; }

template <typename F>
auto operator*(MultiplyScalar<F> f, double k) { return MultiplyScalar<F>(std::move(f), k); }

template <typename F>
auto operator*(double k, MultiplyScalar<F> f) { return std::move(f)*k; }

template <typename F>
auto operator*(F&& f, double k) { return MultiplyScalar<F>(std::forward<F>(f), k); }

template <typename F>
auto operator*(double k, F&& f) { return std::forward<F>(f) * k; }


// ------------------------------------------------------
//  Add two functions
// ------------------------------------------------------
template <typename F, typename G>
auto operator+(SharedField<F> f, G&& g) { return Addition<F, G>(std::move(f), std::forward<G>(g)); }

template <typename F, typename G>
auto operator+(SharedField<F> f, MultiplyScalar<G> g) { return Addition<F, MultiplyScalar<G>>(std::move(f), std::move(g)); }

template <typename F, typename G>
auto operator+(F&& f, G&& g) { return SharedField<F>(std::forward<F>(f)) + SharedField<G>(std::forward<G>(g)); }

template <typename F, typename G>
auto operator+(SharedField<F> f, SharedField<G> g) { return Addition<F, G>(std::move(f), std::move(g)); }

}	// operators
}	// fields