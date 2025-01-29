#pragma once

#include <cmath>

#include <math/vector/Vector3.h>
#include <math/vector/Vector2.h>

struct Vertex {
	Vector3 position;
	Vector3 normal;		// Vertex normal
	Vector2 texCoords;	// Texture coordinates

	mutable bool isSelected;

	// Constructors
	explicit Vertex(const Vector3& position, const Vector2& texCoords) : position(position), normal(Vector3::ZERO), texCoords(texCoords), isSelected(false) {}
	explicit Vertex(const Vector3& position, const Vector3& normal, const Vector2& texCoords) : position(position), normal(normal), texCoords(texCoords), isSelected(false) {}
	explicit Vertex(const float x, const float y, const float z, const Vector2& texCoords) : position(Vector3(x, y, z)), normal(Vector3::ZERO), texCoords(texCoords), isSelected(false) {}

	// Equality operator without floating-point tolerance
	bool operator==(const Vertex& other) const {
		return std::abs(position.x - other.position.x) == 0 &&
			   std::abs(position.y - other.position.y) == 0 &&
			   std::abs(position.z - other.position.z) == 0;
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

// Specialize std::hash for Vertex
template <>
struct std::hash<Vertex> {
	size_t operator()(const Vertex& v) const noexcept {
		// Combine the hashes of position and normal vectors
		const size_t h1 = hash<Vector3>()(v.position);
		const size_t h2 = hash<Vector3>()(v.normal);

		// Combine the two hash values using XOR and bit shifting
		return h1 ^ h2 << 1;
	}
};
