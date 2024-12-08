#pragma once

#include "vector/Vector2.h"
#include "vector/Vector3.h"
#include "vector/Vector4.h"
#include "geometry/Triangle.h"

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

inline Vector3 clampDirection(const SubMode subMode) {
	switch (subMode) {
		case SubMode::X: return {1, 0, 0};
		case SubMode::Y: return {0, 1, 0};
		case SubMode::Z: return {0, 0, 1};
		default: return Vector3::ONE;
	}
}

inline Vector3 faceNormal(const Triangle& t) {
	// Compute the two edge vectors
	const Vector3 e1 = t.v1->position - t.v0->position;
	const Vector3 e2 = t.v2->position - t.v0->position;

	// Compute the normal using the cross product
	return e1.cross(e2).normalize();
}

/**
 * This function maps 3D world space to 2D screen space.
 * @param worldPoint The point in 3D world space to be projected to 2D screen space
 * @param viewport contains screen dimensions data
 * @param viewMatrix
 * @param projMatrix
 * @return the screen space coordinates for the given world position as a Vector2
 */
Vector2 project(const Vector3& worldPoint, const std::array<int, 4>& viewport, const std::array<float, 16>& viewMatrix, const std::array<float, 16>& projMatrix);

/**
 * This function maps 2D screen space to 3D world space.
 * @param screenPoint The point in 2D screen space to be projected to 3D world space
 * @param viewport contains screen dimensions data
 * @param viewMatrix
 * @param projMatrix
 * @return the world space coordinates for the given mouse position as a Vector3
 */
Vector3 unproject(const Vector2& screenPoint, const std::array<int, 4>& viewport, const std::array<float, 16>& viewMatrix, const std::array<float, 16>& projMatrix);