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

template <typename F, typename G>
auto operator*(SharedField<F> f, SharedField<G> g) {
	return Multiply<F, G>(std::move(f), std::move(g));
}
template <typename F, typename G>
auto operator*(F&& f, G&& g) {
	return SharedField<F>(std::forward<F>(f)) * SharedField<G>(std::forward<G>(g));
}

/*template <typename F>
auto operator*(MultiplyScalar<F>&& f, double k) {
	return MultiplyScalar<F>(std::move(f), k);
}

template <typename F>
auto operator*(double k, MultiplyScalar<F>&& f) {
	return std::move(f)*k;
}*/

template <typename F>
auto operator*(MultiplyScalar<F> f, double k) {
	return MultiplyScalar<F>(std::move(f), k);
}

template <typename F>
auto operator*(double k, MultiplyScalar<F> f) {
	return std::move(f)*k;
}

template <typename F>
auto operator*(SharedField<F> f, double k) {
	return MultiplyScalar<F>(std::move(f), k);
}

template <typename F>
auto operator*(double k, SharedField<F> f) {
	return std::move(f)*k;
}

template <typename F>
auto operator*(F&& f, double k) {
	return MultiplyScalar<F>(std::forward<F>(f), k);
}

template <typename F>
auto operator*(double k, F&& f) {
	return std::forward<F>(f) * k;
}

template <typename F, typename G>
auto operator+(SharedField<F> f, G&& g) {
	return Addition<F, G>(std::move(f), std::forward<G>(g));
}

template <typename F, typename G>
auto operator+(SharedField<F> f, MultiplyScalar<G> g) {
	return Addition<F, MultiplyScalar<G>>(std::move(f), std::move(g));
}

template <typename F>
auto operator+(SharedField<F> f, MultiplyScalar<F> g) {
	
	assert(f.get() == &g.lhs());		// assume that objects of the same type are the same. not valid for cases like:		std::sin + 5*std::cos

	// std::cout << "Merging addition and multiplication:\t" << 1.0 << "\t" << g.rhs() << "--->\t" << (1.0 + g.rhs()) << std::endl;

	// can "get away" with just adding the scalars here
	return std::move(f)*(1.0 + g.rhs());
}

template <typename F, typename G>
auto operator+(F&& f, G&& g) {
	return SharedField<F>(std::forward<F>(f)) + SharedField<G>(std::forward<G>(g));
}

template <typename F, typename G>
auto operator+(SharedField<F> f, SharedField<G> g) {
	return Addition<F, G>(std::move(f), std::move(g));
}

/*template <typename F, typename G>
auto operator+(F&& f, MultiplyScalar<G> g) {
	return Addition<std::decay_t<F>, MultiplyScalar<G>>(std::forward<F>(f), std::move(g));
}

template <typename F, typename G>
auto operator+(MultiplyScalar<F> f, G&& g) {
	return Addition<MultiplyScalar<F>, std::decay_t<G>>(std::move(f), std::forward<G>(g));
}*/

}	// operators
}	// fields