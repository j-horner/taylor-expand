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
    // Hamiltonian must return a function of x, therefore make a function which always returns 0
    auto zero = [] (auto /*x*/) { return 0.0; };

    // A Hamiltonian which provides no time evolution
    auto H = [zero] (auto /*phi*/, auto /*t*/) { return zero; };

    // Exact solutions are identically equal to their initial conditions
    auto compare = [this, &H] (auto& phi_0) {
        auto exact = [&phi_0] (auto x, auto /*t*/) { return phi_0(x); };

        compare_with_exact_solution(H, phi_0, exact);
    };

    compare(phi_poly);
    compare(phi_exp);
}

TEST_F(IntegrateTest, ConstantHamiltonian) {
    const auto v = 2.0;
    auto V = [v] (auto /*x*/) { return v; };

    // A constant Hamiltonian H = v
    auto H = [V] (auto /*phi*/, auto /*t*/) { return V; };

    // Exact solutions are x(t) = x_0 + v*t
    auto compare = [this, &H, v] (auto& phi_0) {
        auto exact = [&phi_0, v] (auto x, auto t) { return phi_0(x) + v*t; };

        compare_with_exact_solution(H, phi_0, exact);
    };

    compare(phi_poly);
    compare(phi_exp);
}

TEST_F(IntegrateTest, ParametricHamiltonian) {
    using namespace std;

    const auto v = 2.0;
    const auto w = 5.0;

    auto V = [v] (auto x) { return v*x*x; };

    // An oscillating Hamiltonian
    auto H = [V, w] (auto /*phi*/, auto t) { return V*cos(w*t); };

    // Exact solutions are x(t) = x_0 + v*sin(w*t)/w
    auto compare = [this, &H, &V, w] (auto& phi_0) {
        auto exact = [&phi_0, &V, w] (auto x, auto t) { return phi_0(x) + V(x)*sin(w*t)/w; };

        compare_with_exact_solution(H, phi_0, exact);
    };

    compare(phi_poly);
    compare(phi_exp);
}

TEST_F(IntegrateTest, BasicLinearHamiltonian) {
    const auto a = 3.0;

    auto H = [a] (auto phi, auto /*t*/) { return a*phi; };

    // Exact solutions exponentially grow
    auto compare = [this, &H, a] (auto& phi_0) {
        auto exact = [&phi_0, a] (auto x, auto t) { return phi_0(x)*std::exp(a*t); };

        compare_with_exact_solution(H, phi_0, exact);
    };

    compare(phi_poly);
    compare(phi_exp);
}

TEST_F(IntegrateTest, BasicQuadraticHamiltonian) {
    const auto a = 3.0;
    auto A = [a] (auto x) { return a*x*x; };

    auto H = [A] (auto phi, auto /*t*/) { return A*phi*phi; };

    // Exact solutions have a singularity, sample the solution at different values
    auto compare = [this, &H, A] (auto& phi_0) {
        auto phi_exact = [&phi_0, A] (auto x, auto t) { return phi_0(x)/(1.0 - A(x)*phi_0(x)*t); };

        // evaluate phi at various points on the real axis
        for (auto x = 0.0; x < 1.0; x += 0.1) {

            // solution only exist for: t < 1 / A(x)*phi_0(x)
            auto singularity_point = 1.0/(A(x)*phi_0(x));

            singularity_point *= 0.45;      // make sure we don't get too close to the singularity otherwise the integration starts to break down

            // check for division by 0, this means the singularity is at infinity so we can just stop the integration anywhere
            if (!std::isfinite(singularity_point)) {
                singularity_point = 10.0;
            }

            // integrate up to various upper limits
            for (auto T = 0.0; T < singularity_point; T += 0.1) {
                auto phi = integrate(H, phi_0, 0.0, T);

                ASSERT_NEAR(phi(x), phi_exact(x, T), T*tol*std::abs(phi_exact(x, T))) << "Results:\t" << T << "\t" << x << "\t" << phi(x) << "\t" << phi_exact(x, T) << std::endl;
            }
        }
    };

    compare(phi_poly);
    compare(phi_exp);
}

}   // test
}   // fields