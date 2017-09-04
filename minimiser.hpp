#pragma once

#include <algorithm>
#include <cmath>
#include <limits>
#include <utility>

namespace fields {
namespace util {

// http://www2.units.it/ipl/students_area/imm2/files/Numerical_Recipes.pdf p400
template <typename Func>
auto minimum(Func&& f) {
	constexpr static auto eps = std::numeric_limits<double>::epsilon();
	constexpr static auto tol = std::sqrt(eps);
	constexpr static auto gold = 1.618034;
	constexpr static auto glimit = 100.0;

	auto ax = -10.0, bx = 0.0, cx = 10.0;

	auto fa = f(ax);
	auto fb = f(bx);

	if (fb > fa) {
		std::swap(ax, bx);
		std::swap(fa, fb);
	}
	
	cx = bx + gold*(bx - ax);
	auto fc = f(cx);

	auto sign = [](auto val) {
		using T = decltype(val);
		return (T{0} < val) - (val < T{0});
	};

	while (fb > fc) {
		const auto r = (bx - ax)*(fb - fc);
		const auto q = (bx - cx)*(fb - fa);

		auto u = bx - ((bx - cx)*q - (bx - ax)*r) / (2.0*std::max(std::abs(q - r), eps)*sign(q - r));
		auto fu = 0.0;

		auto ulimit = bx + glimit*(cx - bx);

		if ((bx - u)*(u - cx) > 0.0) {
			fu = f(u);
			if (fu < fc) {
				ax = bx;
				bx = u;
				fa = fb;
				fb = fu;
				return;
			}
			else if (fu > fb) {
				cx = u;
				fc = fu;
				return;
			}
			u = cx + gold*(cx - bx);
			fu = f(u);
		}
		else if ((cx - u)*(u - ulim) > 0.0) {
			fu = f(u);
			if (fu < fc) {
				bx = cx;
				cx = u;
				u = cx + gold*(cx - bx);
				fb = fc;
				fc = fu;
				fu = f(u);
			}
		}
		else if ((u - ulimit)*(ulimit - cx) > 0.0) {
			u = ulimit;
			fu = f(u);
		}
		else {
			u = cx + gold*(cx - bx);
			fu = f(u);
		}
		ax = bx;
		bx = cx;
		cx = u;
		fa = fb;
		fb = fc;
		fc = fu;
	}

}

}	// util
}	// fields
