#pragma once

#include "math/geometry/Vertex.h"

struct Triangle {
	Vertex v0, v1, v2;

	Triangle(const Vertex& v0, const Vertex& v1, const Vertex& v2) : v0(v0), v1(v1), v2(v2) {}
};
