#pragma once

#include "vector/Vector3.h"

struct Triangle {
	Vector3 v0, v1, v2;

	Triangle(const Vector3& v0, const Vector3& v1, const Vector3& v2) : v0(v0), v1(v1), v2(v2) {}
};
