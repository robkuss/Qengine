#pragma once

#include <algorithm>

#include "math/Util.h"


class Ray {
public:
	Vector3 origin, direction;

	// Constructor
	Ray(const Vector3 origin, const Vector3 direction) : origin(origin), direction(direction) {}

	[[nodiscard]] bool intersects(const Mesh& mesh) const;
	[[nodiscard]] bool intersects(const Triangle& t) const;
	[[nodiscard]] static bool intersects(const Vector2 &vertexPos, const Vector2 &mousePos, float tolerance);
};


inline bool Ray::intersects(const Mesh& mesh) const {
	return ranges::any_of(mesh.triangles, [this](const shared_ptr<Triangle>& triangle) {
		return intersects(*triangle);
	});
}

inline bool Ray::intersects(const Triangle &t) const {
	const auto v0 = t.v0->position;
	const auto v1 = t.v1->position;
	const auto v2 = t.v2->position;

	const auto e0 = v1 - v0;
	const auto e1 = v2 - v0;

	// Calculate the determinant
	const auto h = direction.cross(e1);
	const auto a = e0.dot(h);

	// If the determinant is close to 0, the ray lies in the plane of the triangle
	if (a > -EPSILON && a < EPSILON) {
		return false;
	}

	const auto f = 1.0f / a;
	const auto s = origin - v0;
	const auto u = f * s.dot(h);

	// Check if the intersection is outside the triangle
	if (u < 0.0f || u > 1.0f) {
		return false;
	}

	const auto q = s.cross(e0);

	// If the intersection lies outside the triangle
	if (const auto v = f * direction.dot(q); v < 0.0f || u + v > 1.0f) {
		return false;
	}

	// At this stage, we can compute p to find out where the intersection point is on the line
	const auto p = f * e1.dot(q);

	// Check if the ray intersects the triangle (p > 0 indicates a valid intersection)
	return p > EPSILON;
}

inline bool Ray::intersects(const Vector2& vertexPos, const Vector2& mousePos, const float tolerance) {
	return vertexPos.distance(mousePos) < tolerance;
}
