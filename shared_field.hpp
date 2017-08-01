#pragma once

#include <memory>
#include <utility>

namespace fields {
namespace operators {

template <typename F>
class SharedField {
public:
	template <typename G>
	SharedField(G&& g) : f_(std::make_shared<std::decay_t<F>>(std::forward<G>(g))) {
	}

	SharedField(std::shared_ptr<F> f) : f_(std::move(f)) {
	}

	auto func() const -> decltype(auto) { return *f_; }

	auto get() const { return f_; }

protected:
	std::shared_ptr<F> f_;
};


}	// operators
}	// fields