#pragma once

#include <math/vector/Vector3.h>
#include <math/vector/Vector4.h>
#include <math/geometry/Triangle.h>
#include <scene/Mode.h>

constexpr float EPSILON		= 1e-9f;					// Infinitesimal
constexpr double PI			= 3.14159265358979323846;	// Because for some reason there is no Pi in cmath

inline double radians(const double deg) {
	return deg * PI / 180.0;
}

inline double degrees(const double rad) {
	return rad * 180.0 / PI;
}

inline Vector4 vector4(Vector3 v) {
	return {v.x, v.y, v.z, 1.0f};
}

inline Vector3 vector3(Vector4 v) {
	return {v.x, v.y, v.z};
}

/** Calculate the normal for a face in the Mesh */
inline Vector3 faceNormal(const Triangle& t) {
	// Compute the two edge vectors
	const Vector3 e1 = t.v1 - t.v0;
	const Vector3 e2 = t.v2 - t.v0;

	// Compute the normal using the cross product
	return e1.cross(e2).normalize();
}

inline Vector3 clampDirection(const SubMode subMode) {
	switch (subMode) {
		case SubMode::X: return {1, 0, 0};
		case SubMode::Y: return {0, 1, 0};
		case SubMode::Z: return {0, 0, 1};
		default: return Vector3::ONE;
	}
}