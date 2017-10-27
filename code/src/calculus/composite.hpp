#pragma once

#include "shared_field.hpp"

namespace fields {
namespace operators {

template <typename F, typename G>
class Composite {
 public:
    template <typename Lhs, typename Rhs>
    Composite(Lhs&& l, Rhs&& r) : f_(std::forward<Lhs>(l)), g_(std::forward<Rhs>(r)) {
    }

    auto lhs() const -> decltype(auto) { return f_.func(); }
    auto rhs() const -> decltype(auto) { return g_.func(); }

    auto f() const { return f_; }
    auto g() const { return g_; }

 protected:
    SharedField<F> f_;
    SharedField<G> g_;
};


}	// operators
}	// fields