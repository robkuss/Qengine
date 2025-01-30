#include "Triangle.h"

#include "math/Util.h"


/** Calculate the normal of a Triangle */
Vector3 Triangle::faceNormal() const {
	// Compute the two edge vectors
	const Vector3 e1 = v1->position - v0->position;
	const Vector3 e2 = v2->position - v0->position;

	// Compute the normal using the cross product
	return e1.cross(e2).normalize();
}

/** Calculate the centroid of a Triangle */
Vector3 Triangle::center() const {
	return (v0->position + v1->position + v2->position) / 3.0f;
}

/** Calculate whether a Triangle is degenerate (has 0 area) */
bool Triangle::isDegenerate() const {
	return (v1->position - v0->position).cross(v2->position - v0->position).length() < EPSILON;
}