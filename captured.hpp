#pragma once

#include <type_traits>

namespace fields {

// if T is an lvalue reference, captured<T> will be the lvalue reference

// otherwise T is an rvalue reference or a non-reference, in which case captured<T> will be the non-reference

template <typename T>
using captured = std::conditional_t<std::is_lvalue_reference<T>::value,
									std::reference_wrapper<std::remove_reference_t<T>>,
									T>;

}