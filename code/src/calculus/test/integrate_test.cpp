#include "../integrate.hpp"

#include <gtest/gtest.h>

#include <cmath>
#include <iostream>

namespace fields {
namespace test {

// Class to test the integration of various equations
class IntegrateTest : public ::testing::Test {
 protected:

    // Compares the calculated numerical solution with the exact solution
    template <typename Hamiltonian, typename Phi, typename Exact>
    auto compare_with_exact_solution(const Hamiltonian& H, const Phi& phi_0, const Exact& phi_exact) {
        // integrate up to various upper limits
        for (auto T : integration_limits) {
            auto phi = integrate(H, phi_0, 0.0, T);

            // evaluate phi at various points on the real axis
            for (auto x : sample_range) {
                // TODO : check this! do it better!
                // each time step there is a relative error dt^4
                // total steps = T / dt
                // final relative error: T * dt ^3
                ASSERT_NEAR(phi(x), phi_exact(x, T), T*tol*std::abs(phi_exact(x, T))) << "Results:\t" << T << "\t" << x << "\t" << phi(x) << "\t" << phi_exact(x, T) << std::endl;
            }
        }
    }

    // T : the time to integrate to from 0, need to compare numerical phi(x) ~ phi_exact(x, T)
    // High values of T cause a stack overflow (manifesting as SEH exception) at the moment.
    const std::vector<double> integration_limits{0.0, 1.0, 5.0, 10.0, 25.0};

    // x : various points on the real axis to sample the solutions at
    const std::vector<double> sample_range{-100.0, -50.0, -10.0, 0.0, 2.72, 3.14, 10.0, 50.0, 100.0};

    constexpr static auto dt = 0.01;        // expected integration time step (needs to match that in integrate(...), need to make sure they match)
    constexpr static auto tol = dt*dt*dt;   // expected relative error of the final result
};

// --------------------------------------------
// Initial conditions: phi(x, 0)

// A quadratic
static auto phi_poly = [a = 1.0, b = 2.0, c = 3.0] (double x) {
    return a*x*x + b*x + c;
};

// An exponential
static auto phi_exp = [a = 5.0, k = 3.0] (double x) {
    return a*std::exp(k*x);
};

// --------------------------------------------

TEST_F(IntegrateTest, ZeroHamiltonian) {
}

}   // test
}   // fields