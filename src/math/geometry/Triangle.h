#pragma once

#include <memory>

#include "Vertex.h"

struct Triangle {
	std::shared_ptr<Vertex> v0, v1, v2;
	mutable Vector3 normal;	   // Face normal
	mutable Vector3 centroid;  // Center point

	Triangle(const std::shared_ptr<Vertex>& v0, const std::shared_ptr<Vertex>& v1, const std::shared_ptr<Vertex>& v2)
		: v0(v0), v1(v1), v2(v2), normal(Vector3::ZERO), centroid(Vector3::ZERO) {}

	/** Calculate the normal of a Triangle */
	Vector3 faceNormal() const {
		// Compute the two edge vectors
		const Vector3 e1 = v1->position - v0->position;
		const Vector3 e2 = v2->position - v0->position;

		// Compute the normal using the cross product
		return e1.cross(e2).normalize();
	}

	/** Calculate the centroid of a Triangle */
	Vector3 center() const {
		return (v0->position + v1->position + v2->position) / 3.0f;
	}

	/** Calculate whether a Triangle is degenerate (has 0 area) */
	bool isDegenerate() const {
		return (v1->position - v0->position).cross(v2->position - v0->position).length() < EPSILON;
	}
};
