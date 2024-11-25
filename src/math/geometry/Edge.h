#pragma once

#include "math/geometry/Vertex.h"

struct Edge {
	Vertex v0, v1;

	// Constructor to ensure the smaller vertex comes first (based on the lexicographical comparison of Vertex)
	Edge(const Vertex& a, const Vertex& b) : v0(a), v1(b) {
		if (v1 < v0) std::swap(v0, v1); // Ensure that v0 is the smaller vertex TODO: This doesn't do the trick
	}

	// Lexicographical comparison based on v0 first, then v1 if necessary
	bool operator<(const Edge& other) const {
		return std::tie(v0, v1) < std::tie(other.v0, other.v1);
	}

	bool operator==(const Edge& other) const {
		return v0 == other.v0 && v1 == other.v1;
	}
};
