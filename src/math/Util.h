#ifndef MATH_UTIL
#define MATH_UTIL

#include <numbers>

constexpr float EPSILON		= 1e-9f;					// Infinitesimal
constexpr double PI			= 3.14159265358979323846;	// Because for some reason there is no Pi in cmath

inline double radians(const double deg) {
	return deg * std::numbers::pi / 180.0;
}

#endif // MATH_UTIL
