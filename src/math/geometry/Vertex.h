#pragma once

#include <math/vector/Vector3.h>
#include <cmath>

struct Vertex : Vector3 {
	Vector3 normal;	  // Vertex normal

	// Constructors
	explicit Vertex(const Vector3& position) : Vector3(position.x, position.y, position.z), normal(0.0f, 0.0f, 0.0f) {}
	explicit Vertex(const float x, const float y, const float z) : Vector3(x, y, z), normal(0.0f, 0.0f, 0.0f) {}

	// Equality operator with floating-point tolerance
	bool operator==(const Vertex& other) const {
		return std::abs(x - other.x) < EPSILON &&
			   std::abs(y - other.y) < EPSILON &&
			   std::abs(z - other.z) < EPSILON;
	}

	// Less-than operator for consistent ordering
	bool operator<(const Vertex& other) const {
		if (x != other.x) return x < other.x;
		if (y != other.y) return y < other.y;
		return z < other.z;
	}

	// Assignment operator
	Vertex& operator=(const Vector3& other) {
		x = other.x;
		y = other.y;
		z = other.z;
		return *this;
	}

	// In-place addition operator
	Vertex& operator+=(const Vector3& other) {
		x += other.x;
		y += other.y;
		z += other.z;
		return *this;
	}

	// In-place multiplication operator
	Vertex& operator*=(const Vector3& other) {
		x *= other.x;
		y *= other.y;
		z *= other.z;
		return *this;
	}

	// Distance to another vertex
	[[nodiscard]] float distance(const Vertex& other) const {
		return std::sqrt(
			(x - other.x) * (x - other.x) +
			(y - other.y) * (y - other.y) +
			(z - other.z) * (z - other.z));
	}
};