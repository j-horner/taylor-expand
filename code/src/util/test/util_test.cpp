#include "../util.hpp"

#include <gtest/gtest.h>

namespace fields {
namespace util {
namespace test {

class UtilTest : public ::testing::Test {
protected:
    template <typename T, typename U>
    struct not_same {
        constexpr static auto value = false == std::is_same_v<T, U>;
    };

};

TEST_F(UtilTest, Pow_Is_Correct) {
    static_assert(util::pow(2, 3) == 8);
}

TEST_F(UtilTest, Factorial_Is_Correct) {
    static_assert(util::factorial(4) == 24);
}

TEST_F(UtilTest, Binomial_Is_Correct) {
    static_assert(util::binomial(4, 2) == 6);
}
TEST_F(UtilTest, Count) {
    static_assert(util::count<int, std::tuple<>>{} == 0);
    static_assert(util::count<int, std::tuple<int>>{} == 1);
    static_assert(util::count<int, std::tuple<int, int>>{} == 2);
    static_assert(util::count<int, std::tuple<int, int, int, int>>{} == 4);
    static_assert(util::count<int, std::tuple<int, int, int, int, int, int, int, int, int, int, int, int>>{} == 12);


    static_assert(util::count<char, std::tuple<int, char, int, int, char, int, int, int, char, char, int, int>>{} == 4);
}

TEST_F(UtilTest, IsPermutation) {
    static_assert(util::is_permutation<std::tuple<>, std::tuple<>>{});

    static_assert(util::is_permutation<std::tuple<int>, std::tuple<int>>{});
    static_assert(util::is_permutation<std::tuple<int, int>, std::tuple<int, int>>{});
    static_assert(util::is_permutation<std::tuple<int, int, int, int>, std::tuple<int, int, int, int>>{});
    static_assert(util::is_permutation<std::tuple<int, char, double>, std::tuple<double, int, char>>{});
    static_assert(util::is_permutation<std::tuple<int, char, unsigned int, double>, std::tuple<unsigned int, double, int, char>>{});
    static_assert(util::is_permutation<std::tuple<float, int, float, char, float, double>, std::tuple<double, int, char, float, float, float>>{});
    static_assert(util::is_permutation<std::tuple<bool, bool, bool, bool, int, char, double>, std::tuple<bool, double, bool, bool, int, char, bool>>{});

    static_assert(false == util::is_permutation<std::tuple<>, std::tuple<int>>{});
    static_assert(false == util::is_permutation<std::tuple<int, int>, std::tuple<int>>{});
    static_assert(false == util::is_permutation<std::tuple<int, int, int>, std::tuple<int, int, int, int>>{});
    static_assert(false == util::is_permutation<std::tuple<int, double>, std::tuple<double, int, char>>{});
    static_assert(false == util::is_permutation<std::tuple<int, char, unsigned int, double>, std::tuple<unsigned int, int, char>>{});
    static_assert(false == util::is_permutation<std::tuple<float, int, float, char, float, double>, std::tuple<double, int, char, float, float>>{});
    static_assert(false == util::is_permutation<std::tuple<bool, bool, bool, int, char, double>, std::tuple<bool, double, bool, bool, int, char, bool>>{});
}

TEST_F(UtilTest, FilterIndicies) {

    constexpr auto even_terms = util::filter_indicies(std::integer_sequence<Int, 0, -2, 10, 6, 5, 4, 3, 3, 7, 9, 5, 6, 7, 20>{}, [] (auto idx) { return (idx % 2) == 0; });
    static_assert(std::is_same_v<std::decay_t<decltype(even_terms)>, std::integer_sequence<Int, 0, -2, 10, 6, 4, 6, 20>>);
}

template <int Idx, typename Tuple, typename T>
constexpr auto is_tuple_type_equal(Tuple, T) { return std::is_same_v<std::tuple_element_t<Idx, Tuple>, T>; }

TEST_F(UtilTest, MapTerms) {

    {
        constexpr auto mapped_result = util::map_terms(std::tuple<int, char, unsigned int, double>{}, std::tuple<unsigned int, double, int, char>{});

        static_assert(is_tuple_type_equal<0>(mapped_result, std::integer_sequence<Int, 0, 1, 2, 3>{}));
        static_assert(is_tuple_type_equal<1>(mapped_result, std::integer_sequence<Int, 2, 3, 0, 1>{}));
    }

    {
        constexpr auto mapped_result = util::map_terms(std::tuple<char, unsigned int, double>{}, std::tuple<unsigned int, double, int, char>{});

        static_assert(is_tuple_type_equal<0>(mapped_result, std::integer_sequence<Int, 0, 1, 2>{}));
        static_assert(is_tuple_type_equal<1>(mapped_result, std::integer_sequence<Int, 3, 0, 1>{}));
    }

    {
        constexpr auto mapped_result = util::map_terms(std::tuple<int, char, unsigned int, double>{}, std::tuple<double, int, char>{});

        static_assert(is_tuple_type_equal<0>(mapped_result, std::integer_sequence<Int, 0, 1, 3>{}));
        static_assert(is_tuple_type_equal<1>(mapped_result, std::integer_sequence<Int, 1, 2, 0>{}));
    }

    {
        constexpr auto mapped_result = util::map_terms<not_same>(std::tuple<char>{}, std::tuple<char, char, char, int, char, char, char>{});

        static_assert(is_tuple_type_equal<0>(mapped_result, std::integer_sequence<Int, 0>{}));
        static_assert(is_tuple_type_equal<1>(mapped_result, std::integer_sequence<Int, 3>{}));
    }

}

}   // test
}   // util
}   // fields
