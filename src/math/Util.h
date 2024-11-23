#pragma once

#include <numbers>

#include "math/vector/Vector3.h"
#include "math/vector/Vector4.h"

constexpr float EPSILON		= 1e-9f;					// Infinitesimal
constexpr double PI			= 3.14159265358979323846;	// Because for some reason there is no Pi in cmath

inline double radians(const double deg) {
	return deg * std::numbers::pi / 180.0;
}

inline Vector4 vector4(Vector3 v) {
	return {v.x, v.y, v.z, 1.0f};
}

inline Vector3 vector3(Vector4 v) {
	return {v.x, v.y, v.z};
}