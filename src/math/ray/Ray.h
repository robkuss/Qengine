#pragma once

#include <algorithm>
#include <optional>

#include "math/Util.h"
#include "math/geometry/Triangle.h"
#include "math/vector/Vector3.h"
#include "objects/mesh/Mesh.h"


class Ray {
public:
	Vector3 origin, direction;

	// Constructor
	Ray(const Vector3 origin, const Vector3 direction) : origin(origin), direction(direction) {}

	[[nodiscard]] bool intersects(std::vector<Triangle>) const;
	[[nodiscard]] bool intersects(const Triangle&) const;
	[[nodiscard]] std::optional<Vector3> intersectWithPlane(Vector3, Vector3) const;
};


inline bool Ray::intersects(std::vector<Triangle> triangles) const {
	return std::ranges::any_of(triangles, [this](const Triangle& triangle) {
		return intersects(triangle);
	});
}

inline bool Ray::intersects(const Triangle &triangle) const {
	const auto vertex0 = triangle.v0;
	const auto vertex1 = triangle.v1;
	const auto vertex2 = triangle.v2;

	const auto edge1 = vertex1 - vertex0;
	const auto edge2 = vertex2 - vertex0;

	// Calculate the determinant
	const auto h = direction.cross(edge2);
	const auto a = edge1.dot(h);

	// If the determinant is close to 0, the ray lies in the plane of the triangle
	if (a > -EPSILON && a < EPSILON) {
		return false;
	}

	const auto f = 1.0f / a;
	const auto s = origin - vertex0;
	const auto u = f * s.dot(h);

	// Check if the intersection is outside the triangle
	if (u < 0.0f || u > 1.0f) {
		return false;
	}

	const auto q = s.cross(edge1);

	// If the intersection lies outside the triangle
	if (const auto v = f * direction.dot(q); v < 0.0f || u + v > 1.0f) {
		return false;
	}

	// At this stage, we can compute t to find out where the intersection point is on the line
	const auto t = f * edge2.dot(q);

	// Check if the ray intersects the triangle (t > 0 indicates a valid intersection)
	return t > EPSILON;
}

inline std::optional<Vector3> Ray::intersectWithPlane(const Vector3 planePoint, const Vector3 planeNormal) const {
	// Calculate the denominator of the intersection formula (dot product between ray direction and plane normal)
	const auto denominator = planeNormal.dot(direction);

	// If denominator is close to 0, the ray is parallel to the plane and there is no intersection
	if (denominator > -EPSILON && denominator < EPSILON) {
		return std::nullopt;
	}

	// Compute the distance from the ray origin to the plane
	const auto t = (planePoint - origin).dot(planeNormal / denominator);

	// If t is negative, the intersection is behind the ray's origin
	if (t < 0.0f) {
		return std::nullopt;
	}

	// Compute the intersection point as rayOrigin + t * rayDirection
	return origin + (direction * t);
}
