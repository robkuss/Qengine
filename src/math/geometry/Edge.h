#pragma once

#include "math/geometry/Vertex.h"

struct Edge {
	Edge(const Vertex& v0, const Vertex& v1) : v0(v0), v1(v1) {}

	Vertex v0, v1;
};