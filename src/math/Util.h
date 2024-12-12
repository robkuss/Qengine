#pragma once

#include "vector/Vector2.h"
#include "vector/Vector3.h"
#include "vector/Vector4.h"
#include "geometry/Triangle.h"

#include <scene/Mode.h>


constexpr float EPSILON		= 1e-9f;					// Infinitesimal
constexpr double PI			= 3.14159265358979323846;	// Because for some reason there is no Pi in cmath

/** Degrees to radians */
inline double radians(const double deg) {
	return deg * PI / 180.0;
}

/** Radians to degrees */
inline double degrees(const double rad) {
	return rad * 180.0 / PI;
}

/** Vector3 to Vector4 */
inline Vector4 vector4(Vector3 v, const float w) {
	return {v.x, v.y, v.z, w};
}

/** Vector4 to Vector3 */
inline Vector3 vector3(Vector4 v) {
	return {v.x, v.y, v.z};
}

inline Vector3 clampDirection(const SubMode subMode) {
	switch (subMode) {
		case SubMode::X: return {1, 0, 0};
		case SubMode::Y: return {0, 1, 0};
		case SubMode::Z: return {0, 0, 1};
		default: return Vector3::ONE;
	}
}

/** Calculate the normal of a Triangle */
inline Vector3 faceNormal(const Triangle& t) {
	// Compute the two edge vectors
	const Vector3 e1 = t.v1->position - t.v0->position;
	const Vector3 e2 = t.v2->position - t.v0->position;

	// Compute the normal using the cross product
	return e1.cross(e2).normalize();
}

/** Calculate the centroid of a Triangle */
inline Vector3 centroid(const Triangle& t) {
	return (t.v0->position + t.v1->position + t.v2->position) / 3.0f;
}

/** Map 3D world space to 2D screen space */
Vector2 project(const Vector3& worldPoint, const std::array<int, 4>& viewport, const std::array<float, 16>& viewMatrix, const std::array<float, 16>& projMatrix);

/** Map 2D screen space to 3D world space */
Vector3 unproject(const Vector2& screenPoint, const std::array<int, 4>& viewport, const std::array<float, 16>& viewMatrix, const std::array<float, 16>& projMatrix);