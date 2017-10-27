#ifndef CALCULUS_INTEGRATE_HPP_
#define CALCULUS_INTEGRATE_HPP_

#include "fields.hpp"
#include "operators.hpp"
#include "shared_field.hpp"

#include "../util/function_traits.hpp"

#include <array>
#include <type_traits>
#include <utility>

namespace fields {

// helper functions to make sure a function is wrapped in a SharedField
namespace {
using namespace operators;

template <typename F>
auto wrap_function(F&& f) {
    return SharedField<F>(std::forward<F>(f));
}

template <typename F>
auto wrap_function(SharedField<F> f) {
    return f;
}
}   // namespace


template <typename Hamiltonian, typename Psi>
auto integrate(Hamiltonian&& H_, Psi&& psi_init, double t_0, double t_f) {
    using traits = util::function_traits<std::decay_t<Psi>>;

    using Ret = traits::result_type;
    using Arg = traits::arg<0>::type;

    auto psi = FieldList<Ret(Arg)>{};

    auto dt = 0.01;
    constexpr auto sixth = 1.0 / 6.0;

    auto t = t_0;

    auto H = [&H_] (auto&& psi, double t) {
        return H_(wrap_function(std::forward<decltype(psi)>(psi)), t);
    };

    psi.emplace_front(std::forward<Psi>(psi_init));

    constexpr auto tol = 1.0e-6;

    auto RKCK_step = [&H] (auto& psi_0, auto t, auto dt) {
        using namespace operators;

        // RKCK coefficients    https://en.wikipedia.org/wiki/Cash%E2%80%93Karp_method
        constexpr static auto c = std::array<double, 5>{0.2, 0.3, 1.0, 0.875};

        constexpr static auto b = std::array<double, 6>{(37.0 / 378.0), 0.0, (250.0 / 621.0), (125.0 / 594.0), 0.0, (512.0 / 1771.0)};

        constexpr static auto e = std::array<double, 6>{  std::get<0>(b) - (2825.0 / 27648.0),
                                                        std::get<1>(b) - 0.0,
                                                        std::get<2>(b) - (18575.0 / 48384.0),
                                                        std::get<3>(b) - (13525.0 / 55296.0),
                                                        std::get<4>(b) - (277.0 / 14336.0),
                                                        std::get<5>(b) - 0.25};

        constexpr static auto a = std::array<std::array<double, 5>, 5> { { { 0.2, 0.0, 0.0, 0.0, 0.0 },
                                                                         { (3.0 / 40.0), (9.0 / 40.0), 0.0, 0.0, 0.0 },
                                                                         { 0.3, -0.9, 1.2, 0.0, 0.0 },
                                                                         { (-11.0 / 54.0), 2.5, (-70.0 / 27.0), (35.0 / 27.0), 0.0 },
                                                                         { (1631.0 / 55296.0), (175.0 / 512.0), (575.0 / 13824.0), (44275.0 / 110592.0), (253.0 / 4096.0) }}};

        // RKCK
        auto K1 = H(psi_0, t)*dt;
        auto K2 = H(psi_0 + a[0][0] * K1, t + c[0] * dt)*dt;
        auto K3 = H(psi_0 + a[1][0] * K1 + a[1][1] * K2, t + c[1] * dt)*dt;
        auto K4 = H(psi_0 + a[2][0] * K1 + a[2][1] * K2 + a[2][2] * K3, t + c[2] * dt)*dt;
        auto K5 = H(psi_0 + a[3][0] * K1 + a[3][1] * K2 + a[3][2] * K3 + a[3][3] * K4, t + c[3] * dt)*dt;
        auto K6 = H(psi_0 + a[4][0] * K1 + a[4][1] * K2 + a[4][2] * K3 + a[4][3] * K4 + a[4][4] * K5, t + c[4] * dt)*dt;

        auto psi_1 = psi_0 + b[0] * K1 + b[1] * K2 + b[2] * K3 + b[3] * K4 + b[4] * K5 + b[5] * K6;

        auto error = e[0] * K1 + e[1] * K2 + e[2] * K3 + e[3] * K4 + e[4] * K5 + e[5] * K6;

        return std::make_pair(std::move(psi_1), std::move(error));
    };

    // fixed step size loop
    while (t < t_f) {
        // prevent overshooting past the integration boundary
        if (t + dt > t_f) {
            dt = t_f - t;
        }

        using namespace operators;

        const auto& psi_0 = psi.front();

        // some compile time sanity checks
        static_assert(std::is_lvalue_reference_v<decltype(psi_0)>, "Psi is NOT an lvalue reference!");

        static_assert(std::is_reference_v<decltype(H(psi_0, t))> == false, "H(psi) returns reference when it should always create a new object!");
        static_assert(std::is_reference_v<decltype(H(psi_0, t)*dt)> == false, "H(psi)*dt returns reference when it should always create a new object!");
        static_assert(std::is_reference_v<decltype(psi_0 + H(psi_0, t)*dt)> == false, "psi + H(psi)*dt returns reference when it should always create a new object!");

        // euler
        // auto psi_1 = psi_0 + H_(psi_0, t)*dt;

        // RK2
        // auto K1 = H_(psi_0, t)*dt;
        // auto psi_1	= psi_0 + H(psi_0 + 0.5*K1, t + 0.5*dt)*dt;

        // RK4
        auto K1 = H(psi_0, t)*dt;
        auto K2 = H(psi_0 + 0.5*K1, t + 0.5*dt)*dt;
        auto K3 = H(psi_0 + 0.5*K2, t + 0.5*dt)*dt;
        auto K4 = H(psi_0 + K3, t + dt)*dt;

        auto psi_1 = psi_0 + (K1 + 2.0*K2 + 2.0*K3 + K4)*sixth;

        // RKCK
        // auto step_pair = RKCK_step(psi_0, t, dt);
        // auto psi_1 = std::move(step_pair.first);

        psi.emplace_front(std::move(psi_1));

        t += dt;
    }

    return psi;
}


}   // fields

#endif  // CALCULUS_INTEGRATE_HPP_