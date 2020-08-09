#pragma once


// C++ headers
#include <tuple>
#include <utility>

// C headers
#include <cstddef>

namespace fields {

template <typename... Fs>
class Addition {
public:
	constexpr Addition() = default;

	template <typename... Ts, typename G>
	constexpr Addition(G lhs, Addition<Ts...> rhs) : fs(std::tuple_cat(std::make_tuple(lhs), rhs.fs)) {
	}

	template <typename... Ts, typename G>
	constexpr Addition(Addition<Ts...> lhs, G rhs) : fs(std::tuple_cat(lhs.fs, std::make_tuple(rhs))) {
	}

	template <typename... Ts, typename... Us>
	constexpr Addition(Addition<Ts...> lhs, Addition<Us...> rhs) : fs(std::tuple_cat(lhs.fs, rhs.fs)) {
	}

	template <typename F, typename G>
	constexpr Addition(F lhs, G rhs) : fs(std::make_tuple(lhs, rhs)) {
	}

	template <typename... Args>
	constexpr auto operator()(Args... args) const { return sum_impl(components_{}, std::make_tuple(args...)); }

	template <std::size_t I>
	constexpr auto get() const { return std::get<I>(fs); }

	template <typename F>
	constexpr auto get() const { return std::get<F>(fs); }

	template <std::size_t I, std::size_t J>
	constexpr auto sub_sum() const {
		static_assert((I <= sizeof...(Fs)) && (J <= sizeof...(Fs)) && (I <= J), "Expect following conditions on I, J: (I <= N) && (J <= N) && (I <= J)");
		return subset_impl<I>(std::make_index_sequence<J - I>{});
	}

	template <typename D>
	constexpr auto derivative(D d) const {  return derivative_impl(d, components_{}); }

private:
	using components_ = std::make_index_sequence<sizeof...(Fs)>;
	
	template <typename... Gs> friend class Addition;

	explicit constexpr Addition(std::tuple<Fs...> ts) : fs(ts) {
	}

	template <std::size_t... Is, typename Tuple>
	constexpr auto sum_impl(std::index_sequence<Is...>, Tuple args) const { return (std::apply(get<Is>(), args) + ...); }

	template <typename D, std::size_t... Is>
	constexpr auto derivative_impl(D d, std::index_sequence<Is...>) const { return ((d(get<Is>())) + ...); }

	template <std::size_t I, std::size_t... Is>
	constexpr auto subset_impl(std::index_sequence<Is...>) const {
		if constexpr (sizeof...(Is) == 1) {
			return get<I>();
		}
		else {
			return Addition<std::decay_t<decltype(get<I + Is>())>...>(std::make_tuple(get<I + Is>()...));
		}
	}

	std::tuple<Fs...> fs;
};

namespace detail {

template <typename Stream, typename T, typename... Rest>
auto print(Stream& os, T first, Rest... rest) -> void;

template <typename Stream, typename T>
auto print(Stream& os, T last) -> void {
	os << last;
}

template <typename Stream, typename T, typename... Rest>
auto print(Stream& os, T first, Rest... rest) -> void {
	os << first << " + ";
	print(os, rest...);
}

}	// detail

template <typename Stream, typename... Fs>
auto& operator<<(Stream& os, Addition<Fs...> y) {
	os << "(";
	fields::detail::print(os, y.template get<Fs>()...);
	os << ")";

	return os;
}

}	// fields
