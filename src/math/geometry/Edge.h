#pragma once

#include <memory>
#include <utility>

#include "Vertex.h"

struct Edge {
	shared_ptr<Vertex> v0, v1;

	Edge(const shared_ptr<Vertex>& v0, const shared_ptr<Vertex>& v1) : v0(min(v0, v1)), v1(max(v0, v1)) {}

	bool operator==(const Edge& other) const {
		return v0 == other.v0 && v1 == other.v1;
	}

	bool operator<(const Edge& other) const {
		return tie(v0, v1) < tie(other.v0, other.v1);
	}
};

template<>
struct std::hash<Edge> {
	size_t operator()(const Edge& edge) const noexcept {
		// Combine the two vertex indices in a symmetric way
		const size_t h1 = std::hash<Vertex>()(*edge.v0);
		const size_t h2 = std::hash<Vertex>()(*edge.v1);

		// Combine the two hash values using XOR and bit shifting
		return h1 ^ h2 << 1;
	}
};
