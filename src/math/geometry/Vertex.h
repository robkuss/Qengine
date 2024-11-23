#pragma once

#include "math/vector/Vector3.h"

struct Vertex : Vector3 {
	explicit Vertex(const Vector3 &position) : Vector3(position.x, position.y, position.z), position(position) {}

	Vector3 position;

	bool operator<(const Vertex& other) const {
		if (x != other.x) return x < other.x;
		if (y != other.y) return y < other.y;
		return z < other.z;
	}

	bool operator==(const Vertex& other) const {
		return x == other.x && y == other.y && z == other.z;
	}
};