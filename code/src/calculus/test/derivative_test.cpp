#include "../derivative.hpp"

#include "../comparison.hpp"
#include "../functions/linear.hpp"

#include "../operators/math_operators.hpp"

#include <gtest/gtest.h>

namespace fields {

// TODO: clang fails the first test if placed in fields::test namespace. WHY?
// namespace test {

struct A {
    template <typename T> constexpr auto operator()(T) const { return 3; }
};
struct B {
    template <typename T> constexpr auto operator()(T x) const { return 2*x; }
};
struct C {
    template <typename T> constexpr auto operator()(T x) const { return 4*x; }
};
struct D {
    template <typename T> constexpr auto operator()(T x) const { return x*x; }
};

template <int N = 1> struct dA_dx {};
template <int N = 1> struct dB_dx {};
template <int N = 1> struct dC_dx {};
template <int N = 1> struct dD_dx {};

template <int N = 1> constexpr auto d_dx(A) { return dA_dx<N>{}; }
template <int N = 1> constexpr auto d_dx(B) { return dB_dx<N>{}; }
template <int N = 1> constexpr auto d_dx(C) { return dC_dx<N>{}; }
template <int N = 1> constexpr auto d_dx(D) { return dD_dx<N>{}; }

class DerivativeTest : public ::testing::Test {
protected:
    	
	constexpr static auto a = A{};
    constexpr static auto b = B{};
    constexpr static auto c = C{};
    constexpr static auto d = D{};

    template <typename T, typename U>
    struct identify_terms;

    template <typename... Fs, typename... Gs>
    struct identify_terms<Multiplication<Fs...>, Multiplication<Gs...>> {
        using type = std::integer_sequence<Int, util::contains<Fs, Multiplication<Gs...>>::index...>;
    };

    template <typename... Fs, typename... Gs, typename... Hs,
        typename found_multiple_locations = std::integer_sequence<Int, fields::detail::contains_sub_multiple<Hs, Multiplication<Addition<Fs...>, Gs...>>::index...>,
        typename enabled = std::enable_if_t<fields::detail::any_found_terms_in_rhs<found_multiple_locations>::value>>
    constexpr auto add_function(Addition<Hs...> lhs, Multiplication<Addition<Fs...>, Gs...> rhs) {

        static_assert(sizeof...(Fs) > 1);
        static_assert(sizeof...(Gs) > 0);
        static_assert(sizeof...(Hs) > 1);

        return rhs + lhs;
    }
};

TEST_F(DerivativeTest, Derivative_Of_Sums_Is_Correct) {
	constexpr auto da_dx = d_dx(a);
    constexpr auto db_dx = d_dx(b);
    constexpr auto dc_dx = d_dx(c);
    constexpr auto dd_dx = d_dx(d);

	static_assert(d_dx(a + b) == da_dx + db_dx);
    static_assert(d_dx(a + b + c) == da_dx + db_dx + dc_dx);
    static_assert(d_dx(a + b + c + d) == da_dx + db_dx + dc_dx + dd_dx);

    static_assert(d_dx(a - b) == da_dx - db_dx);
}

TEST_F(DerivativeTest, Derivative_Of_Products_Is_Correct) {
	using namespace literals;

	constexpr auto da_dx = d_dx(a);
	constexpr auto db_dx = d_dx(b);
	constexpr auto dc_dx = d_dx(c);
	constexpr auto dd_dx = d_dx(d);

    static_assert(d_dx(a*b) == da_dx*b + a*db_dx);
    static_assert(d_dx(a*b*c) == da_dx*b*c + db_dx*a*c + dc_dx*a*b);
    static_assert(d_dx(a*b*c*d) == da_dx*b*c*d + db_dx*a*c*d + dc_dx*a*b*d + dd_dx*a*b*c);

    static_assert(d_dx(a/b) == da_dx*(b^-1_c) + (-a)*(b^-2_c)*db_dx);
}

TEST_F(DerivativeTest, Derivative_Of_Linear_Is_Correct) {
	using namespace literals;

    {
        static_assert(d_dt(t) == 1);
        static_assert(d_dt<1>(t) == 1);
        static_assert(d_dt<2>(t) == 0);
        static_assert(d_dt<30>(t) == 0);

        static_assert(d_dx(x) == 1);
        static_assert(d_dx<1>(x) == 1);
        static_assert(d_dx<2>(x) == 0);
        static_assert(d_dx<30>(x) == 0);

        static_assert(d_dx(t) == 0);
        static_assert(d_dt(x) == 0);
        static_assert(d_dx<3>(t) == 0);
        static_assert(d_dt<7>(x) == 0);

        static_assert(d_dt<2>(x + t) == 0);
        static_assert(d_dx<4>(x - t) == 0);
    }

    {
        constexpr auto y = x;
        static_assert(d_dx(y) == 1);
    }
    {
        constexpr auto y = x + x + x + x;

        static_assert(y == 4_c*x);
        static_assert(d_dx(y) == 4);
    }
    {
        constexpr auto y = x - x;

        static_assert(y == 0);
        static_assert(d_dx(y) == 0);
    }
    {
        constexpr auto y = x + x - x + x + x + x - x - x - x + x - x - x;

        static_assert(y == 0);
        static_assert(d_dx(y) == 0);
    }
    {
        constexpr auto y = x*x;
        static_assert(d_dx(y) == 2_c*x);
    }
    {
        constexpr auto y = x/x;
        static_assert(d_dx(y) == 0);
    }
    {
        constexpr auto y = x*x*x;
        static_assert(d_dx(y) == 3_c*x*x);
    }
    {
        constexpr auto y = x*x*x*x*x*x;
        static_assert(d_dx(y) == 6_c*x*x*x*x*x);
    }
}

TEST_F(DerivativeTest, Derivative_Of_Vector_Is_Correct) {
	using namespace fields;
	using namespace literals;

	constexpr auto v = Vector{ x , x * x, t * x, t * t - 3_c * x };

	static_assert(d_dx(v) == Vector{ 1_c, 2_c * x, t, -3_c });
	static_assert(d_dt(v) == Vector{ 0_c, 0_c, x, 2_c * t });

	static_assert(d_dx<2>(v) == Vector{ 0_c, 2_c, 0_c, 0_c });
	static_assert(d_dx(d_dt(v)) == d_dt(d_dx(v)));
	static_assert(d_dx(d_dt(v)) == Vector{0_c, 0_c, 1_c, 0_c});
	static_assert(d_dt<2>(v) == Vector{ 0_c, 0_c, 0_c, 2_c });

}

template <typename... Fs, typename... Gs>
constexpr auto add_test(Multiplication<Fs...> lhs, Multiplication<Gs...> rhs) {

    static_assert(sizeof...(Fs) > 0);
    static_assert(sizeof...(Gs) > 0);

    if constexpr (util::is_same<Multiplication<Fs...>, Multiplication<Gs...>>::value) {
        return Constant<2>{}*lhs;
    } else {

        const auto same_mapping = fields::util::map_terms<util::is_same>(lhs, rhs);

        const auto terms_successfully_found = std::get<2>(same_mapping);

        if constexpr (terms_successfully_found) {
            const auto mapped_terms = std::get<0>(same_mapping);
            const auto mapped_locations = std::get<1>(same_mapping);

            return fields::detail::group_terms(mapped_terms, mapped_locations, lhs, rhs);
        } else {

            const auto power_mapping = fields::util::map_terms<fields::detail::is_power>(lhs, rhs);

            const auto powers_successfully_found = std::get<2>(power_mapping);

            if constexpr (powers_successfully_found) {
                const auto mapped_terms = std::get<0>(power_mapping);
                const auto mapped_locations = std::get<1>(power_mapping);

                return fields::detail::group_powers(mapped_terms, mapped_locations, lhs, rhs);
            } else {

                // multiply out brackets to try and find common factors
                constexpr auto lhs_addition_location = util::tuple_find<std::tuple<Fs...>, fields::detail::is_addition>::value;
                constexpr auto rhs_addition_location = util::tuple_find<std::tuple<Gs...>, fields::detail::is_addition>::value;

                if constexpr ((-1 != lhs_addition_location) && (-1 != rhs_addition_location)) {

                    const auto expanded_lhs = fields::detail::expand_terms<lhs_addition_location>(lhs);
                    const auto expanded_rhs = fields::detail::expand_terms<rhs_addition_location>(rhs);

                    const auto exact_map_result = fields::util::map_terms<util::is_same>(expanded_lhs, expanded_rhs);
                    const auto exact_common_terms_exist = std::get<2>(exact_map_result);

                    if constexpr (exact_common_terms_exist) {
                        return expanded_lhs + expanded_rhs;
                    } else {

                        const auto multiple_map_result = fields::util::map_terms<fields::detail::is_multiple>(expanded_lhs, expanded_rhs);
                        const auto multiple_common_terms_exist = std::get<2>(multiple_map_result);

                        if constexpr (multiple_common_terms_exist) {
                            return expanded_lhs + expanded_rhs;
                        } else {
                            return Addition<Multiplication<Fs...>, Multiplication<Gs...>>(lhs, rhs);
                        }
                    }
                } else {
                    return Addition<Multiplication<Fs...>, Multiplication<Gs...>>(lhs, rhs);
                }
            }
        }
    }
}

TEST_F(DerivativeTest, Time_Derivative_Of_Fields_Is_Correct) {
    using namespace fields::detail;
    using namespace literals;

    {
        constexpr auto H = [] (auto y) { return y; };

		constexpr auto y = Field{H};

        static_assert(d_dt(y) == H(y));

        static_assert(d_dt<0>(y) == y);
        static_assert(d_dt<1>(y) == d_dt(y));

        // only true because of the form of H
        static_assert(d_dt<1>(y) == y);
        static_assert(d_dt<10>(y) == y);
    }
    {
        constexpr auto H = [] (auto y) { return y + 5_c; };

		constexpr auto y = Field{H};

        static_assert(d_dt(y) == H(y));

        static_assert(d_dt<0>(y) == y);
        static_assert(d_dt<1>(y) == d_dt(y));

        // only true because of the form of H
        static_assert(d_dt<1>(y) == y + 5_c);
        static_assert(d_dt<10>(y) == y + 5_c);
    }
    {
        constexpr auto H = [] (auto y) { return 2_c*y; };

		constexpr auto y = Field{ H };

        static_assert(d_dt(y) == H(y));

        static_assert(d_dt<0>(y) == y);
        static_assert(d_dt<1>(y) == d_dt(y));

        // only true because of the form of H
        static_assert(d_dt<1>(y) == 2_c*y);
        static_assert(d_dt<2>(y) == 4_c*y);
        static_assert(d_dt<5>(y) == 32_c*y);
        static_assert(d_dt<10>(y) == 1024_c*y);
    }
    {
        constexpr auto H = [] (auto y) { return y*y; };

		constexpr auto y = Field{ H };

        static_assert(d_dt(y) == H(y));

        static_assert(d_dt<0>(y) == y);
        static_assert(d_dt<1>(y) == d_dt(y));

        // only true because of the form of H
        static_assert(d_dt<1>(y) == factorial(1_c)*(y^2_c));
        static_assert(d_dt<2>(y) == factorial(2_c)*(y^3_c));
        static_assert(d_dt<5>(y) == factorial(5_c)*(y^6_c));
        static_assert(d_dt<10>(y) == factorial(10_c)*(y^11_c));
        static_assert(d_dt<20>(y) == factorial(20_c)*(y^21_c));
    }
    {
        constexpr auto H = [] (auto) { return x + t; };

		constexpr auto y = Field{ H };

        static_assert(d_dt(y) == H(y));

        static_assert(d_dt<0>(y) == y);
        static_assert(d_dt<1>(y) == d_dt(y));

        // only true because of the form of H
        static_assert(d_dt(y) == x + t);
        static_assert(d_dt<2>(y) == 1);
        static_assert(d_dt<3>(y) == 0);
        static_assert(d_dt<5>(y) == 0);
        static_assert(d_dt<10>(y) == 0);
        static_assert(d_dt<20>(y) == 0);
    }
    {
        constexpr auto g = x + t*t;
        
        constexpr auto H = [g] (auto y) { return g*y; };

		constexpr auto y = Field{ H };

        static_assert(d_dt(y) == H(y));

        static_assert(d_dt<0>(y) == y);
        static_assert(d_dt<1>(y) == d_dt(y));

        static_assert(d_dt(y) == g*y);

        using G = std::decay_t<decltype(g)>;
        using Y = std::decay_t<decltype(y)>;
        {
            constexpr auto lhs = 2_c*t*y;
            constexpr auto rhs = g*y*g;
        
            static_assert(rhs == (g^2_c)*y);
            static_assert(rhs == y*(g^2_c));
            static_assert(rhs == g*g*y);
            static_assert(rhs == y*g*g);

            static_assert(std::is_same_v<std::decay_t<decltype(lhs)>, Multiplication<Constant<2>, Y, T>>);
            static_assert(std::is_same_v<std::decay_t<decltype(rhs)>, Multiplication<Power<G, 2>, Y>>);

            constexpr auto mapped_terms = util::map_terms<fields::util::is_same>(lhs, rhs);
            
            constexpr auto found_terms = std::get<0>(mapped_terms);
            constexpr auto term_locations = std::get<1>(mapped_terms);
            constexpr auto have_terms_been_found = std::get<2>(mapped_terms);

            static_assert(have_terms_been_found);
            static_assert(std::is_same_v<std::decay_t<decltype(found_terms)>, std::integer_sequence<Int, 1>>);
            static_assert(std::is_same_v<std::decay_t<decltype(term_locations)>, std::integer_sequence<Int, 1>>);
            

            static_assert(util::is_same<std::decay_t<decltype(lhs + rhs)>, Multiplication<Y, Addition<Multiplication<Constant<2>, T>, Power<G, 2>>>>::value);
            static_assert(util::is_same<std::decay_t<decltype(d_dt<2>(y))>, Multiplication<Y, Addition<Multiplication<Constant<2>, T>, Power<G, 2>>>>::value);
        }

        static_assert(d_dt<2>(y) == (2_c*t + (g^2_c))*y);
 
        {
            constexpr auto f = (2_c*t + (g^2_c));

            constexpr auto df_dt = d_dt(f);

            static_assert(df_dt == 2_c + 4_c*g*t);

            static_assert(d_dt(f*y) == df_dt*y + g*y*f);
            static_assert(d_dt(f*y) == (2_c + 4_c*g*t)*y + g*y*f);
            static_assert(d_dt(f*y) == (2_c + 4_c*g*t)*y + g*y*(2_c*t + (g^2_c)));
            static_assert(d_dt(f*y) == y*((2_c + 4_c*g*t) + g*(2_c*t + (g^2_c))));

            static_assert(d_dt(f*y) == y*(2_c + 4_c*g*t + g*(2_c*t + (g^2_c))));

            constexpr auto result = fields::detail::factorise(2_c + 4_c*g*t, g*(2_c*t + (g^2_c)));

            constexpr auto addition = 2_c + 4_c*g*t;
            constexpr auto multiplication = g*(2_c*t + (g^2_c));

            static_assert(fields::detail::search_for_term(addition, multiplication).index >= 0);

            static_assert(result == 2_c + g*(6_c*t + (g^2_c)));

            static_assert(d_dt(f*y) == y*result);

            static_assert(4_c*g*t + g*2_c*t + (g^3_c) == 6_c*g*t + (g^3_c));
            static_assert(4_c*g*t + g*(2_c*t + (g^2_c)) == g*(6_c*t + (g^2_c)));

            static_assert(2_c + 4_c*g*t + g*2_c*t + (g^3_c) == 2_c + 6_c*g*t + (g^3_c));
            static_assert(2_c + 4_c*g*t + g*(2_c*t + (g^2_c)) == 2_c + g*(6_c*t + (g^2_c)));
        }

        static_assert(d_dt<3>(y) == (2_c + 4_c*t*g + g*(2_c*t + (g^2_c)))*y);
        static_assert(d_dt<3>(y) == (2_c + g*(6_c*t + (g^2_c)))*y);

        {
            constexpr auto f = (2_c + g*(6_c*t + (g^2_c)));

            constexpr auto df_dt = d_dt(f);

            static_assert(d_dt(f) == d_dt(2_c + g*(6_c*t + (g^2_c))));
            static_assert(d_dt(f) == d_dt(g*(6_c*t + (g^2_c))));
            
            // static_assert(d_dt(f) == 6_c*(g + t*g*g) + 12_c*t*t);

            static_assert(g + g*g == g*(1_c + g));

            {
                constexpr auto rhs = t*g*g;
                using Rhs = std::decay_t<decltype(rhs)>;
                static_assert(-1 == util::contains<Rhs, G>::index);
                static_assert(-1 == fields::detail::contains_subtraction<Rhs, G>::index);
                static_assert(-1 == fields::detail::contains_multiple<Rhs, G>::index);
                static_assert(fields::detail::is_multiplication<Rhs>::value);
                static_assert(-1 == util::contains<G, Rhs>::index);
                
                constexpr auto power_location = util::tuple_index<Rhs, G, fields::detail::is_power>::value;
                
                static_assert(1 == power_location);

                constexpr auto remaining_terms = rhs.filter<power_location>();

                static_assert(std::is_same_v<std::decay_t<decltype(remaining_terms)>, fields::T>);

                static_assert(remaining_terms == t);
            }


            static_assert(g + t*g*g == g*(1_c + t*g));
            // static_assert(df_dt == 6_c*g*(1_c + t*g) + 12_c*t*t);

            static_assert(d_dt<4>(y) == d_dt(f*y));
            static_assert(d_dt<4>(y) == df_dt*y + f*d_dt(y));
            static_assert(d_dt<4>(y) == df_dt*y + f*g*y);
            static_assert(d_dt<4>(y) == (df_dt + f*g)*y);
            static_assert(d_dt<4>(y) == (df_dt + g*f)*y);
        }

        // TODO
        // static_assert(d_dt<4>(y) == ((g^4_c) + 8_c*g + 12_c*(g^2_c)*t + 12_c*t*t)*y);

    }
    {
		using fields::d_dx;
		constexpr auto H = [] (auto y) { return d_dx<2>(y); };

		constexpr auto y = Field{ H };

        static_assert(d_dt(y) == H(y));

        static_assert(d_dt<0>(y) == y);
        static_assert(d_dt<1>(y) == d_dt(y));

        static_assert(d_dt<1>(y) == d_dx<2>(y));
        static_assert(d_dt<2>(y) == d_dx<4>(y));
        static_assert(d_dt<5>(y) == d_dx<10>(y));
        static_assert(d_dt<10>(y) == d_dx<20>(y));
        static_assert(d_dt<20>(y) == d_dx<40>(y));
        static_assert(d_dt<100>(y) == d_dx<200>(y));
    }
}

TEST_F(DerivativeTest, Time_Derivative_Of_FieldVector_Is_Correct) {
	
	{
		// simple harmonic oscillator
		constexpr auto H = [](auto sin, auto cos) { return Vector{ cos, -sin }; };

		constexpr auto y = make_field_vector<2>(H);

		constexpr auto y0 = y.vector_of_fields();

		constexpr auto dydt = d_dt(y);

		static_assert(dydt == H(y0.get<0>(), y0.get<1>()));
		static_assert(dydt == Vector{ y0.get<1>(), -y0.get<0>() });


		static_assert(d_dt<2>(y) == H(dydt.get<0>(), dydt.get<1>()));


		static_assert(d_dt<3>(y) == -dydt);
		static_assert(d_dt<5>(y) == dydt);

		static_assert(d_dt<2>(y) == -y0);
		static_assert(d_dt<6>(y) == -y0);

		static_assert(d_dt<4>(y) == y0);
		static_assert(d_dt<8>(y) == y0);
	}

	{
		// schroedinger free particle, real and imaginary parts
		constexpr auto H = [](auto real, auto imag) { return Vector{ -d_dx<2>(imag), d_dx<2>(real) };  };

		constexpr auto phi = make_field_vector<2>(H);

		constexpr auto phi0 = phi.vector_of_fields();

		constexpr auto dphi_dt = d_dt(phi);

		static_assert(dphi_dt == H(phi0.get<0>(), phi0.get<1>()));

		static_assert(dphi_dt == Vector{ -d_dx<2>(phi0.get<1>()), d_dx<2>(phi0.get<0>()) });
		static_assert(dphi_dt == d_dx<2>(Vector{ -phi0.get<1>(), phi0.get<0>() }));

		constexpr auto px = phi0.get<0>();

		static_assert(d_dx(d_dx<2>(d_dx<5>(px))) == d_dx<8>(px));

		static_assert(d_dt(d_dx(d_dt(d_dx(px)))) == d_dx<2>(d_dt<2>(px)));

		static_assert(d_dt<2>(phi) == -d_dx<4>(phi0));

		static_assert(d_dt<4>(phi) == d_dx<8>(phi0));

		static_assert(d_dt<5>(phi) == d_dx<10>(Vector{ -phi0.get<1>(), phi0.get<0>() }));

		static_assert(d_dt<40>(phi) == d_dx<80>(phi0));
	}

}


// }   // test
}   // fields