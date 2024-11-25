#pragma once

#include "math/geometry/Vertex.h"

struct Triangle {
	Vertex v0, v1, v2;

	Triangle(const Vertex& v0, const Vertex& v1, const Vertex& v2) : v0(v0), v1(v1), v2(v2) {}

	// Helper function to ensure that the vertices are sorted lexicographically
	void sortVertices() {
		if (v1 < v0) std::swap(v0, v1);
		if (v2 < v0) std::swap(v0, v2);
		if (v2 < v1) std::swap(v1, v2);
	}

	// Comparison function to allow lexicographical comparison of triangles
	bool operator==(const Triangle& other) const {
		// Create sorted versions of the triangles for comparison
		Triangle t1 = *this;
		Triangle t2 = other;
		t1.sortVertices();
		t2.sortVertices();
		return t1.v0 == t2.v0 && t1.v1 == t2.v1 && t1.v2 == t2.v2;
	}
};
