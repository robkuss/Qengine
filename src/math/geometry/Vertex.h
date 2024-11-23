#pragma once

#include "math/vector/Vector3.h"

struct Vertex : Vector3 {
	explicit Vertex(const Vector3 &position) : Vector3(position.x, position.y, position.z), position(position) {}

	Vector3 position;
};