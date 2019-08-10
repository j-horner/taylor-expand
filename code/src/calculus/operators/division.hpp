#pragma once

// -------------------------------------------------------------------------------------------------
//                                      / operations
// -------------------------------------------------------------------------------------------------

namespace fields {

template <typename... Fs>
class Addition;

template <typename F, typename G>
class Subtraction;

template <typename... Fs>
class Multiplication;

template <typename F, typename G>
class Division;

template <typename F, typename G>
constexpr auto operator/(F lhs, G rhs);

}	// fields
