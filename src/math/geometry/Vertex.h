#pragma once

#include <math/vector/Vector3.h>

struct Vertex : Vector3 {
	explicit Vertex(const Vector3& position) : Vector3(position.x, position.y, position.z) {}
	explicit Vertex(const float x, const float y, const float z) : Vector3(x, y, z) {}

	bool operator==(const Vertex& other) const {
		return x == other.x && y == other.y && z == other.z;
	}

	bool operator<(const Vertex& other) const {
		if (x != other.x) return x < other.x;
		if (y != other.y) return y < other.y;
		return z < other.z;
	}

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
};