#pragma once

#include <algorithm>
#include <array>
#include <cmath>
#include <limits>
#include <utility>

namespace fields {
namespace util {

// http://www2.units.it/ipl/students_area/imm2/files/Numerical_Recipes.pdf p400
template <typename Func>
auto bracket_minimum(Func&& f) {
	constexpr static auto eps = std::numeric_limits<double>::epsilon();
	constexpr static auto gold = 1.618034;
	constexpr static auto glimit = 100.0;
	const static auto tol = std::sqrt(eps);

	auto ax = -10.0, bx = 10.0;

	auto fa = f(ax);
	auto fb = f(bx);

	// switch roles of a and b so that we can go downhill from a to b
	if (fb > fa) {
		std::swap(ax, bx);
		std::swap(fa, fb);
	}
	
	// first guess for c
	auto cx = bx + gold*(bx - ax);
	auto fc = f(cx);

	// returns +1, 0, -1 depending on sign of val
	auto sign = [] (auto val) {
		using T = decltype(val);
		return (T{0} < val) - (val < T{0});
	};

	auto return_points = [&ax, &bx, &cx, &fa, &fb, &fc] {
		using T = std::decay_t<decltype(ax)>;
		using U = std::decay_t<decltype(fa)>;
		return std::make_pair(std::array<T, 3>{ax, bx, cx}, std::array<U, 3>{fa, fb, fc});
	};

	// keep returning here util we bracket
	while (fb > fc) {
		const auto r = (bx - ax)*(fb - fc);		
		const auto q = (bx - cx)*(fb - fa);

		// compute u by parabolic extrapolation from a, b, c. prevent division by zero
		auto u = bx - ((bx - cx)*q - (bx - ax)*r) / (2.0*std::max(std::abs(q - r), eps)*sign(q - r));
		auto fu = 0.0;

		const auto ulimit = bx + glimit*(cx - bx);	// limit how are u can go

		if ((bx - u)*(u - cx) > 0.0) {
			// parabolic u is between b and c: try it
			fu = f(u);
			if (fu < fc) {
				// got a minimum between b and c
				ax = bx;
				bx = u;
				fa = fb;
				fb = fu;
				return return_points();
			} else if (fu > fb) {
				// got a minimum between a and u
				cx = u;
				fc = fu;
				return return_points();
			}
			// parabolic fit was no use, use default magnification
			u = cx + gold*(cx - bx);
			fu = f(u);
		} else if ((cx - u)*(u - ulimit) > 0.0) {
			// parabolic fit is between c and its allowed limit
			fu = f(u);
			if (fu < fc) {
				bx = cx;
				cx = u;
				u = cx + gold*(cx - bx);
				fb = fc;
				fc = fu;
				fu = f(u);
			}
		} else if ((u - ulimit)*(ulimit - cx) > 0.0) {
			// limit parabolic u to a maximum allowed value
			u = ulimit;
			fu = f(u);
		} else {
			u = cx + gold*(cx - bx);
			fu = f(u);
		}
		// eleminate oldest point and continue
		ax = bx;
		bx = cx;
		cx = u;
		fa = fb;
		fb = fc;
		fc = fu;
	}

	return return_points();
}

}	// util
}	// fields
