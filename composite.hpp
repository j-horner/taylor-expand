#pragma once

#include "shared_field.hpp"

namespace fields {
namespace operators {

template <typename F, typename G>
class Composite {
public:
	template <typename Lhs, typename Rhs>
	Composite(Lhs&& f, Rhs&& g) : f_(std::forward<Lhs>(f)), g_(std::forward<Rhs>(g)) {
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