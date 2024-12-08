#pragma once

#include <memory>

#include "Vertex.h"

struct Triangle {
	std::shared_ptr<Vertex> v0, v1, v2;

	Triangle(const std::shared_ptr<Vertex>& v0, const std::shared_ptr<Vertex>& v1, const std::shared_ptr<Vertex>& v2) : v0(v0), v1(v1), v2(v2) {}

	/** Calculates the centroid of this Triangle */
	[[nodiscard]] Vector3 centroid() const {
		return (*v0 + *v1 + *v2) / 3.0f;
	}
};
