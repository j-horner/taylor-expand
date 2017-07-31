#pragma once

#include <memory>

namespace fields {
namespace operators {

template <typename F, typename G>
class Composite {
public:
	Composite(F&& f, G&& g) : f_(std::make_shared<F>(std::forward<F>(f))), g_(std::make_shared<G>(std::forward<G>(g))) {
	}

	Composite(F&& f, std::shared_ptr<G> g) : f_(std::make_shared<F>(std::forward<F>(f))), g_(g) {
	}

	Composite(std::shared_ptr<F> f, G&& g) : f_(f), g_(std::make_shared<G>(std::forward<G>(g))) {
	}

	Composite(std::shared_ptr<F> f, std::shared_ptr<G> g) : f_(f), g_(g) {
	}

	auto lhs() const -> decltype(auto) { return *f_; }
	auto rhs() const -> decltype(auto) { return *g_; }

	auto f() const { return f_; }
	auto g() const { return g_; }

protected:
	std::shared_ptr<F> f_;
	std::shared_ptr<G> g_;
};


}	// operators
}	// fields