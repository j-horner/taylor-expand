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
    const std::vector<double> integration_limits{0.0, 1.0, 10.0, 25.0, 50.0};

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
static auto phi_exp = [k = 3.0] (double x) {
    return std::exp(k*x);
};

// --------------------------------------------

TEST_F(IntegrateTest, ZeroHamiltonian) {
    // Hamiltonian must return a function of x, therefore make a function which always returns 0
    auto zero = [] (auto /*x*/) { return 0.0; };

    // A Hamiltonian which provides no time evolution
    auto H = [zero] (auto /*phi*/, auto /*t*/) { return zero; };

    // Exact solutions are identically equal to their initial conditions
    auto phi_poly_exact = [] (auto x, auto /*t*/) { return phi_poly(x); };
    auto phi_exp_exact = [] (auto x, auto /*t*/) { return phi_exp(x); };

    compare_with_exact_solution(H, phi_poly, phi_poly_exact);
    compare_with_exact_solution(H, phi_exp, phi_exp_exact);
}

TEST_F(IntegrateTest, ConstantHamiltonian) {
    const auto v = 2.0;
    auto V = [v] (auto /*x*/) { return v; };

    // A constant Hamiltonian H = v
    auto H = [V] (auto /*phi*/, auto /*t*/) { return V; };

    // Exact solutions are x(t) = x_0 + v*t
    auto phi_poly_exact = [v] (auto x, auto t) { return phi_poly(x) + v*t; };
    auto phi_exp_exact = [v] (auto x, auto t) { return phi_exp(x) + v*t; };

    compare_with_exact_solution(H, phi_poly, phi_poly_exact);
    compare_with_exact_solution(H, phi_exp, phi_exp_exact);
}

TEST_F(IntegrateTest, TimeDependentHamiltonian) {
    using namespace std;

    const auto v = 2.0;
    const auto w = 5.0;

    auto V = [v] (auto /*x*/) { return v; };

    // An oscillating Hamiltonian
    auto H = [V, w] (auto /*phi*/, auto t) { return V*cos(w*t); };

    // Exact solutions are x(t) = x_0 + v*sin(w*t)/w
    auto phi_poly_exact = [v, w] (auto x, auto t) { return phi_poly(x) + v*sin(w*t)/w; };
    auto phi_exp_exact = [v, w] (auto x, auto t) { return phi_exp(x) + v*sin(w*t)/w; };

    compare_with_exact_solution(H, phi_poly, phi_poly_exact);
    compare_with_exact_solution(H, phi_exp, phi_exp_exact);
}

}   // test
}   // fields