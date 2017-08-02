#pragma once

#include <memory>
#include <utility>

namespace fields {
namespace operators {

template <typename F>
class SharedField {
public:
	using Pointer = std::shared_ptr<std::decay_t<F>>;
	
	SharedField(F&& f) : f_(std::make_shared<std::decay_t<F>>(std::forward<F>(f))) {
	}

	SharedField(Pointer p) : f_(std::move(p)) {
	}

	auto func() const -> decltype(auto) { return *f_; }

	auto get() const { return f_; }

protected:
	Pointer f_;
};


}	// operators
}	// fields