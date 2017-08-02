#pragma once

#include "shared_field.hpp"

namespace fields {
namespace operators {

template <typename F, typename G>
class Composite {
public:
	Composite(F&& f, G&& g) : f_(std::forward<F>(f)), g_(std::forward<G>(g)) {
	}

	Composite(SharedField<F>&& f, G&& g) : f_(std::move(f)), g_(std::forward<G>(g)) {
	}

	Composite(F&& f, SharedField<G>&& g) : f_(std::forward<F>(f)), g_(std::move(g)) {
	}

	Composite(SharedField<F>&& f, SharedField<G>&& g) : f_(std::move(f)), g_(std::move(g)) {
	}

	auto lhs() const -> decltype(auto) { return f_.func(); }
	auto rhs() const -> decltype(auto) { return g_.func(); }

	auto f() const { return f_.get(); }
	auto g() const { return g_.get(); }

protected:
	SharedField<F> f_;
	SharedField<G> g_;
};


}	// operators
}	// fields