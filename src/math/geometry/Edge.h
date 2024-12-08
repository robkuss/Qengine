#pragma once

#include <memory>
#include <utility>

#include "Vertex.h"

struct Edge {
	std::shared_ptr<Vertex> v0;
	std::shared_ptr<Vertex> v1;

	Edge(const std::shared_ptr<Vertex>& v0, const std::shared_ptr<Vertex>& v1) : v0(std::min(v0, v1)), v1(std::max(v0, v1)) {}

	bool operator<(const Edge& other) const {
		return std::tie(v0, v1) < std::tie(other.v0, other.v1);
	}

};