#pragma once

#include <math/vector/Vector3.h>
#include <cmath>

struct Vertex {
	Vector3 position;
	Vector3 normal;	  // Vertex normal

	// Constructors
	explicit Vertex(const Vector3& position) : position(position), normal(Vector3::ZERO) {}
	explicit Vertex(const float x, const float y, const float z) : position(Vector3(x, y, z)), normal(Vector3::ZERO) {}

	// Equality operator with floating-point tolerance
	bool operator==(const Vertex& other) const {
		return std::abs(position.x - other.position.x) < EPSILON &&
			   std::abs(position.y - other.position.y) < EPSILON &&
			   std::abs(position.z - other.position.z) < EPSILON;
	}

	// Less-than operator for consistent ordering
	bool operator<(const Vertex& other) const {
		if (position.x != other.position.x) return position.x < other.position.x;
		if (position.y != other.position.y) return position.y < other.position.y;
		return position.z < other.position.z;
	}

	// Assignment operator
	Vertex& operator=(const Vector3& other) {
		position = other;
		return *this;
	}

	// In-place addition operator
	Vertex& operator+=(const Vector3& other) {
		position.x += other.x;
		position.y += other.y;
		position.z += other.z;
		return *this;
	}

	// In-place multiplication operator
	Vertex& operator*=(const Vector3& other) {
		position.x *= other.x;
		position.y *= other.y;
		position.z *= other.z;
		return *this;
	}

	// Distance to another vertex
	[[nodiscard]] float distance(const Vertex& other) const {
		return std::sqrt(
			(position.x - other.position.x) * (position.x - other.position.x) +
			(position.y - other.position.y) * (position.y - other.position.y) +
			(position.z - other.position.z) * (position.z - other.position.z));
	}
};