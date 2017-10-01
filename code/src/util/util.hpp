#pragma once

namespace fields {
namespace util {

constexpr auto factorial(int n) -> int {
	return (n == 0) ? 1 : n*factorial(n - 1);
}

constexpr auto binomial(int n, int k) -> int {
	return factorial(n) / (factorial(k)*factorial(n - k));
}

template <typename T>
constexpr auto pow(T x, int n) {
	auto y = static_cast<T>(1.0);
	for (auto i = 0; i < n; ++i) {
		y *= x;
	}
	return y;
}

}	// util
}	// fields