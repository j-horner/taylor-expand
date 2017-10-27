#pragma once

#include "shared_field.hpp"

#include <functional>
#include <list>
#include <unordered_map>
#include <utility>

namespace fields {

template <typename T>
class Field {
 public:
    template <typename F>
    explicit Field(F&& f) : f_(std::forward<F>(f)) {
    }

    template <typename X>
    auto operator()(X&& x) const {
        const auto idx = static_cast<int>(x*dx_1);

        const auto location = memory_.find(idx);

        if (memory_.end() != location) {
            return location->second;
        } else {
            const auto val = f_(std::forward<X>(x));

            memory_.emplace(idx, val);

            return val;
        }
        }

 private:
    std::function<T> f_;

    mutable std::unordered_map<int, double> memory_;

    constexpr static auto dx_1 = 1.0e7;     // inverse of the dynamic lattice resolution
};

template <typename T>
class FieldList {
 public:
    template <typename... Args>
    auto operator()(Args&&... args) const {
        return front().func()(std::forward<Args>(args)...);
    }

    auto front() const -> decltype(auto) {
        return field_.front();
    }

    template <typename... Args>
    auto emplace_front(Args&&... args) {
        field_.emplace_front(SharedField<Field<T>>(Field<T>{std::forward<Args>(args)...}));
    }

 private:
    template <typename G>
    using SharedField = operators::SharedField<G>;

    std::list<SharedField<Field<T>>> field_;
};

}   // fields
