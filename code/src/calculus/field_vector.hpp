#pragma once

#include "../util/integer.hpp"

#include "vector.hpp"

#include <tuple>
#include <utility>

namespace fields {

// TODO: make a nested struct
template <typename Hamiltonian, std::size_t Idx, std::size_t Dim, Int SpatialDerivative>
struct FieldVectorComponent;

template <typename Hamiltonian, std::size_t Dim>
class FieldVector {
public:	
	constexpr FieldVector(Hamiltonian H_) : H(H_) {
	}

	template <typename... Ys, typename X_, typename T_>
	constexpr auto operator()(Vector<Ys...> y, X_ x, T_ t) const { static_assert(sizeof...(Ys) == Dim); return vector_of_fields()(y, x, t); }

	constexpr auto vector_of_fields() const {
		return vector_of_fields_impl(std::make_index_sequence<Dim>{});
	}

	constexpr auto vector_of_time_derivatives() const {
		return std::apply(H, vector_of_fields().components());
	}

	Hamiltonian H;

 private:
	template <std::size_t... Idx>
	constexpr auto vector_of_fields_impl(std::index_sequence<Idx...>) const {
		static_assert(sizeof...(Idx) == Dim);
		
		return Vector{ FieldVectorComponent<Hamiltonian, Idx, Dim, 0>{*this}... };
	}

};

template <typename Hamiltonian, std::size_t Idx, std::size_t Dim, Int SpatialDerivative>
struct FieldVectorComponent {
	template <typename... Ys, typename X_, typename T_>
	constexpr auto operator()(Vector<Ys...> y, X_, T_) const { static_assert(sizeof...(Ys) == Dim); return y.template get<Idx>(); }

	FieldVector<Hamiltonian, Dim> equations;
};

template <std::size_t Dim, typename Hamiltonian>
constexpr auto make_field_vector(Hamiltonian H) {
	return FieldVector<Hamiltonian, Dim>{H};
}

template <typename Stream, typename Hamiltonian, std::size_t Idx, std::size_t Dim, Int SpatialDerivative>
auto& operator<<(Stream& os, FieldVectorComponent<Hamiltonian, Idx, Dim, SpatialDerivative> y) {
	if constexpr (SpatialDerivative != 0) {
		os << "d_dx<" << SpatialDerivative << ">(y[" << Idx << "])";
	}
	else {
		os << "y[" << Idx << "]";
	}

	return os;
}

}	// fields