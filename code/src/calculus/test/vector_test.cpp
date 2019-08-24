#include "../vector.hpp"

#include <gtest/gtest.h>

namespace fields {
namespace test {

class VectorTest : public ::testing::Test {
 protected:
};

TEST_F(VectorTest, Construction_Works) {

	constexpr auto v = Vector{ 1, 2.0, "hello" };

	
	static_assert(std::is_same_v<std::decay_t<decltype(v)>, Vector<int, double, const char*>>);


}

}	// test
}	// fields