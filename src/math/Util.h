#pragma once

#include <objects/mesh/Mesh.h>
#include <viewport/scene/Mode.h>

#include "vector/Vector3.h"
#include "vector/Vector4.h"

class UIOptionBase;
class UITab;

class Object;


// Constants
constexpr float EPSILON		= 1e-12f;					// Infinitesimal
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

inline Vector3 clampDirection(const SubMode& subMode) {
	switch (subMode) {
		case SubMode::X: return {1, 0, 0};
		case SubMode::Y: return {0, 1, 0};
		case SubMode::Z: return {0, 0, 1};
		default: return Vector3::ONE;
	}
}

/** Map 3D world space to 2D screen space */
Vector2 project(const Vector3& worldPoint, const std::array<int, 4>* viewport, const std::array<float, 16>& viewMatrix, const std::array<float, 16>& projMatrix);

/** Map 2D screen space to 3D world space */
Vector3 unproject(const Vector2& screenPoint, const std::array<int, 4>* viewport, const std::array<float, 16>& viewMatrix, const std::array<float, 16>& projMatrix);


/** Helper to get all Meshes from a given vector of Objects */
inline std::vector<std::shared_ptr<Mesh>> filterMeshes(const std::vector<std::shared_ptr<Object>>& objects) {
	std::vector<std::shared_ptr<Mesh>> meshes;
	for (const auto& obj : objects) {
		if (const auto mesh = std::dynamic_pointer_cast<Mesh>(obj)) {
			meshes.emplace_back(mesh);
		}
	}
	return meshes;
}
