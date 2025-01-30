#pragma once

#include <memory>

#include "Vertex.h"

struct Triangle {
	shared_ptr<Vertex> v0, v1, v2;

	mutable Vector3 normal;	   // Face normal
	mutable Vector3 centroid;  // Center point

	Triangle(const shared_ptr<Vertex>& v0, const shared_ptr<Vertex>& v1, const shared_ptr<Vertex>& v2)
		: v0(v0), v1(v1), v2(v2), normal(Vector3::ZERO), centroid(Vector3::ZERO) {}

	Vector3 faceNormal() const;
	Vector3 center() const;

	bool isDegenerate() const;
};
