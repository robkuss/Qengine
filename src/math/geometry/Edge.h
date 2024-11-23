#pragma once

#include "math/geometry/Vertex.h"

struct Edge {
	Vertex v0, v1;

	// Constructor that ensures the vertices are ordered consistently
	Edge(const Vertex& v0, const Vertex& v1)
		: v0(v0 < v1 ? v0 : v1), v1(v0 < v1 ? v1 : v0) {}

	// Comparison operator for the set (lexicographical comparison of v0, v1)
	bool operator<(const Edge& other) const {
		if (v0 != other.v0) return v0 < other.v0;
		return v1 < other.v1;
	}
};